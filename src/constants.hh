#pragma once

#include <napi.h>
#include <xapian.h>

class Constants {
 public:
  static void Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    exports.Set("DB_CREATE_OR_OPEN", Xapian::DB_CREATE_OR_OPEN);
    exports.Set("DB_CREATE_OR_OVERWRITE", Xapian::DB_CREATE_OR_OVERWRITE);
    exports.Set("DB_CREATE", Xapian::DB_CREATE);
    exports.Set("DB_OPEN", Xapian::DB_OPEN);
    exports.Set("DB_NO_SYNC", Xapian::DB_NO_SYNC);
    exports.Set("DB_FULL_SYNC", Xapian::DB_FULL_SYNC);
    exports.Set("DB_DANGEROUS", Xapian::DB_DANGEROUS);
    exports.Set("DB_NO_TERMLIST", Xapian::DB_NO_TERMLIST);
    exports.Set("DB_RETRY_LOCK", Xapian::DB_RETRY_LOCK);
    exports.Set("DB_BACKEND_GLASS", Xapian::DB_BACKEND_GLASS);
    exports.Set("DB_BACKEND_CHERT", Xapian::DB_BACKEND_CHERT);
    exports.Set("DB_BACKEND_STUB", Xapian::DB_BACKEND_STUB);
    exports.Set("DB_BACKEND_INMEMORY", Xapian::DB_BACKEND_INMEMORY);
    exports.Set("DBCHECK_SHORT_TREE", Xapian::DBCHECK_SHORT_TREE);
    exports.Set("DBCHECK_FULL_TREE", Xapian::DBCHECK_FULL_TREE);
    exports.Set("DBCHECK_SHOW_FREELIST", Xapian::DBCHECK_SHOW_FREELIST);
    exports.Set("DBCHECK_SHOW_STATS", Xapian::DBCHECK_SHOW_STATS);
    exports.Set("DBCHECK_FIX", Xapian::DBCHECK_FIX);
    exports.Set("DBCOMPACT_NO_RENUMBER", Xapian::DBCOMPACT_NO_RENUMBER);
    exports.Set("DBCOMPACT_MULTIPASS", Xapian::DBCOMPACT_MULTIPASS);
    exports.Set("DBCOMPACT_SINGLE_FILE", Xapian::DBCOMPACT_SINGLE_FILE);
    exports.Set("DOC_ASSUME_VALID", Xapian::DOC_ASSUME_VALID);
    exports.Set("BAD_VALUENO", Xapian::BAD_VALUENO);
  }
};

