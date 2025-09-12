#pragma once

#include <memory>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

#include "base/macros/require.h"
#include "base/type/span.h"
#include "network/tls12/model.h"

namespace idk::net::tls12 {

struct EVPKeyDeleter {
  void
  operator()(EVP_PKEY* ptr) const noexcept;
};

struct X509Deleter {
  void
  operator()(X509* ptr) const noexcept;
};

struct EVPKeyCtxDeleter {
  void
  operator()(EVP_PKEY_CTX* ptr) const noexcept;
};

struct EVPCipherCtxDeleter {
  void
  operator()(EVP_CIPHER_CTX* ptr) const noexcept;
};

using EVPKeyPtr = std::unique_ptr<EVP_PKEY, EVPKeyDeleter>;
using X509Ptr = std::unique_ptr<X509, X509Deleter>;
using EVPKeyCtxPtr = std::unique_ptr<EVP_PKEY_CTX, EVPKeyCtxDeleter>;
using EVPCipherCtxPtr = std::unique_ptr<EVP_CIPHER_CTX, EVPCipherCtxDeleter>;

EVPKeyPtr
make_evp_key(EVP_PKEY* ptr);
X509Ptr
make_x509(X509* ptr);
EVPKeyCtxPtr
make_evp_key_ctx(EVP_PKEY_CTX* ptr);
EVPCipherCtxPtr
make_evp_cipher_ctx(EVP_CIPHER_CTX* ptr);

void
prf(base::ByteView secret, const char* label, base::ByteView seed, base::MutableByteView out);

EVPKeyPtr
generate_x25519_key();

constexpr int AES_128_KEY_SIZE = 16;
using AES128Key = std::array<uint8_t, AES_128_KEY_SIZE>;

class EncryptContext {
public:
  explicit EncryptContext(const AES128Key& key) : ctx(make_evp_cipher_ctx(EVP_CIPHER_CTX_new())) {
    REQUIRE_EQ(EVP_EncryptInit_ex(ctx.get(), EVP_aes_128_gcm(), nullptr, key.data(), nullptr), 1, "");
  }

  void init(const NOnce& nonce) {
    REQUIRE(EVP_EncryptInit_ex(ctx.get(), nullptr, nullptr, nullptr, (unsigned char*) &nonce), "");
  }

  int update(base::ByteView bytes, uint8_t* out = nullptr) {
    int len = 0;
    REQUIRE(EVP_EncryptUpdate(ctx.get(), out, &len, bytes.data(), bytes.size()), "");
    return len;
  }

  int final_ex(uint8_t* out) {
    int len = 0;
    REQUIRE(EVP_EncryptFinal_ex(ctx.get(), out, &len), "");
    return len;
  }

  int append_gcm_tag(uint8_t* out) {
    REQUIRE(EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_GET_TAG, kGcmTagSize, out),
            "");
    return kGcmTagSize;
  }
  EVPCipherCtxPtr ctx;
private:

};

} // namespace idk::net::tls12
