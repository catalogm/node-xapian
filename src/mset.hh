#pragma once

#include <napi.h>
#include <xapian.h>

#include "exceptions.hh"
#include "msetiterator.hh"
#include "stem.hh"

class MSet : public Napi::ObjectWrap<MSet> {
 public:
  MSet(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MSet>(info) {
    auto env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1) {
      throw Napi::Error::New(env, "mset is required");
    }

    auto msetPtr = info[0].As<Napi::External<Xapian::MSet>>().Data();
    mset_ = *msetPtr;
  }

  static Napi::Value New(Napi::Env env, Xapian::MSet mset) {
    auto eMSet = Napi::External<Xapian::MSet>::New(env, &mset);
    return constructor.New({eMSet});
  }

  Napi::Value get_matches_estimated(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(
                                             mset_.get_matches_estimated()));
  }

  Napi::Value snippet(const Napi::CallbackInfo& info) {
    size_t length = 500;
    Xapian::Stem stem;
    uint32_t flags = Xapian::MSet::SNIPPET_BACKGROUND_MODEL |
                     Xapian::MSet::SNIPPET_EXHAUSTIVE;
    std::string hi_start = "<b>";
    std::string hi_end = "</b>";
    std::string omit = "...";
    switch (info.Length()) {
      case 7:
        omit = info[6].ToString();
        [[fallthrough]];
      case 6:
        hi_end = info[5].ToString();
        [[fallthrough]];
      case 5:
        hi_start = info[4].ToString();
        [[fallthrough]];
      case 4:
        flags = info[3].ToNumber();
        [[fallthrough]];
      case 3: {
        Stem* stemmer =
            Napi::ObjectWrap<Stem>::Unwrap(info[2].As<Napi::Object>());
        stem = *stemmer;
      }
        [[fallthrough]];
      case 2:
        length = info[1].IsBigInt()
                     ? info[1].As<Napi::BigInt>().Uint64Value(nullptr)
                     : static_cast<uint64_t>(info[1].ToNumber().Uint32Value());
    }
    return Napi::String::New(
        info.Env(),
        TRY_CATCH_XAPIAN_CALLBACK_INFO(mset_.snippet(
            info[0].ToString(), length, stem, flags, hi_start, hi_end, omit)));
  }

  Napi::Value size(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(),
                             TRY_CATCH_XAPIAN_CALLBACK_INFO(mset_.size()));
  }

  Napi::Value empty(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(),
                              TRY_CATCH_XAPIAN_CALLBACK_INFO(mset_.empty()));
  }

  Napi::Value get_description(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::String::New(info.Env(), mset_.get_description()));
  }

  void iter(const Napi::CallbackInfo& info) {
    auto env = info.Env();
    auto cb = info[0].As<Napi::Function>();
    for (auto it = mset_.begin(); it != mset_.end(); it++) {
      cb.Call(env.Global(), {MSetIterator::New(env, it)});
    }
  }

  Napi::Value get_iterator(const Napi::CallbackInfo& info) {
    auto env = info.Env();
    auto begin = mset_.begin();
    auto end = mset_.end();
    auto cb = Napi::Function::New(
        env, [begin, end](const Napi::CallbackInfo& info) mutable {
          auto env = info.Env();
          auto res = Napi::Object::New(env);
          if (begin != end) {
            res.Set("value",
                    MSetIterator::New(info.Env(),
                                      TRY_CATCH_XAPIAN_CALLBACK_INFO(begin)));
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

  static void Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "MSet",
        {
            InstanceMethod("get_matches_estimated",
                           &MSet::get_matches_estimated),
            InstanceAccessor("matches_estimaged", &MSet::get_matches_estimated,
                             nullptr),
            InstanceMethod("snippet", &MSet::snippet),
            InstanceMethod("get_size", &MSet::size),
            InstanceAccessor("size", &MSet::size, nullptr),
            InstanceMethod("empty", &MSet::empty),
            InstanceMethod("get_description", &MSet::get_description),
            InstanceMethod("toString", &MSet::get_description),
            InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"),
                           &MSet::get_iterator),

            // custom methods
            InstanceMethod("iter", &MSet::iter),

            // constants
            StaticValue(
                "SNIPPET_BACKGROUND_MODEL",
                Napi::Number::New(env, Xapian::MSet::SNIPPET_BACKGROUND_MODEL)),
            StaticValue(
                "SNIPPET_EXHAUSTIVE",
                Napi::Number::New(env, Xapian::MSet::SNIPPET_EXHAUSTIVE)),
            StaticValue("SNIPPET_EMPTY_WITHOUT_MATCH",
                        Napi::Number::New(
                            env, Xapian::MSet::SNIPPET_EMPTY_WITHOUT_MATCH)),
            StaticValue(
                "SNIPPET_CJK_NGRAM",
                Napi::Number::New(env, Xapian::MSet::SNIPPET_CJK_NGRAM)),
        });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("MSet", func);
  }

 private:
  inline static Napi::FunctionReference constructor;
  Xapian::MSet mset_;
};

