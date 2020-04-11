#pragma once

#include <napi.h>
#include <xapian.h>

#include "exceptions.hh"

class MSetIterator : public Napi::ObjectWrap<MSetIterator> {
 public:
  MSetIterator(const Napi::CallbackInfo& info)
      : Napi::ObjectWrap<MSetIterator>(info) {
    auto env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1) {
      throw Napi::Error::New(env, "MSetIterator is required");
    }

    auto ptr = info[0].As<Napi::External<Xapian::MSetIterator>>().Data();
    it_ = *ptr;
  }

  static Napi::Value New(Napi::Env env, Xapian::MSetIterator& it) {
    auto eIt = Napi::External<Xapian::MSetIterator>::New(env, &it);
    return constructor.New({eIt});
  }

  Napi::Value get_rank(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(),
                             TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.get_rank()));
  }

  Napi::Value get_document(const Napi::CallbackInfo& info) {
    return Document::New(info.Env(),
                         TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.get_document()));
  }

  Napi::Value get_weight(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(),
                             TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.get_weight()));
  }

  Napi::Value get_collapse_key(const Napi::CallbackInfo& info) {
    return Napi::String::New(
        info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.get_collapse_key()));
  }

  Napi::Value get_collapse_count(const Napi::CallbackInfo& info) {
    return Napi::Number::New(
        info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.get_collapse_count()));
  }

  Napi::Value get_sort_key(const Napi::CallbackInfo& info) {
    return Napi::String::New(
        info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.get_sort_key()));
  }

  Napi::Value get_percent(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(),
                             TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.get_percent()));
  }

  Napi::Value get_docid(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(*it_));
  }

  Napi::Value get_description(const Napi::CallbackInfo& info) {
    return Napi::String::New(
        info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.get_description()));
  }

  static void Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "MSetIterator",
        {
            InstanceMethod("get_docid", &MSetIterator::get_docid),
            InstanceAccessor("docid", &MSetIterator::get_docid, nullptr),
            InstanceMethod("get_rank", &MSetIterator::get_rank),
            InstanceAccessor("rank", &MSetIterator::get_rank, nullptr),
            InstanceMethod("get_document", &MSetIterator::get_document),
            InstanceAccessor("document", &MSetIterator::get_document, nullptr),
            InstanceAccessor("doc", &MSetIterator::get_document, nullptr),
            InstanceMethod("toString", &MSetIterator::get_description),
            InstanceMethod("get_weight", &MSetIterator::get_weight),
            InstanceAccessor("weight", &MSetIterator::get_weight, nullptr),
            InstanceMethod("get_collapse_key", &MSetIterator::get_collapse_key),
            InstanceMethod("get_collapse_count",
                           &MSetIterator::get_collapse_count),
            InstanceMethod("get_sort_key", &MSetIterator::get_sort_key),
            InstanceMethod("get_percent", &MSetIterator::get_percent),
            InstanceMethod("get_description", &MSetIterator::get_description),

        });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("MSetIterator", func);
  }

 private:
  inline static Napi::FunctionReference constructor;
  Xapian::MSetIterator it_;
};

