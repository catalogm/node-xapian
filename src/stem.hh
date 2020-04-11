#pragma once
#include <napi.h>
#include <xapian.h>

#include "exceptions.hh"

class Stem : public Napi::ObjectWrap<Stem> {
 public:
  Stem(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Stem>(info) {
    stem_ = TRY_CATCH_XAPIAN_CALLBACK_INFO(Xapian::Stem(info[0].ToString()));
  }

  Napi::Value is_none(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), stem_.is_none());
  }

  Napi::Value get_description(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), stem_.get_description());
  }

  Napi::Value call(const Napi::CallbackInfo& info) {
    return Napi::String::New(
        info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(stem_(info[0].ToString())));
  }

  static void Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "Stem",
        {
            InstanceMethod("is_none", &Stem::is_none),
            InstanceMethod("get_description", &Stem::get_description),
            InstanceMethod("call", &Stem::call),
        });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("Stem", func);
  }

  operator const Xapian::Stem&() { return stem_; }

 private:
  inline static Napi::FunctionReference constructor;
  Xapian::Stem stem_;
};

