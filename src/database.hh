#pragma once

#include <napi.h>
#include <xapian.h>

#include "document.hh"
#include "exceptions.hh"

template <class T>
class BaseDatabase {
 public:
  void close(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.close());
  }

  Napi::Value reopen(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::Boolean::New(info.Env(), db_.reopen()));
  }

  Napi::Value size(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::Number::New(info.Env(), db_.size()));
  }

  Napi::Value get_description(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::String::New(info.Env(), db_.get_description()));
  }

  Napi::Value has_positions(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::Boolean::New(info.Env(), db_.has_positions()));
  }

  Napi::Value get_doccount(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::Number::New(info.Env(), db_.get_doccount()));
  }

  Napi::Value get_lastdocid(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::Number::New(info.Env(), db_.get_lastdocid()));
  }

  Napi::Value get_avlength(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::Number::New(info.Env(), db_.get_avlength()));
  }

  Napi::Value get_total_length(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::Number::New(info.Env(), db_.get_total_length()));
  }

  Napi::Value get_doclength(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::Number::New(info.Env(), db_.get_doclength(info[0].ToNumber())));
  }

  Napi::Value get_document(const Napi::CallbackInfo& info) {
    auto doc =
        TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.get_document(info[0].ToNumber()));
    return Document::New(info.Env(), doc);
  }

  Napi::Value get_metadata(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::String::New(info.Env(), db_.get_metadata(info[0].ToString())));
  }

  Napi::Value get_uuid(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::String::New(info.Env(), db_.get_uuid()));
  }

  Napi::Value locked(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::Boolean::New(info.Env(), db_.locked()));
  }

  Napi::Value get_revision(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::Number::New(info.Env(), db_.get_revision()));
  }

  void compact(const Napi::CallbackInfo& info) {
    uint32_t flags = 0;
    int block_size = 0;
    if (info.Length() > 1) {
      flags = info[1].ToNumber();
    }
    if (info.Length() > 2) {
      block_size = info[2].ToNumber();
    }
    TRY_CATCH_XAPIAN_CALLBACK_INFO(
        db_.compact(info[0].ToString(), flags, block_size));
  }

  operator const T&() { return db_; }

 protected:
  T db_;
};

class Database : public Napi::ObjectWrap<Database>,
                 public BaseDatabase<Xapian::Database> {
 public:
  Database(const Napi::CallbackInfo& info)
      : Napi::ObjectWrap<Database>(info), BaseDatabase<Xapian::Database>() {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0) {
      db_ = TRY_CATCH_XAPIAN_CALLBACK_INFO(Xapian::Database());
    } else {
      if (!info[0].IsString()) {
        throw Napi::Error::New(env, "first argument must be database path");
      }

      int flags = 0;
      if (info.Length() > 1) {
        if (!info[1].IsNumber()) {
          throw Napi::Error::New(
              env, "second argument must be an integer for flags");
        }
        flags = info[1].ToNumber();
      }

      db_ = TRY_CATCH_XAPIAN_CALLBACK_INFO(
          Xapian::Database(info[0].ToString(), flags));
    }
  }

  static void Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "Database",
        {
            InstanceMethod("get_size", &Database::size),
            InstanceAccessor("size", &Database::size, nullptr),
            InstanceMethod("close", &Database::close),
            InstanceMethod("reopen", &Database::reopen),
            InstanceMethod("get_description", &Database::get_description),
            InstanceMethod("toString", &Database::get_description),
            InstanceMethod("has_positions", &Database::has_positions),
            InstanceMethod("get_doccount", &Database::get_doccount),
            InstanceAccessor("doccount", &Database::get_doccount, nullptr),
            InstanceMethod("get_lastdocid", &Database::get_lastdocid),
            InstanceAccessor("lastdocid", &Database::get_lastdocid, nullptr),
            InstanceMethod("get_avlength", &Database::get_avlength),
            InstanceMethod("get_total_length", &Database::get_total_length),
            InstanceMethod("get_doclength", &Database::get_doclength),
            InstanceMethod("get_document", &Database::get_document),

            InstanceMethod("get_metadata", &Database::get_metadata),
            InstanceMethod("get_uuid", &Database::get_uuid),
            InstanceAccessor("uuid", &Database::get_uuid, nullptr),
            InstanceMethod("locked", &Database::locked),
            InstanceMethod("get_revision", &Database::get_revision),
            InstanceMethod("compact", &Database::compact),
        });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("Database", func);
  }

 private:
  inline static Napi::FunctionReference constructor;
};

