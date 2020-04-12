#pragma once

#include <napi.h>
#include <xapian.h>

#include "database.hh"
#include "exceptions.hh"

class Query : public Napi::ObjectWrap<Query> {
 public:
  Query(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Query>(info) {
    if (info.Length() == 1 && info[0].IsExternal()) {
      auto qPtr = reinterpret_cast<Xapian::Query*>(
          info[0].As<Napi::External<Xapian::Query>>().Data());
      query_ = *qPtr;
    }
  }

  static Napi::Object New(Napi::Env env, Xapian::Query query) {
    auto eQuery = Napi::External<decltype(query)>::New(env, &query);
    return constructor.New({eQuery});
  }

  Napi::Value empty(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), query_.empty());
  }

  Napi::Value serialise(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), query_.serialise());
  }

  Napi::Value get_description(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), query_.get_description());
  }

  operator const Xapian::Query&() { return query_; }

  static void Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "Query",
        {
            InstanceMethod("empty", &Query::empty),
            InstanceMethod("serialise", &Query::serialise),
            InstanceMethod("get_description", &Query::get_description),

            // constants
            StaticValue("OP_AND",
                        Napi::Number::New(env, Xapian::Query::OP_AND)),
            StaticValue("OP_OR", Napi::Number::New(env, Xapian::Query::OP_OR)),
            StaticValue("OP_AND_NOT",
                        Napi::Number::New(env, Xapian::Query::OP_AND_NOT)),
            StaticValue("OP_XOR",
                        Napi::Number::New(env, Xapian::Query::OP_XOR)),
            StaticValue("OP_AND_MAYBE",
                        Napi::Number::New(env, Xapian::Query::OP_AND_MAYBE)),
            StaticValue("OP_FILTER",
                        Napi::Number::New(env, Xapian::Query::OP_FILTER)),
            StaticValue("OP_NEAR",
                        Napi::Number::New(env, Xapian::Query::OP_NEAR)),
            StaticValue("OP_PHRASE",
                        Napi::Number::New(env, Xapian::Query::OP_PHRASE)),
            StaticValue("OP_VALUE_RANGE",
                        Napi::Number::New(env, Xapian::Query::OP_VALUE_RANGE)),
            StaticValue("OP_SCALE_WEIGHT",
                        Napi::Number::New(env, Xapian::Query::OP_SCALE_WEIGHT)),
            StaticValue("OP_ELITE_SET",
                        Napi::Number::New(env, Xapian::Query::OP_ELITE_SET)),
            StaticValue("OP_VALUE_GE",
                        Napi::Number::New(env, Xapian::Query::OP_VALUE_GE)),
            StaticValue("OP_VALUE_LE",
                        Napi::Number::New(env, Xapian::Query::OP_VALUE_LE)),
            StaticValue("OP_SYNONYM",
                        Napi::Number::New(env, Xapian::Query::OP_SYNONYM)),
            StaticValue("OP_MAX",
                        Napi::Number::New(env, Xapian::Query::OP_MAX)),
            StaticValue("OP_WILDCARD",
                        Napi::Number::New(env, Xapian::Query::OP_WILDCARD)),
            StaticValue("OP_INVALID",
                        Napi::Number::New(env, Xapian::Query::OP_INVALID)),
            StaticValue("LEAF_TERM",
                        Napi::Number::New(env, Xapian::Query::LEAF_TERM)),
            StaticValue(
                "LEAF_POSTING_SOURCE",
                Napi::Number::New(env, Xapian::Query::LEAF_POSTING_SOURCE)),
            StaticValue("LEAF_MATCH_ALL",
                        Napi::Number::New(env, Xapian::Query::LEAF_MATCH_ALL)),
            StaticValue(
                "LEAF_MATCH_NOTHING",
                Napi::Number::New(env, Xapian::Query::LEAF_MATCH_NOTHING)),

        });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("Query", func);
  }

 private:
  inline static Napi::FunctionReference constructor;
  Xapian::Query query_;
};

