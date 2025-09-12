#include "type.h"

namespace idk::net::tls12 {

HandshakeHash::HandshakeHash() {
  ctx_ = EVP_MD_CTX_new();
  EVP_DigestInit_ex(ctx_, EVP_sha256(), nullptr);
}

HandshakeHash::~HandshakeHash() {
  if (ctx_) {
    EVP_MD_CTX_free(ctx_);
  }
}

void HandshakeHash::update(base::ByteView data) {
  EVP_DigestUpdate(ctx_, data.data(), data.size());
}

std::array<uint8_t, 32> HandshakeHash::finalize() const {
  std::array<uint8_t, 32> digest{};
  EVP_MD_CTX* tmp_ctx = EVP_MD_CTX_new();
  EVP_MD_CTX_copy_ex(tmp_ctx, ctx_);
  unsigned int digest_len;
  EVP_DigestFinal_ex(tmp_ctx, digest.data(), &digest_len);
  EVP_MD_CTX_free(tmp_ctx);
  return digest;
}

} // namespace idk::net::tls12