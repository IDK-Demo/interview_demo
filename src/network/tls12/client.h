#pragma once

#include <array>
#include <optional>
#include <stddef.h>
#include "base/stream/stream.h"
#include "base/type/span.h"
#include "network/tcp/client.h"
#include "openssl.h"

#include "model.h"
#include "type.h"

namespace idk::net::tls12 {

struct Client : base::NoCopy {
  Client(const std::string& sni_hostname, tcp::Client tcp_client);

  Client(Client&& other) = default;

  enum class HandshakeState {
    ClientHelloToSend = 0,
    ServerHello,
    ServerCertificate,
    ServerKeyExchange,
    ServerHelloDone,
    ClientKeyExchange,
    ClientFinished,
    ServerFinished
  };

  template<typename Object, typename... Args>
  Object&
  push(Args&&... args) {
    return send_buffer.push<Object>(std::forward<Args>(args)...);
  }

  template<typename Object>
  Object&
  push(const Object& obj) {
    return send_buffer.push(obj);
  }

  void
  push_bytes(base::ByteView bytes) {
    send_buffer.push_bytes(bytes);
  }

  void
  push_bytes(const std::string& s) {
    send_buffer.push_bytes(s);
  }

  void
  flush();

  std::optional<base::MutableByteView>
  receive();

  void
  process_packet(tcp::PacketView tcp_packet);

  HandshakeState
  state() const {
    return handshake_state;
  }

  bool
  is_handshake_complete() const {
    return handshake_state == HandshakeState::ServerFinished;
  }

  Connection
  get_connection() const {
    return tcp.get_connection();
  }

private:
  void
  process_handshake();

  base::MutableByteView
  decrypt(base::MutableByteView bytes);

  bool
  process_server_hello_done();

  void
  send_client_key_exchange_and_finished();

  size_t
  encrypt(base::MutableByteView view, ContentType content_type);

  bool
  process_server_finished();

  void
  send_client_hello();

  bool
  process_server_hello();

  bool
  process_server_key_exchange();

  void
  send_client_certificate();

  base::Stream send_buffer{20600};
  tcp::Client tcp;
  base::Stream stream{20600};

  struct TLSRandoms {
    TLSRandom server;
    TLSRandom client;
  };
  static_assert(alignof(TLSRandoms) == 1);

  TLSRandoms tls_randoms{};
  std::array<uint8_t, 2048> server_eph_pubkey{};
  std::array<uint8_t, 48> master_secret{};
  HandshakeHash handshake_hash;

  std::optional<EncryptContext> enc_ctx;
  std::optional<EncryptContext> dec_ctx;

  struct Keys {
    // INFO keys and ivs are placed in specific for prf order
    AES128Key client_write_key{};
    AES128Key server_write_key{};
    MessageIV client_write_iv{};
    MessageIV server_write_iv{};
  } keys;
  static_assert(alignof(TLSRandoms) == 1);

  uint64_t write_sequence_number = 0;
  uint64_t read_sequence_number = 0;

  IV iv{};

  std::string sni_hostname;
  HandshakeState handshake_state = HandshakeState::ClientHelloToSend;
};

} // namespace idk::net::tls12
