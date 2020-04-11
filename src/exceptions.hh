#pragma once

#include <napi.h>
#include <xapian.h>

#include <string>

#define TRY_CATCH_XAPIAN(env, ...)                                  \
  [&]() -> decltype(auto) {                                         \
    try {                                                           \
      return __VA_ARGS__;                                           \
    } catch (Xapian::Error & err) {                                 \
      throw Napi::Error::New(                                       \
          env, std::string(err.get_type()) + ": " + err.get_msg()); \
    }                                                               \
  }()

#define TRY_CATCH_XAPIAN_CALLBACK_INFO(...) \
  TRY_CATCH_XAPIAN(info.Env(), __VA_ARGS__)

