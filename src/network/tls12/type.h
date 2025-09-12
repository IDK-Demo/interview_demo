#pragma once

#include <array>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include "base/macros/require.h"
#include "base/type/span.h"

namespace idk::net::tls12 {

class HandshakeHash {
public:
  HandshakeHash();
  ~HandshakeHash();

  void
  update(base::ByteView data);

  std::array<uint8_t, 32>
  finalize() const;

private:
  EVP_MD_CTX* ctx_;
};

struct TLSRandom {
  std::array<uint8_t, 32> value;
  void
  generate() {
    REQUIRE(RAND_bytes(value.data(), 32) == 1, "rand failed");
  }

  const std::array<uint8_t, 32>&
  data() const {
    return value;
  }

  uint8_t*
  data() {
    return value.data();
  }

  constexpr size_t
  size() const {
    return 32;
  }
};

} // namespace idk::net::tls12
