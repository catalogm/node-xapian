#pragma once

#include <napi.h>
#include <xapian.h>

#include "exceptions.hh"
#include "termiterator.hh"

class Document : public Napi::ObjectWrap<Document> {
 public:
  Document(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Document>(info) {
    if (info.Length() > 0) {
      auto docPtr = reinterpret_cast<Xapian::Document*>(
          info[0].As<Napi::External<Xapian::Document>>().Data());
      doc_ = *docPtr;
    } else {
      doc_ = Xapian::Document();
    }
  }

  static Napi::Object New(Napi::Env env, Xapian::Document doc) {
    auto external = Napi::External<decltype(doc)>::New(env, &doc);
    return constructor.New({external});
  }

  Napi::Value get_value(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(Napi::String::New(
        info.Env(), doc_.get_value(info[0].ToNumber().Int64Value())));
  }

  void add_value(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(
        doc_.add_value(info[0].ToNumber().Int64Value(), info[1].ToString()));
  }

  void remove_value(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(
        doc_.remove_value(info[0].ToNumber().Int64Value()));
  }

  void clear_values(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(doc_.clear_values());
  }

  Napi::Value get_data(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::String::New(info.Env(), doc_.get_data()));
  }

  void set_data(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(doc_.set_data(info[0].ToString()));
  }

  void set_data_setter(const Napi::CallbackInfo& info,
                       const Napi::Value& value) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(doc_.set_data(value.ToString()));
  }

  void add_posting(const Napi::CallbackInfo& info) {
    if (info.Length() < 3) {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(
          doc_.add_posting(info[0].ToString(), info[1].ToNumber()));
    } else {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(doc_.add_posting(
          info[0].ToString(), info[1].ToNumber(), info[2].ToNumber()));
    }
  }

  void add_term(const Napi::CallbackInfo& info) {
    if (info.Length() < 2) {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(doc_.add_term(info[0].ToString()));
    } else {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(
          doc_.add_term(info[0].ToString(), info[1].ToNumber()));
    }
  }

  void add_boolean_term(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(doc_.add_boolean_term(info[0].ToString()));
  }

  void remove_posting(const Napi::CallbackInfo& info) {
    if (info.Length() < 3) {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(
          doc_.remove_posting(info[0].ToString(), info[1].ToNumber()));
    } else {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(doc_.remove_posting(
          info[0].ToString(), info[1].ToNumber(), info[2].ToNumber()));
    }
  }

  void remove_postings(const Napi::CallbackInfo& info) {
    if (info.Length() < 4) {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(doc_.remove_postings(
          info[0].ToString(), info[1].ToNumber(), info[2].ToNumber()));
    } else {
      TRY_CATCH_XAPIAN_CALLBACK_INFO(
          doc_.remove_postings(info[0].ToString(), info[1].ToNumber(),
                               info[2].ToNumber(), info[3].ToNumber()));
    }
  }

  void remove_term(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(doc_.remove_term(info[0].ToString()));
  }

  void clear_terms(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(doc_.clear_terms());
  }

  Napi::Value termlist_count(const Napi::CallbackInfo& info) {
    return Napi::Number::New(
        info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(doc_.termlist_count()));
  }

  Napi::Value termlist(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(TermIterator::New(
        info.Env(), doc_.termlist_begin(), doc_.termlist_end()));
  }

  Napi::Value values_count(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::Number::New(info.Env(), doc_.values_count()));
  }

  Napi::Value get_docid(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::Number::New(info.Env(), doc_.get_docid()));
  }

  Napi::Value serialise(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::String::New(info.Env(), doc_.serialise()));
  }

  Napi::Value get_description(const Napi::CallbackInfo& info) {
    return TRY_CATCH_XAPIAN_CALLBACK_INFO(
        Napi::String::New(info.Env(), doc_.get_description()));
  }

  static void Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "Document",
        {
            InstanceMethod("get_value", &Document::get_value),
            InstanceMethod("add_value", &Document::add_value),
            InstanceMethod("remove_value", &Document::remove_value),
            InstanceMethod("clear_values", &Document::clear_values),
            InstanceMethod("get_data", &Document::get_data),
            InstanceAccessor("data", &Document::get_data,
                             &Document::set_data_setter),
            InstanceMethod("set_data", &Document::set_data),
            InstanceMethod("add_posting", &Document::add_posting),
            InstanceMethod("add_term", &Document::add_term),
            InstanceMethod("add_boolean_term", &Document::add_boolean_term),
            InstanceMethod("remove_posting", &Document::remove_posting),
            InstanceMethod("remove_postings", &Document::remove_postings),
            InstanceMethod("remove_term", &Document::remove_term),
            InstanceMethod("clear_terms", &Document::clear_terms),
            InstanceMethod("termlist_count", &Document::termlist_count),
            InstanceMethod("termlist", &Document::termlist),
            InstanceMethod("values_count", &Document::values_count),
            InstanceMethod("get_docid", &Document::get_docid),
            InstanceAccessor("docid", &Document::get_docid, nullptr),
            InstanceMethod("serialise", &Document::serialise),
            InstanceMethod("get_description", &Document::get_description),
            InstanceMethod("toString", &Document::get_description),
        });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("Document", func);
  }

  operator const Xapian::Document&() { return doc_; }

 private:
  inline static Napi::FunctionReference constructor;
  Xapian::Document doc_;
};
