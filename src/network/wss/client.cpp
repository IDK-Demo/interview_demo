#include "client.h"

#include "base/logger/logger.h"
#include <random>
#include <openssl/evp.h>

namespace idk::net::wss {

std::optional<Client::Result>
Client::extract_payload() {
  ASSIGN_OR_RETURN(FrameHeader header, peek_header(stream), {});
  ASSIGN_OR_RETURN(std::ignore, stream.peek(header.header_size + header.size), {});
  ASSIGN_OR_RETURN(std::ignore, pop_header(stream), {});
  ASSIGN_OR_RETURN(auto payload, stream.pop(header.size), {});

  TRACE("Header {}", header);
  REQUIRE(!header.mask, "mask");
  REQUIRE((int) header.code <= 10, "code");

  return Result{header.code, payload};
}

std::string gen_sec_webSocket_key() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 255);

  std::array<uint8_t, 16> random_bytes{};
  for (auto& byte : random_bytes) {
    byte = distrib(gen);
  }

  std::array<char, 25> encoded{}; // 16 bytes -> 24 base64 chars + null terminator
  EVP_EncodeBlock(reinterpret_cast<unsigned char*>(encoded.data()),
                  random_bytes.data(),
                  random_bytes.size());
  return std::string(encoded.data());
}


void
Client::generate_upgrade_request() {
  REQUIRE_EQ(state, State::Init, "");
  std::string buffer = fmt::format("GET {} HTTP/1.1\r\n"
                                   "Host: {}\r\n"
                                   "Upgrade: websocket\r\n"
                                   "Connection: Upgrade, keep-alive\r\n"
                                   "Sec-WebSocket-Key: {}\r\n"
                                   "Sec-WebSocket-Version: 13\r\n\r\n",
                                   config.path, config.host, gen_sec_webSocket_key());

  DEBUG("{}", buffer);
  tls->push_bytes(buffer);
  tls->flush();
}

bool
Client::accept_handshake() {
  REQUIRE_EQ(state, State::HttpHandshake, "wrong state");
  auto tls_payload = stream.peek(stream.size()).value();

  DEBUG("WebSocket handshake response received: \n{}", base::to_string_view(tls_payload));

  static constexpr std::string_view prefix = "HTTP/1.1 101";
  static constexpr std::string_view suffix = "\r\n\r\n";

  auto pref_pos = stream.search(prefix);
  auto suf_pos = stream.search(suffix);

  if (!pref_pos || !suf_pos) {
    return false;
  }
  stream.pop(suf_pos.value() + suffix.size()).value();

  DEBUG("All handshakes done.");
  return true;
}

std::optional<base::ByteView>
Client::next_message() {
  auto tls_result = tls->receive();
  while (tls_result) {
    stream.shift();
    stream.push_bytes(tls_result.value());
    tls_result = tls->receive();
  }
  if (tls->is_handshake_complete()) {
    if (state == State::Init) {
      generate_upgrade_request();
      state = State::HttpHandshake;
    } else if (state == State::HttpHandshake) {
      if (!accept_handshake()) {
        return {};
      }
      state = State::Connected;
    }
  }

  ASSIGN_OR_RETURN(auto res, extract_payload(), {});
  if (res.code == OpCode::Text || res.code == OpCode::Binary) {
    return res.bytes;
  }
  DEBUG("Got message with code: {}", res.code);

  return {};
}

} // namespace idk
