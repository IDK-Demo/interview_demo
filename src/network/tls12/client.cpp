#include "client.h"
#include <algorithm>
#include <arpa/inet.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/modes.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <string.h>

#include "base/macros/require.h"
#include "base/stream/byte_view_stream.h"
#include "base/stream/mutable_byte_view_stream.h"
#include "openssl.h"

#include "messages.h"
#include "model.h"

namespace idk::net::tls12 {

Client::Client(const std::string& sni_hostname_, tcp::Client tcp_client) :
    sni_hostname(sni_hostname_), tcp(std::move(tcp_client)) {
  tls_randoms.client.generate();
  tcp.connect();
}

void
Client::flush() {
  auto packet = tcp.get_send_buffer(tcp::Flags::PSH_ACK);

  auto write_stream = packet.tcp.payload_write_stream();
  auto& record = write_stream.push(kApplicationDataTlsRecord);
  record.explicit_iv = write_sequence_number;

  auto to_send = send_buffer.pop(send_buffer.size());
  REQUIRE(to_send, "");
  write_stream.push_bytes(to_send.value());

  size_t encrypted_len =
      sizeof(write_sequence_number) + encrypt({record.payload().data(), to_send->size()}, ContentType::ApplicationData);
  record.header.size = encrypted_len;
  tcp.send(std::move(packet), sizeof(RecordHeader) + encrypted_len);
}

std::optional<base::MutableByteView>
Client::receive() {
  if (handshake_state == HandshakeState::ClientHelloToSend && stream.empty()) [[unlikely]] {
    send_client_hello();
    handshake_state = HandshakeState::ServerHello;
    return {};
  }

  ASSIGN_OR_RETURN(auto& header, stream.peek<RecordHeader>(), {});
  REQUIRE_EQ(header.version, kTls12, "wrong protocol");
  uint16_t record_size = header.size.value();
  ASSIGN_OR_RETURN(std::ignore, stream.peek(sizeof(RecordHeader) + record_size), {});

  if (handshake_state != HandshakeState::ServerFinished) {
    process_handshake();
    if (stream.size() >= sizeof(RecordHeader)) {
      return receive();
    }
    return {};
  }
  REQUIRE(stream.pop<RecordHeader>(), "");
  REQUIRE(header.content_type == ContentType::ApplicationData, "Expected application data after handshake");

  ASSIGN_OR_RETURN(iv.explicit_iv, stream.pop<ExplicitIV>(), std::nullopt);
  ASSIGN_OR_RETURN(auto ciphertext, stream.pop(record_size - iv.explicit_iv.size()), std::nullopt);

  return decrypt(ciphertext);
}

void
Client::process_packet(tcp::PacketView tcp_packet) {
  tcp.process_packet(tcp_packet);
  stream.shift();
  stream.push_bytes(tcp_packet.payload());
  TRACE("-------------------TCP PACKET--------------------");
}

void
Client::process_handshake() {
  DEBUG("Connecting. state: {}", handshake_state);

  switch (handshake_state) {
    case HandshakeState::ServerHello: {
      REQUIRE(process_server_hello(), "");
      handshake_state = HandshakeState::ServerCertificate;
      break;
    }
    case HandshakeState::ServerCertificate: {
      ASSIGN_OR_RETURN(auto& header, stream.pop<RecordHeader>(), );
      ASSIGN_OR_RETURN(auto& hanshake_header, stream.peek<HandshakeHeader>(), );
      REQUIRE_EQ(hanshake_header.handshake_type, HandshakeType::Certificate, "");
      auto in_tls_payload = stream.pop(header.size.value()).value();
      handshake_hash.update(in_tls_payload);
      handshake_state = HandshakeState::ServerKeyExchange;
      break;
    }
    case HandshakeState::ServerKeyExchange: {
      if (process_server_key_exchange()) {
        handshake_state = HandshakeState::ServerHelloDone;
      }
      break;
    }
    case HandshakeState::ServerHelloDone: {
      if (process_server_hello_done()) {
        DEBUG("Server hello done TLS received");
        send_client_key_exchange_and_finished();
        handshake_state = HandshakeState::ClientFinished;
      }
      break;
    }
    case HandshakeState::ClientFinished: {
      if (process_server_finished()) {
        DEBUG("Server Finished");
        handshake_state = HandshakeState::ServerFinished;
      }
      break;
    }
    default:
      REQUIRE(false, "Unexpected flow");
  }
}

size_t
Client::encrypt(base::MutableByteView view, ContentType content_type) {
  enc_ctx->init(NOnce{keys.client_write_iv, write_sequence_number});
  AdditionalAuthData aad;
  aad.read_sequence_number = write_sequence_number;
  aad.app_content_type = content_type;
  aad.tls_version = kTls12;
  aad.size = view.size();
  enc_ctx->update(base::to_byte_view(aad));

  int len = enc_ctx->update(view, view.data());
  len += enc_ctx->final_ex(view.data() + len);
  len += enc_ctx->append_gcm_tag(view.data() + len);
  write_sequence_number++;
  return len;
}

base::MutableByteView
Client::decrypt(base::MutableByteView bytes) {
  REQUIRE_GT(bytes.size(), kGcmTagSize, "Ciphertext too small");

  REQUIRE(EVP_DecryptInit_ex(dec_ctx->ctx.get(), nullptr, nullptr, nullptr, (uint8_t*) &iv),
          "Failed to set IV in decryption context");

  AdditionalAuthData aad;
  aad.read_sequence_number = read_sequence_number++;
  aad.app_content_type = ContentType::ApplicationData;
  aad.tls_version = kTls12;
  aad.size = bytes.size();

  int len;
  REQUIRE(EVP_DecryptUpdate(dec_ctx->ctx.get(), nullptr, &len, (const unsigned char*) &aad, sizeof(aad)),
          "Error: Failed to set AAD");
  REQUIRE(EVP_DecryptUpdate(dec_ctx->ctx.get(), bytes.data(), &len, bytes.data(), bytes.size()),
          "Error: Decryption failed");

  return bytes.subspan(0, bytes.size() - kGcmTagSize);
}

bool
Client::process_server_finished() {
  auto read = stream.read_transaction();
  ASSIGN_OR_RETURN(auto& tls_header, read.pop<RecordHeader>(), false);
  REQUIRE_EQ(tls_header.content_type, ContentType::ChangeCipher, "unexpected message");
  ASSIGN_OR_RETURN(auto change_cipher_input, read.pop(tls_header.size.value()), false);
  REQUIRE_EQ(change_cipher_input[0], 1, "Incorrect ChangeCipher message");

  ASSIGN_OR_RETURN(auto& tls_header2, read.pop<RecordHeader>(), false);
  REQUIRE_EQ(tls_header2.content_type, ContentType::Handshake, "unexpected message");
  ASSIGN_OR_RETURN(std::ignore, read.pop(tls_header2.size.value()), false);
  read.commit();

  DEBUG("Received encrypted Finished message");
  return true;
}

void
Client::send_client_key_exchange_and_finished() {
  auto packet = tcp.get_send_buffer(net::tcp::Flags::PSH_ACK);
  auto write_stream = packet.tcp.payload_write_stream();
  DEBUG("Generating Client Key Exchange (ECDHE) and Finished messages");

  auto& record = write_stream.push(kX25519KeyExchangeMessage);
  size_t client_eph_pub_len = X25519KeySize;
  auto client_eph_key = generate_x25519_key();
  REQUIRE(EVP_PKEY_get_raw_public_key(client_eph_key.get(), record.client_eph_pub.data(), &client_eph_pub_len) > 0,
          "Failed to get raw client ephemeral public key");
  REQUIRE_EQ(client_eph_pub_len, X25519KeySize, "");

  auto server_eph_key =
      make_evp_key(EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, nullptr, server_eph_pubkey.data(), 32));
  REQUIRE(server_eph_key, "Failed to create EVP_PKEY from server ephemeral pubkey");

