#pragma once
#include <napi.h>
#include <xapian.h>

#include "exceptions.hh"
#include "stem.hh"
#include "termiterator.hh"
#include "writabledatabase.hh"

class TermGenerator : public Napi::ObjectWrap<TermGenerator> {
 public:
  TermGenerator(const Napi::CallbackInfo& info)
      : Napi::ObjectWrap<TermGenerator>(info), tg_() {}

  void set_stemmer(const Napi::CallbackInfo& info) {
    Stem* stemmer = Napi::ObjectWrap<Stem>::Unwrap(info[0].As<Napi::Object>());
    TRY_CATCH_XAPIAN_CALLBACK_INFO(tg_.set_stemmer(*stemmer));
  }

  void set_document(const Napi::CallbackInfo& info) {
    auto obj = info[0].As<Napi::Object>();
    doc_ = Napi::Reference<Napi::Object>::New(obj);
    Document* doc = Napi::ObjectWrap<Document>::Unwrap(obj);
    TRY_CATCH_XAPIAN_CALLBACK_INFO(tg_.set_document(*doc));
  }

  Napi::Value get_document(const Napi::CallbackInfo& info) {
    return doc_.Value();
  }

  void set_database(const Napi::CallbackInfo& info) {
    auto obj = info[0].As<Napi::Object>();
    db_ = Napi::Reference<Napi::Object>::New(obj);
    WritableDatabase* db = Napi::ObjectWrap<WritableDatabase>::Unwrap(obj);
    TRY_CATCH_XAPIAN_CALLBACK_INFO(tg_.set_database(*db));
  }

  Napi::Value get_database(const Napi::CallbackInfo& info) {
    return db_.Value();
  }

  void set_flags(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(
        tg_.set_flags(static_cast<int>(info[0].ToNumber())));
  }

  void set_stemming_strategy(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(tg_.set_stemming_strategy(
        static_cast<Xapian::TermGenerator::stem_strategy>(
            info[0].ToNumber().Int32Value())));
  }

  void set_stopper_strategy(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(tg_.set_stopper_strategy(
        static_cast<Xapian::TermGenerator::stop_strategy>(
            info[0].ToNumber().Int32Value())));
  }

  void set_max_word_length(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(tg_.set_max_word_length(info[0].ToNumber()));
  }

  void index_text(const Napi::CallbackInfo& info) {
    uint32_t wdf_inc = 1;
    std::string prefix;
    if (info.Length() > 1) {
      wdf_inc = info[1].ToNumber();
    }
    if (info.Length() > 2) {
      prefix = info[2].ToString();
    }
    TRY_CATCH_XAPIAN_CALLBACK_INFO(
        tg_.index_text(info[0].ToString(), wdf_inc, prefix));
  }

  void index_text_without_positions(const Napi::CallbackInfo& info) {
    uint32_t wdf_inc = 1;
    std::string prefix;
    if (info.Length() > 1) {
      wdf_inc = info[1].ToNumber();
    }
    if (info.Length() > 2) {
      prefix = info[2].ToString();
    }
    TRY_CATCH_XAPIAN_CALLBACK_INFO(
        tg_.index_text_without_positions(info[0].ToString(), wdf_inc, prefix));
  }

  void increase_termpos(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(tg_.increase_termpos(info[0].ToNumber()));
  }

  Napi::Value get_termpos(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(),
                             TRY_CATCH_XAPIAN_CALLBACK_INFO(tg_.get_termpos()));
  }

  void set_termpos(const Napi::CallbackInfo& info) {
    TRY_CATCH_XAPIAN_CALLBACK_INFO(tg_.set_termpos(info[0].ToNumber()));
  }

  Napi::Value get_description(const Napi::CallbackInfo& info) {
    return Napi::String::New(
        info.Env(), TRY_CATCH_XAPIAN_CALLBACK_INFO(tg_.get_description()));
  }

  static void Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "TermGenerator",
        {
            InstanceMethod("set_stemmer", &TermGenerator::set_stemmer),
            InstanceMethod("set_document", &TermGenerator::set_document),
            InstanceMethod("get_document", &TermGenerator::get_document),
            InstanceMethod("set_database", &TermGenerator::set_database),
            InstanceMethod("get_database", &TermGenerator::get_database),
            InstanceMethod("set_flags", &TermGenerator::set_flags),
            InstanceMethod("set_stemming_strategy",
                           &TermGenerator::set_stemming_strategy),
            InstanceMethod("set_stopper_strategy",
                           &TermGenerator::set_stopper_strategy),
            InstanceMethod("set_max_word_length",
                           &TermGenerator::set_max_word_length),
            InstanceMethod("index_text", &TermGenerator::index_text),
            InstanceMethod("index_text_without_positions",
                           &TermGenerator::index_text_without_positions),
            InstanceMethod("increase_termpos",
                           &TermGenerator::increase_termpos),
            InstanceMethod("get_termpos", &TermGenerator::get_termpos),
            InstanceMethod("set_termpos", &TermGenerator::set_termpos),
            InstanceMethod("get_description", &TermGenerator::get_description),

            // Constants
            StaticValue(
                "FLAG_SPELLING",
                Napi::Number::New(env, Xapian::TermGenerator::FLAG_SPELLING)),
            StaticValue(
                "FLAG_CJK_NGRAM",
                Napi::Number::New(env, Xapian::TermGenerator::FLAG_CJK_NGRAM)),
            StaticValue(
                "STEM_NONE",
                Napi::Number::New(env, Xapian::TermGenerator::STEM_NONE)),
            StaticValue(
                "STEM_SOME",
                Napi::Number::New(env, Xapian::TermGenerator::STEM_SOME)),
            StaticValue("STEM_ALL", Napi::Number::New(
                                        env, Xapian::TermGenerator::STEM_ALL)),
            StaticValue(
                "STEM_ALL_Z",
                Napi::Number::New(env, Xapian::TermGenerator::STEM_ALL_Z)),
            StaticValue("STEM_SOME_FULL_POS",
                        Napi::Number::New(
                            env, Xapian::TermGenerator::STEM_SOME_FULL_POS)),
            StaticValue(
                "STOP_NONE",
                Napi::Number::New(env, Xapian::TermGenerator::STOP_NONE)),
            StaticValue("STOP_ALL", Napi::Number::New(
                                        env, Xapian::TermGenerator::STOP_ALL)),
            StaticValue(
                "STOP_STEMMED",
                Napi::Number::New(env, Xapian::TermGenerator::STOP_STEMMED)),
        });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("TermGenerator", func);
  }

 private:
  inline static Napi::FunctionReference constructor;
  Xapian::TermGenerator tg_;
  Napi::ObjectReference db_;
  Napi::ObjectReference doc_;
};

