#pragma once
#include "fmt/format.h"
#include "quill/bundled/fmt/format.h"
#include "quill/DeferredFormatCodec.h"
#include "quill/DirectFormatCodec.h"


#define IDK_LOGGER_REGISTER_SYNC_TYPE(T)                                                                              \
  template<>                                                                                                           \
  struct fmtquill::formatter<T> : fmt::formatter<T> {};                                                                \
                                                                                                                       \
  template<>                                                                                                           \
  struct quill::Codec<T> : DirectFormatCodec<T> {};

#define IDK_LOGGER_REGISTER_ASYNC_TYPE(T)                                                                             \
  template<>                                                                                                           \
  struct fmtquill::formatter<T> : fmt::formatter<T> {};                                                                \
                                                                                                                       \
  template<>                                                                                                           \
  struct quill::Codec<T> : DeferredFormatCodec<T> {};

#define IDK_LOGGER_REGISTER_SYNC_CONCEPT(Concept)                                                                     \
  template<Concept T>                                                                                                  \
  struct fmtquill::formatter<T> : fmt::formatter<T> {};                                                                \
                                                                                                                       \
  template<Concept T>                                                                                                  \
  struct quill::Codec<T> : DirectFormatCodec<T> {};

#define IDK_LOGGER_REGISTER_ASYNC_CONCEPT(Concept)                                                                     \
  template<Concept T>                                                                                                  \
  struct fmtquill::formatter<T> : fmt::formatter<T> {};                                                                \
                                                                                                                       \
  template<Concept T>                                                                                                  \
  struct quill::Codec<T> : DeferredFormatCodec<T> {};
