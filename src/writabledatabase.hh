#pragma once

#include <napi.h>
#include <xapian.h>

#include "database.h"
#include "document.hh"

class WritableDatabase : public Napi::ObjectWrap<WritableDatabase>,
                         public BaseDatabase<Xapian::WritableDatabase> {
 public:
  WritableDatabase(const Napi::CallbackInfo& info)
      : Napi::ObjectWrap<WritableDatabase>(info),
        BaseDatabase<Xapian::WritableDatabase>() {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0) {
      db_ = TRY_CATCH_XAPIAN_CALLBACK_INFO(Xapian::WritableDatabase());
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
          Xapian::WritableDatabase(info[0].ToString(), flags));
    }
  }

  void commit(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.commit());
  }

  void begin_transaction(const Napi::CallbackInfo& info) {
    if (info.Length() == 0) {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.begin_transaction());
    } else {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(
          db_.begin_transaction(info[0].ToBoolean()));
    }
  }

  void commit_transaction(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.commit_transaction());
  }

  void cancel_transaction(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.cancel_transaction());
  }

  Napi::Value add_document(const Napi::CallbackInfo& info) {
    Document* doc =
        Napi::ObjectWrap<Document>::Unwrap(info[0].As<Napi::Object>());
    Xapian::docid docid =
        TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.add_document(*doc));
    return Napi::Number::New(info.Env(), docid);
  }

  void delete_document(const Napi::CallbackInfo& info) {
    if (info[0].IsString()) {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.delete_document(info[0].ToString()));
    } else {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.delete_document(info[0].ToNumber()));
    }
  }

  Napi::Value replace_document(const Napi::CallbackInfo& info) {
    Document* doc =
        Napi::ObjectWrap<Document>::Unwrap(info[0].As<Napi::Object>());
    Xapian::docid docid;
    if (info[0].IsString()) {
      docid = TRY_CATCH_XAPIAN_CALLBACK_INFO(
          db_.replace_document(info[0].ToString(), *doc));
    } else {
      docid = info[0].ToNumber();
      TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.replace_document(docid, *doc));
    }
    return Napi::Number::New(info.Env(), docid);
  }

  void add_spelling(const Napi::CallbackInfo& info) {
    if (info.Length() > 1) {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(
          db_.add_spelling(info[0].ToString(), info[1].ToNumber()));
    } else {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.add_spelling(info[0].ToString()));
    }
  }

  void remove_spelling(const Napi::CallbackInfo& info) {
    if (info.Length() > 1) {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(
          db_.remove_spelling(info[0].ToString(), info[1].ToNumber()));
    } else {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.remove_spelling(info[0].ToString()));
    }
  }

  void add_synonym(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(
        db_.add_synonym(info[0].ToString(), info[1].ToString()));
  }

  void remove_synonym(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(
        db_.remove_synonym(info[0].ToString(), info[1].ToString()));
  }

  void clear_synonyms(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(db_.clear_synonyms(info[0].ToString()));
  }

  void set_metadata(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(
        db_.set_metadata(info[0].ToString(), info[1].ToString()));
  }

  static void Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "WritableDatabase",
        {
            // WritableDatabase properties
            InstanceMethod("commit", &WritableDatabase::commit),
            InstanceMethod("begin_transaction",
                           &WritableDatabase::begin_transaction),
            InstanceMethod("commit_transaction",
                           &WritableDatabase::commit_transaction),
            InstanceMethod("cancel_transaction",
                           &WritableDatabase::cancel_transaction),
            InstanceMethod("add_document", &WritableDatabase::add_document),
            InstanceMethod("delete_document",
                           &WritableDatabase::delete_document),
            InstanceMethod("replace_document",
                           &WritableDatabase::replace_document),
            InstanceMethod("add_spelling", &WritableDatabase::add_spelling),
            InstanceMethod("remove_spelling",
                           &WritableDatabase::remove_spelling),
            InstanceMethod("add_synonym", &WritableDatabase::add_synonym),
            InstanceMethod("remove_synonym", &WritableDatabase::remove_synonym),
            InstanceMethod("clear_synonyms", &WritableDatabase::clear_synonyms),
            InstanceMethod("set_metadata", &WritableDatabase::set_metadata),

            // Base methods
            InstanceMethod("get_size", &WritableDatabase::size),
            InstanceAccessor("size", &WritableDatabase::size, nullptr),
            InstanceMethod("close", &WritableDatabase::close),
            InstanceMethod("reopen", &WritableDatabase::reopen),
            InstanceMethod("get_description",
                           &WritableDatabase::get_description),
            InstanceMethod("toString", &WritableDatabase::get_description),
            InstanceMethod("has_positions", &WritableDatabase::has_positions),
            InstanceMethod("get_doccount", &WritableDatabase::get_doccount),
            InstanceAccessor("doccount", &WritableDatabase::get_doccount,
                             nullptr),
            InstanceMethod("get_lastdocid", &WritableDatabase::get_lastdocid),
            InstanceAccessor("lastdocid", &WritableDatabase::get_lastdocid,
                             nullptr),
            InstanceMethod("get_avlength", &WritableDatabase::get_avlength),
            InstanceMethod("get_total_length",
                           &WritableDatabase::get_total_length),
            InstanceMethod("get_doclength", &WritableDatabase::get_doclength),
            InstanceMethod("get_document", &WritableDatabase::get_document),

            InstanceMethod("get_metadata", &WritableDatabase::get_metadata),
            InstanceMethod("get_uuid", &WritableDatabase::get_uuid),
            InstanceAccessor("uuid", &WritableDatabase::get_uuid, nullptr),
            InstanceMethod("locked", &WritableDatabase::locked),
            InstanceMethod("get_revision", &WritableDatabase::get_revision),
            InstanceMethod("compact", &WritableDatabase::compact),

        });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("WritableDatabase", func);
  }

 private:
  inline static Napi::FunctionReference constructor;
  Xapian::WritableDatabase db_;
};

