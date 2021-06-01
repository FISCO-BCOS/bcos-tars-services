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

inline bcos::Error::Ptr toBcosError(const bcostars::Error &error) {
  if (error.errorCode == 0) {
    return nullptr;
  }

  auto bcosError =
      std::make_shared<bcos::Error>(error.errorCode, error.errorMessage);
  return bcosError;
}

inline bcos::Error::Ptr toBcosError(tars::Int32 ret) {
  if (ret == 0) {
    return nullptr;
  }

  auto bcosError = std::make_shared<bcos::Error>(ret, "TARS error!");
  return bcosError;
}

} // namespace bcostars