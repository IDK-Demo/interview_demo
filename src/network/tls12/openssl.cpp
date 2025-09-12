#include "openssl.h"
#include <cstring>
#include "base/logger/logger.h"
#include "base/type/enum.h"

namespace idk::net::tls12 {

void
EVPKeyDeleter::operator()(EVP_PKEY* ptr) const noexcept {
  if (ptr) {
    EVP_PKEY_free(ptr);
  }
}

void
X509Deleter::operator()(X509* ptr) const noexcept {
  if (ptr) {
    X509_free(ptr);
  }
}

void
EVPKeyCtxDeleter::operator()(EVP_PKEY_CTX* ptr) const noexcept {
  if (ptr) {
    EVP_PKEY_CTX_free(ptr);
  }
}

void
EVPCipherCtxDeleter::operator()(EVP_CIPHER_CTX* ptr) const noexcept {
  if (ptr) {
    EVP_CIPHER_CTX_free(ptr);
  }
}

EVPKeyPtr
make_evp_key(EVP_PKEY* ptr) {
  REQUIRE(ptr, "Failed to extract public key from certificate");
  return EVPKeyPtr(ptr);
}

X509Ptr
make_x509(X509* ptr) {
  REQUIRE(ptr, "Failed to parse X.509 certificate");
  return X509Ptr(ptr);
}

EVPKeyCtxPtr
make_evp_key_ctx(EVP_PKEY_CTX* ptr) {
  REQUIRE(ptr, "nullptr");
  return EVPKeyCtxPtr(ptr);
}

EVPCipherCtxPtr
make_evp_cipher_ctx(EVP_CIPHER_CTX* ptr) {
  REQUIRE(ptr, "nullptr");
  return EVPCipherCtxPtr(ptr);
}

void prf(base::ByteView secret, const char* label, base::ByteView seed, base::MutableByteView out) {
  auto pctx = make_evp_key_ctx(EVP_PKEY_CTX_new_id(EVP_PKEY_TLS1_PRF, NULL));
  REQUIRE(pctx, "Failed to create TLS1-PRF context");

  REQUIRE(EVP_PKEY_derive_init(pctx.get()) > 0, "Failed to initialize PRF derivation");
  REQUIRE(EVP_PKEY_CTX_set_tls1_prf_md(pctx.get(), EVP_sha256()) > 0, "Failed to set PRF digest");
  REQUIRE(EVP_PKEY_CTX_set1_tls1_prf_secret(pctx.get(), secret.data(), secret.size()) > 0,
          "Failed to set PRF secret");
  REQUIRE(EVP_PKEY_CTX_add1_tls1_prf_seed(pctx.get(), reinterpret_cast<const unsigned char*>(label), strlen(label)) > 0,
          "Failed to add PRF label");
  REQUIRE(EVP_PKEY_CTX_add1_tls1_prf_seed(pctx.get(), seed.data(), seed.size()) > 0,
          "Failed to add PRF seed");

  size_t outlen = out.size();
  REQUIRE(EVP_PKEY_derive(pctx.get(), out.data(), &outlen) > 0, "Failed to derive key material");
  DEBUG("PRF Output: {}", base::ByteView{out.data(), outlen});
}

EVPKeyPtr
generate_x25519_key() {
  auto pctx = make_evp_key_ctx(EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, NULL));
  REQUIRE(pctx, "EVP_PKEY_CTX_new_id failed");
  REQUIRE(EVP_PKEY_keygen_init(pctx.get()) > 0, "EVP_PKEY_keygen_init failed");

  EVP_PKEY* pkey = NULL;
  REQUIRE(EVP_PKEY_keygen(pctx.get(), &pkey) > 0, "EVP_PKEY_keygen failed");
  return make_evp_key(pkey);
}

} // namespace idk::net::tls12
