#pragma once

#include "model.h"

namespace idk::net::tls12 {

// clang-format off

constexpr RecordHeader kApplicationDataTlsRecordHeader = {
    .content_type = ContentType::ApplicationData, .version = kTls12, .size = 0};

constexpr TLSRecord kApplicationDataTlsRecord = {
    .header = kApplicationDataTlsRecordHeader,
};

constexpr ClientHelloMessage kClientHelloMessage = {
  .tls_header = {
    .content_type = ContentType::Handshake,
    .version = kTls12,
  },
  .handshake_header ={
    .handshake_type = HandshakeType::ClientHello,
  },
  .version = kTls12,
  .session_id_size = 0,
  .cipher_suites_size = sizeof(ClientHelloMessage::CipherSuites),
  .cipher_suites = {CipherSuite::ECDHE_RSA_WITH_AES_128_GCM_SHA256},
  .compression_methods_size = 1,
  .compression_method = 0
};

inline SNIExtensionMessage
make_sni_extensions(size_t sni_size) {
  return {
    .header = {
      .extension_type = TlsExtension::SNI,
      .extension_size = sizeof(SNIExtensionMessage) - sizeof(TLSExtensionHeader) + sni_size
    },
    .server_name_list_size = sizeof(SNIExtensionMessage) - sizeof(TLSExtensionHeader) + sni_size - 2,
    .name_type = 0,
    .hostname_size = sni_size
  };
}

constexpr Curve25519GroupExtension kCurve25519GroupExtension = {
  .header = {
    .extension_type = TlsExtension::SupportedGroups,
    .extension_size = sizeof(Curve25519GroupExtension) - sizeof(TLSExtensionHeader),
  },
  .size = 2,
  .groups = {Group::Curve25519}
};

constexpr TLSExtensionHeader kExtendedMasterSecretExtension = {
  .extension_type = TlsExtension::ExtendedMasterKey,
  .extension_size = 0
};

constexpr RenegotiationExtension kRenegotiationExtension = {
  .header = {
    .extension_type = TlsExtension::RenegotiationInfo,
    .extension_size = sizeof(RenegotiationExtension) - sizeof(TLSExtensionHeader),
  },
  .renegotiation_info_size = 0
};

constexpr SHA256RSAExtension kSHA256RSAExtension = {
  .header = {
    .extension_type = TlsExtension::SIGNATURE_ALGORITHMS,
    .extension_size = sizeof(SHA256RSAExtension) - sizeof(TLSExtensionHeader)
  },
  .algorithms = {{HashAlgo::SHA256, SignatureAlgo::RSA}}
};

constexpr auto kDefaultExtensions = std::make_tuple(
  kCurve25519GroupExtension, kExtendedMasterSecretExtension, kRenegotiationExtension, kSHA256RSAExtension);

constexpr ClientX25519KeyExchangeMessage kX25519KeyExchangeMessage = {
  .tls_header = {
    .content_type = ContentType::Handshake,
    .version = kTls12,
    .size = sizeof(HandshakeHeader) + sizeof(ECDHEClientKeyExchange) + X25519KeySize,
  },
  .handshake_header = {
    .handshake_type = HandshakeType::ClientKeyExchange,
    .size = sizeof(ECDHEClientKeyExchange) + X25519KeySize,
  },
  .ecdhe_key_exchange = {.pubkey_size = X25519KeySize},
  .client_eph_pub = {}
};

constexpr ChangeCipherSpecRecord kChangeCipherSpecRecord = {
  .header = {
    .content_type = ContentType::ChangeCipher,
    .version = kTls12,
    .size = 1,
  },
  .message = 1
};

constexpr FinishedHandshakeMessage kFinishedHandshakeMessage = {
  .header = {
    .handshake_type = HandshakeType::Finished,
    .size = sizeof(FinishedHandshakeMessage) - sizeof(HandshakeHeader),
  }};

constexpr FinishedHandshakeFullMessage kFinishedHandshakeFullMessage = {
  .tls_header = {
    .content_type = ContentType::Handshake, .version = kTls12
  },
  .msg = {.finished_handshake = kFinishedHandshakeMessage}};

constexpr CertificateHandshakeMessage kCertificateHandshakeMessage = {
  .tls_header = {
    .content_type = ContentType::Handshake,
    .version = kTls12,
    .size = sizeof(CertificateHandshakeMessage) - sizeof(RecordHeader)},
  .handshake = {
    .header ={
      .handshake_type = HandshakeType::Certificate,
      .size = sizeof(CertificateHandshakeMessage) - sizeof(RecordHeader) - sizeof(HandshakeHeader),
    },
    .cert_list_size = 0
  }
};

// clang-format on

} // namespace idk::net::tls12
