#pragma once

#include <array>
#include "base/macros/require.h"
#include "base/type/uint24.h"
#include "network/type/big_endian.h"

#include "type.h"

namespace idk::net::tls12 {

#pragma pack(push, 1)
struct Version {
  static constexpr bool kLoggable = true;
  uint8_t major;
  uint8_t minor;

  auto
  operator<=>(const Version&) const = default;
};

constexpr Version kTls12 = {3, 3};
constexpr int kGcmIvSize = 4;
constexpr int kGcmTagSize = 16;
constexpr int kExplicitIvSize = 8;

using ExplicitIV = std::array<uint8_t, kExplicitIvSize>;
using MessageIV = std::array<uint8_t, kGcmIvSize>;

struct IV {
  MessageIV server_write_iv;
  ExplicitIV explicit_iv;
};

enum class CipherSuite : uint16_t { ECDHE_RSA_WITH_AES_128_GCM_SHA256 = 0xC02F };

enum class TlsExtension : uint16_t {
  SNI = 0x0000,
  SupportedGroups = 0x000A,
  SIGNATURE_ALGORITHMS = 0x000D,
  ExtendedMasterKey = 0x0017,
  RenegotiationInfo = 0xFF01,
};

enum class ContentType : uint8_t {
  ChangeCipher = 0x14,
  Handshake = 0x16,
  ApplicationData = 0x17,
};

struct RecordHeader {
  static constexpr bool kLoggable = true;

  ContentType content_type;
  Version version;
  BE<uint16_t> size{0};

  base::MutableByteView
  payload() {
    return {reinterpret_cast<uint8_t*>(this + 1), size.value()};
  }
};

static_assert(sizeof(RecordHeader) == 5);

struct TLSRecord {
  RecordHeader header;
  BE<uint64_t> explicit_iv;

  base::MutableByteView
  payload() {
    return {reinterpret_cast<uint8_t*>(this + 1), header.size.value()};
  }
};

enum class HandshakeType : uint8_t {
  HelloRequest = 0x00,
  ClientHello = 0x01,
  Certificate = 0x0B,
  ServerKeyExchange = 0x0C,
  CertificateRequest = 0x0D,
  ServerHelloDone = 0x0E,
  ClientKeyExchange = 0x10,
  Finished = 0x14
};

struct HandshakeHeader {
  static constexpr bool kLoggable = true;

  HandshakeType handshake_type;
  base::UInt24 size{0};
};

static_assert(sizeof(HandshakeHeader) == 4);

struct ClientHelloMessage {
  using CipherSuites = std::array<BE<CipherSuite>, 1>;

  RecordHeader tls_header;
  HandshakeHeader handshake_header;

  Version version;
  std::array<uint8_t, 32> client_random;
  uint8_t session_id_size;
  BE<uint16_t> cipher_suites_size;
  CipherSuites cipher_suites;
  uint8_t compression_methods_size;
  uint8_t compression_method;
  BE<uint16_t> extensions_size;
};

struct TLSExtensionHeader {
  BE<TlsExtension> extension_type;
  BE<uint16_t> extension_size;
};

struct SNIExtensionMessage {
  TLSExtensionHeader header;
  BE<uint16_t> server_name_list_size;
  uint8_t name_type;
  BE<uint16_t> hostname_size;
};

enum class Group : uint16_t { Curve25519 = 0x1D };
struct Curve25519GroupExtension {
  TLSExtensionHeader header;
  BE<uint16_t> size;
  std::array<BE<Group>, 1> groups;
};

struct RenegotiationExtension {
  TLSExtensionHeader header;
  uint8_t renegotiation_info_size;
};

enum class HashAlgo : uint8_t { SHA256 = 0x04 };

enum class SignatureAlgo : uint8_t { RSA = 0x01 };

struct SHA256RSAExtension {
  struct Pair {
    HashAlgo hash_algo;
    SignatureAlgo signature_algo;
  };

  TLSExtensionHeader header;
  BE<uint16_t> size;
  std::array<Pair, 1> algorithms;
};

struct ECDHEClientKeyExchange {
  uint8_t pubkey_size;
};

constexpr size_t X25519KeySize = 32;

struct ClientX25519KeyExchangeMessage {
  RecordHeader tls_header;
  HandshakeHeader handshake_header;
  ECDHEClientKeyExchange ecdhe_key_exchange;
  std::array<uint8_t, X25519KeySize> client_eph_pub;
};

struct ChangeCipherSpecRecord {
  RecordHeader header;
  uint8_t message;
};
static_assert(sizeof(ChangeCipherSpecRecord) == 6);

struct FinishedHandshakeMessage {
  HandshakeHeader header;
  std::array<uint8_t, 12> verify_data;
};

static_assert(sizeof(FinishedHandshakeMessage) == 16);

struct FinishedHandshakeFullMessage {
  struct Message {
    BE<uint64_t> explicit_iv;
    FinishedHandshakeMessage finished_handshake;
  };

  RecordHeader tls_header;
  Message msg;
};

struct CertificateHandshakeMessage {
  struct Handshake {
    HandshakeHeader header;
    base::UInt24 cert_list_size;
  };
  RecordHeader tls_header;
  Handshake handshake;
};

struct ServerHelloMessage {
  uint8_t handshake_type;
  base::UInt24 size;
  Version version;
  std::array<uint8_t, 32> server_random;
  uint8_t session_id_size;
};

static_assert(sizeof(ServerHelloMessage) == 39);

struct ServerHelloCipherAndCompression {
  BE<CipherSuite> cipher_suite;
  uint8_t compression_method;
  BE<uint16_t> extensions_size;
};

static_assert(sizeof(ServerHelloCipherAndCompression) == 5);

struct TLSExtension {
  BE<uint16_t> type;
  BE<uint16_t> size;
};

enum class CurveType : uint8_t { NamedCurve = 0x03 };

struct ServerKeyExchangeECDHE {
  CurveType curve_type;
  BE<Group> named_curve;
  uint8_t pubkey_size;
};

struct SignatureAlgorithmAndLength {
  BE<uint16_t> signature_algorithm;
  BE<uint16_t> signature_size;
};
struct AdditionalAuthData {
  BE<uint64_t> read_sequence_number;
  ContentType app_content_type;
  Version tls_version;
  BE<uint16_t> size;
};
static_assert(sizeof(AdditionalAuthData) == 13);

struct NOnce {
  MessageIV fixed_iv{};
  BE<uint64_t> explicit_iv;
};

struct CertHeader {
  base::UInt24 chain_len;
  base::UInt24 len;
};

#pragma pack(pop)

} // namespace idk::net::tls12
