#pragma once
#include <napi.h>
#include <xapian.h>

#include <iostream>

#include "exceptions.hh"

class TermIterator : public Napi::ObjectWrap<TermIterator> {
 public:
  TermIterator(const Napi::CallbackInfo& info)
      : Napi::ObjectWrap<TermIterator>(info) {
    if (info.Length() > 1) {
      auto itPtr = reinterpret_cast<Xapian::TermIterator*>(
          info[0].As<Napi::External<Xapian::TermIterator>>().Data());
      auto endPtr = reinterpret_cast<Xapian::TermIterator*>(
          info[1].As<Napi::External<Xapian::TermIterator>>().Data());

      it_ = *itPtr;
      end_ = *endPtr;
    }
  }

  static Napi::Object New(Napi::Env env, Xapian::TermIterator it,
                          Xapian::TermIterator end) {
    auto eIt = Napi::External<decltype(it)>::New(env, &it);
    auto eEnd = Napi::External<decltype(end)>::New(env, &end);
    return constructor.New({eIt, eEnd});
  }

  Napi::Value term(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(*it_));
  }

  Napi::Value get_wdf(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(),
                             TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.get_wdf()));
  }

  Napi::Value get_termfreq(const Napi::CallbackInfo& info) {
    return Napi::Number::New(
        info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.get_termfreq()));
  }

  Napi::Value positionlist_count(const Napi::CallbackInfo& info) {
    return Napi::Number::New(
        info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.positionlist_count()));
  }

  static Napi::Object ToObject(Napi::Env env, Xapian::TermIterator& it) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("term", *it);
    obj.Set("wdf", it.get_wdf());
    obj.Set("termfreq", it.get_termfreq());
    obj.Set("positionlist_count", it.positionlist_count());
    return obj;
  }

  Napi::Value ToObject(const Napi::CallbackInfo& info) {
    return ToObject(info.Env(), it_);
  }

  Napi::Value get_iterator(const Napi::CallbackInfo& info) {
    auto env = info.Env();
    auto begin = Xapian::TermIterator(it_);
    auto end = end_;
    auto cb = Napi::Function::New(
        env, [begin, end](const Napi::CallbackInfo& info) mutable {
          auto env = info.Env();
          auto res = Napi::Object::New(env);
          if (begin != end) {
            auto tit = ToObject(env, begin);
            res.Set("value", tit);
            res.Set("done", false);
            TRY_CATCH_XAPIAN_CALLBACK_INFO(begin++);
            return res;
          }
          res.Set("done", true);
          return res;
        });

    auto res = Napi::Object::New(env);
    res.Set("next", cb);
    return res;
  }

  void iter(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Function cb = info[0].As<Napi::Function>();
    for (auto bit = it_; bit != end_; bit++) {
      cb.Call(env.Global(), {TermIterator::New(env, bit, end_)});
    }
  }

  void skip_to(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.skip_to(info[0].ToString()));
  }

  Napi::Value get_description(const Napi::CallbackInfo& info) {
    return Napi::String::New(
        info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(it_.get_description()));
  }

  static void Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "TermIterator",
        {
            InstanceMethod("term", &TermIterator::term),
            InstanceMethod("toObject", &TermIterator::ToObject),
            InstanceMethod("get_wdf", &TermIterator::get_wdf),
            InstanceMethod("get_termfreq", &TermIterator::get_termfreq),
            InstanceMethod("positionlist_count",
                           &TermIterator::positionlist_count),
            InstanceMethod("skip_to", &TermIterator::skip_to),
            InstanceMethod("get_description", &TermIterator::get_description),
            InstanceMethod("toString", &TermIterator::get_description),
            InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"),
                           &TermIterator::get_iterator),
            InstanceMethod("iter", &TermIterator::iter),
        });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("TermIterator", func);
  }

  operator const Xapian::TermIterator&() { return it_; }

 private:
  inline static Napi::FunctionReference constructor;
  Xapian::TermIterator it_;
  Xapian::TermIterator end_;
};