  auto derive_ctx = make_evp_key_ctx(EVP_PKEY_CTX_new(client_eph_key.get(), nullptr));
  REQUIRE(derive_ctx, "Failed to create derive context");
  REQUIRE(EVP_PKEY_derive_init(derive_ctx.get()) > 0, "EVP_PKEY_derive_init failed");
  REQUIRE(EVP_PKEY_derive_set_peer(derive_ctx.get(), server_eph_key.get()) > 0, "EVP_PKEY_derive_set_peer failed");

  std::array<uint8_t, X25519KeySize> shared_secret{};
  size_t shared_secret_len = shared_secret.size();
  REQUIRE(EVP_PKEY_derive(derive_ctx.get(), shared_secret.data(), &shared_secret_len) > 0, "EVP_PKEY_derive failed");
  REQUIRE(shared_secret_len == 32, "X25519 derived secret size != 32");
  DEBUG("ECDHE Shared Secret (premaster): {}", base::ByteView{shared_secret});

  handshake_hash.update(record.tls_header.payload());
  prf(shared_secret, "extended master secret", handshake_hash.finalize(), master_secret);
  prf(master_secret, "key expansion", base::to_byte_view(tls_randoms), {(uint8_t*) &keys, sizeof(Keys)});
  enc_ctx.emplace(keys.client_write_key);
  dec_ctx.emplace(keys.server_write_key);
  iv.server_write_iv = keys.server_write_iv;

