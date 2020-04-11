#pragma once

#include <napi.h>
#include <xapian.h>

#include "database.h"
#include "exceptions.hh"
#include "query.hh"
#include "stem.hh"

class QueryParser : public Napi::ObjectWrap<QueryParser> {
 public:
  QueryParser(const Napi::CallbackInfo& info)
      : Napi::ObjectWrap<QueryParser>(info) {}

  void set_stemmer(const Napi::CallbackInfo& info) {
    Stem* stemmer = Napi::ObjectWrap<Stem>::Unwrap(info[0].As<Napi::Object>());
    TRY_CATCH_XAPIAN_CALLBACK_INFO(qp_.set_stemmer(*stemmer));
  }

  void set_stemming_strategy(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(qp_.set_stemming_strategy(
        static_cast<Xapian::QueryParser::stem_strategy>(
            info[0].ToNumber().Int32Value())));
  }

  void set_default_op(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(qp_.set_default_op(
        static_cast<Xapian::Query::op>(info[0].ToNumber().Int32Value())));
  }

  Napi::Value get_default_op(const Napi::CallbackInfo& info) {
    return Napi::Number::New(
        info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(qp_.get_default_op()));
  }

  void set_database(const Napi::CallbackInfo& info) {
    auto obj = info[0].As<Napi::Object>();
    Database* db = Napi::ObjectWrap<Database>::Unwrap(obj);
    TRY_CATCH_XAPIAN_CALLBACK_INFO(qp_.set_database(*db));
  }

  void set_max_expansion(const Napi::CallbackInfo& info) {
    if (info.Length() >= 3) {
      qp_.set_max_expansion(info[0].ToNumber(), info[1].ToNumber(),
                            info[2].ToNumber());
    } else if (info.Length() == 2) {
      qp_.set_max_expansion(info[0].ToNumber(), info[1].ToNumber());
    } else {
      qp_.set_max_expansion(info[0].ToNumber());
    }
  }

  Napi::Value parse_query(const Napi::CallbackInfo& info) {
    uint32_t flags = Xapian::QueryParser::FLAG_DEFAULT;
    std::string default_prefix;
    if (info.Length() > 1) {
      flags = info[1].ToNumber();
    }
    if (info.Length() > 2) {
      default_prefix = info[2].ToString();
    }

    auto query = qp_.parse_query(info[0].ToString(), flags, default_prefix);
    return Query::New(info.Env(), query);
  }

  void add_prefix(const Napi::CallbackInfo& info) {
    qp_.add_prefix(info[0].ToString(), info[1].ToString());
  }

  void add_boolean_prefix(const Napi::CallbackInfo& info) {
    qp_.add_boolean_prefix(info[0].ToString(), info[1].ToString());
  }

  Napi::Value get_corrected_query_string(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), qp_.get_corrected_query_string());
  }

  Napi::Value get_description(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), qp_.get_description());
  }

  static void Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "QueryParser",
        {
            InstanceMethod("set_stemmer", &QueryParser::set_stemmer),
            InstanceMethod("set_stemming_strategy",
                           &QueryParser::set_stemming_strategy),
            InstanceMethod("set_default_op", &QueryParser::set_default_op),
            InstanceMethod("get_default_op", &QueryParser::get_default_op),
            InstanceMethod("set_database", &QueryParser::set_database),
            InstanceMethod("set_max_expansion",
                           &QueryParser::set_max_expansion),
            InstanceMethod("parse_query", &QueryParser::parse_query),
            InstanceMethod("add_prefix", &QueryParser::add_prefix),
            InstanceMethod("add_boolean_prefix",
                           &QueryParser::add_boolean_prefix),
            InstanceMethod("get_corrected_query_string",
                           &QueryParser::get_corrected_query_string),
            InstanceMethod("get_description", &QueryParser::get_description),

            // constants
            StaticValue("STEM_NONE",
                        Napi::Number::New(env, Xapian::QueryParser::STEM_NONE)),
            StaticValue("STEM_SOME",
                        Napi::Number::New(env, Xapian::QueryParser::STEM_SOME)),
            StaticValue("STEM_ALL",
                        Napi::Number::New(env, Xapian::QueryParser::STEM_ALL)),
            StaticValue(
                "STEM_ALL_Z",
                Napi::Number::New(env, Xapian::QueryParser::STEM_ALL_Z)),
            StaticValue("STEM_SOME_FULL_POS",
                        Napi::Number::New(
                            env, Xapian::QueryParser::STEM_SOME_FULL_POS)),

        });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("QueryParser", func);
  }

 private:
  inline static Napi::FunctionReference constructor;
  Xapian::QueryParser qp_;
};

