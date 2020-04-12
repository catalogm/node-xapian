#pragma once

#include <napi.h>
#include <xapian.h>

#include "database.hh"
#include "exceptions.hh"
#include "mset.hh"
#include "query.hh"

class Enquire : public Napi::ObjectWrap<Enquire> {
 public:
  Enquire(const Napi::CallbackInfo& info)
      : Napi::ObjectWrap<Enquire>(info), enquire_{nullptr} {
    auto env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1) {
      throw Napi::Error::New(env, "database is required");
    }
    auto obj = info[0].As<Napi::Object>();
    auto db = Napi::ObjectWrap<Database>::Unwrap(obj);
    enquire_ =
        TRY_CATCH_XAPIAN_CALLBACK_INFO(std::make_shared<Xapian::Enquire>(*db));
  }

  void set_query(const Napi::CallbackInfo& info) {
    auto obj = info[0].As<Napi::Object>();
    Query* q = Napi::ObjectWrap<Query>::Unwrap(obj);
    TRY_CATCH_XAPIAN_CALLBACK_INFO(enquire_->set_query(*q));
  }

  void set_docid_order(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(
        enquire_->set_docid_order(static_cast<Xapian::Enquire::docid_order>(
            info[0].ToNumber().Int32Value())));
  }

  void set_sort_by_relevance(const Napi::CallbackInfo& info) {
    enquire_->set_sort_by_relevance();
  }

  Napi::Value get_mset(const Napi::CallbackInfo& info) {
    uint64_t checkatleast = 0;
    if (info.Length() > 2) {
      checkatleast = static_cast<uint64_t>(info[2].ToNumber().Int64Value());
    }
    return MSet::New(
        info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(enquire_->get_mset(
                        info[0].ToNumber(), info[1].ToNumber(), checkatleast)));
  }

  Napi::Value get_description(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::String::New(info.Env(), enquire_->get_description()));
  }

  static void Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "Enquire",
        {
            InstanceMethod("set_query", &Enquire::set_query),
            InstanceMethod("set_docid_order", &Enquire::set_docid_order),
            InstanceMethod("set_sort_by_relevance",
                           &Enquire::set_sort_by_relevance),
            InstanceMethod("get_mset", &Enquire::get_mset),
            InstanceMethod("get_description", &Enquire::get_description),
            InstanceMethod("toString", &Enquire::get_description),

            // constants
            StaticValue("ASCENDING",
                        Napi::Number::New(env, Xapian::Enquire::ASCENDING)),
            StaticValue("DESCENDING",
                        Napi::Number::New(env, Xapian::Enquire::DESCENDING)),
            StaticValue("DONT_CARE",
                        Napi::Number::New(env, Xapian::Enquire::DONT_CARE)),
        });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("Enquire", func);
  }

 private:
  inline static Napi::FunctionReference constructor;
  std::shared_ptr<Xapian::Enquire> enquire_;
};