  std::array<uint8_t, 12> verify_data;
  prf(master_secret, "client finished", handshake_hash.finalize(), verify_data);
  DEBUG("Verify Data: {}", base::ByteView{verify_data});

  write_stream.push(kChangeCipherSpecRecord);
  auto& msg = write_stream.push(kFinishedHandshakeFullMessage);
  msg.msg.finished_handshake.verify_data = verify_data;

  msg.msg.explicit_iv = write_sequence_number;
  size_t encrypted_size = sizeof(msg.msg.explicit_iv) +
                          encrypt(base::to_mutable_byte_view(msg.msg.finished_handshake), ContentType::Handshake);
  msg.tls_header.size = encrypted_size;
  write_stream.reserve(encrypted_size - sizeof(msg.msg));

  DEBUG("Encrypted size: {}", encrypted_size);
  DEBUG("Encrypted Finished message built, total size: {}", write_stream.size());
  tcp.send(std::move(packet), write_stream.size());
}


void
Client::send_client_hello() {
  auto packet = tcp.get_send_buffer(net::tcp::Flags::PSH_ACK);
  auto write_stream = packet.tcp.payload_write_stream();
  tls_randoms.client.generate();

  auto& client_hello = write_stream.push(kClientHelloMessage);
  client_hello.client_random = tls_randoms.client.value;

  size_t prev_size = write_stream.size();
  write_stream.push(make_sni_extensions(sni_hostname.size()));
  write_stream.push(sni_hostname);
  write_stream.push(kDefaultExtensions);
  client_hello.extensions_size = write_stream.size() - prev_size;

  client_hello.tls_header.size = write_stream.size() - sizeof(RecordHeader);
  client_hello.handshake_header.size = client_hello.tls_header.size.value() - sizeof(HandshakeHeader);

  handshake_hash.update({write_stream.data() + sizeof(RecordHeader), write_stream.size() - sizeof(RecordHeader)});
  DEBUG("ClientHello generated: size={}, SNI={}", write_stream.size(), sni_hostname);
  tcp.send(std::move(packet), write_stream.size());
}

