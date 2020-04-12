
#include <napi.h>

#include "constants.hh"
#include "database.hh"
#include "document.hh"
#include "enquire.hh"
#include "mset.hh"
#include "msetiterator.hh"
#include "query.hh"
#include "queryparser.hh"
#include "stem.hh"
#include "termgenerator.hh"
#include "termiterator.hh"
#include "writabledatabase.hh"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  Constants::Init(env, exports);
  TermIterator::Init(env, exports);
  Document::Init(env, exports);
  Database::Init(env, exports);
  WritableDatabase::Init(env, exports);
  TermGenerator::Init(env, exports);
  Stem::Init(env, exports);
  Enquire::Init(env, exports);
  Query::Init(env, exports);
  QueryParser::Init(env, exports);
  MSet::Init(env, exports);
  MSetIterator::Init(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)
