#pragma once

#include "Common.h"
#include <bcos-framework/libutilities/Error.h>

namespace bcostars {
inline Error toTarsError(const bcos::Error &error) {
  Error tarsError;
  tarsError.errorCode = error.errorCode();
  tarsError.errorMessage = error.errorMessage();

  return tarsError;
}

inline Error toTarsError(const bcos::Error::Ptr &error) {
  Error tarsError;

  if (error) {
    tarsError.errorCode = error->errorCode();
    tarsError.errorMessage = error->errorMessage();
  }

  return tarsError;
}

} // namespace bcostars