bool
Client::process_server_hello() {
  ASSIGN_OR_RETURN(auto& header, stream.pop<RecordHeader>(), false);
  ASSIGN_OR_RETURN(std::ignore, stream.peek(header.size.value()), false);

  auto read = stream.read_transaction();
  ASSIGN_OR_RETURN(auto& hello, read.pop<ServerHelloMessage>(), false);
  ASSIGN_OR_RETURN(std::ignore, read.pop(hello.session_id_size), false);
  ASSIGN_OR_RETURN(auto& cipher_comp, read.pop<ServerHelloCipherAndCompression>(), false);
  REQUIRE_EQ(cipher_comp.cipher_suite.value(), CipherSuite::ECDHE_RSA_WITH_AES_128_GCM_SHA256, "");
  REQUIRE_EQ(cipher_comp.compression_method, 0, "Unexpected compression method {}", cipher_comp.compression_method);

  DEBUG("Server Random: {}", base::ByteView{hello.server_random});

  // INFO extensions are ignored
  // Manually popping instead of commit, because we have not read all.
  handshake_hash.update(stream.pop(header.size.value()).value());
  tls_randoms.server.value = hello.server_random;
  DEBUG("ServerHello processed");
  return true;
}

bool
Client::process_server_key_exchange() {
  ASSIGN_OR_RETURN(std::ignore, stream.pop<RecordHeader>(), {});

  auto read = stream.read_transaction();
  ASSIGN_OR_RETURN(auto& header, read.pop<HandshakeHeader>(), false);
  REQUIRE_EQ(header.handshake_type, HandshakeType::ServerKeyExchange, "Expected handshake type");

  ASSIGN_OR_RETURN(auto& ecdhe_params, read.pop<ServerKeyExchangeECDHE>(), false);
  REQUIRE_EQ(ecdhe_params.curve_type, CurveType::NamedCurve, "Expected curve type");
  REQUIRE_EQ(ecdhe_params.named_curve, Group::Curve25519, "Unexpected curve");

  uint8_t pubkey_len = ecdhe_params.pubkey_size;
  ASSIGN_OR_RETURN(auto pubkey_data, read.pop(pubkey_len), false);
  ASSIGN_OR_RETURN(auto& sig_info, read.pop<SignatureAlgorithmAndLength>(), false);
  uint16_t sig_alg = sig_info.signature_algorithm.value();
  uint16_t sig_len = sig_info.signature_size.value();
  ASSIGN_OR_RETURN(std::ignore, read.pop(sig_len), false); // INFO ignoring signature

  std::copy_n(pubkey_data.data(), pubkey_len, server_eph_pubkey.data());

  DEBUG("ServerKeyExchange parsed successfully (ephemeral pubkey saved)");
  DEBUG("Signature algorithm = {}, signature size = {}", sig_alg, sig_len);

  handshake_hash.update(read.commit());
  return true;
}

void
Client::send_client_certificate() {
  auto packet = tcp.get_send_buffer(tcp::Flags::PSH_ACK);
  auto write_stream = packet.tcp.payload_write_stream();

  auto& cert_msg = write_stream.push(kCertificateHandshakeMessage);
  handshake_hash.update({(uint8_t*) &cert_msg.handshake, sizeof(cert_msg.handshake)});

  DEBUG("Generated empty Certificate message");
  tcp.send(std::move(packet), write_stream.size());
}

bool
Client::process_server_hello_done() {
  ASSIGN_OR_RETURN(std::ignore, stream.pop<RecordHeader>(), {});

  auto read = stream.read_transaction();
  ASSIGN_OR_RETURN(auto& header, read.pop<HandshakeHeader>(), false);
  REQUIRE_EQ(header.handshake_type, HandshakeType::ServerHelloDone, "Unexpected hanshake type");
  ASSIGN_OR_RETURN(std::ignore, read.pop(header.size.value()), false);

  handshake_hash.update(read.commit());
  DEBUG("ServerHelloDone parsed");
  return true;
}

} // namespace idk::net::tls12
