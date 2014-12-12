#include "ruby.h"
#include "hmsearch.h"

extern "C" void Init_postgres_ext();

static VALUE eHmSearchError;
static ID id_hash;
static ID id_distance;

static HmSearch *HmSearch_ptr(VALUE obj);
static void HmSearch_free(void *p);
static VALUE HmSearch_initdb(VALUE klass, VALUE _path, VALUE _hash_bits, VALUE _max_error, VALUE _num_hashes);
static VALUE HmSearch_open(VALUE klass, VALUE _path);
static VALUE HmSearch_close(VALUE self);
static VALUE HmSearch_insert(VALUE self, VALUE _hash);
static VALUE HmSearch_lookup(int argc, VALUE *argv, VALUE self);

static HmSearch *HmSearch_ptr(VALUE obj)
{
  HmSearch *p;
  Data_Get_Struct(obj, HmSearch, p);
  return p;
}

static void HmSearch_free(void *p)
{
  static_cast<HmSearch*>(p)->~HmSearch();
}

static VALUE HmSearch_initdb(VALUE klass, VALUE _path, VALUE _hash_bits, VALUE _max_error, VALUE _num_hashes)
{
  const char *path = StringValueCStr(_path);
  unsigned hash_bits = FIX2UINT(_hash_bits);
  unsigned max_error = FIX2UINT(_max_error);
  uint64_t num_hashes = NUM2ULONG(_num_hashes);

  std::string error_msg;
  if (!HmSearch::init(path, hash_bits, max_error, num_hashes, &error_msg)) {
    rb_raise(eHmSearchError, "%s", error_msg.c_str());
  }

  return Qnil;
}

static VALUE HmSearch_open(VALUE klass, VALUE _path)
{
  const char *path = StringValueCStr(_path);

  std::string error_msg;
  HmSearch* p = HmSearch::open(path, &error_msg);
  if (!p) {
    rb_raise(eHmSearchError, "%s", error_msg.c_str());
  }

  VALUE obj = Data_Wrap_Struct(klass, NULL, HmSearch_free, p);

  if (rb_block_given_p()) {
    return rb_ensure(RUBY_METHOD_FUNC(rb_yield), obj, RUBY_METHOD_FUNC(HmSearch_close), obj);
  }

  return obj;
}

static VALUE HmSearch_close(VALUE self)
{
  HmSearch_ptr(self)->close();
  return Qnil;
}

static VALUE HmSearch_insert(VALUE self, VALUE _hash)
{
  HmSearch_ptr(self)->insert(HmSearch::parse_hexhash(StringValueCStr(_hash)));
  return Qnil;
}

static VALUE HmSearch_lookup(int argc, VALUE *argv, VALUE self)
{
  VALUE _hash, _reduced_error;
  rb_scan_args(argc, argv, "11", &_hash, &_reduced_error);

  const char *hash = StringValueCStr(_hash);

  int reduced_error = NIL_P(_reduced_error) ? -1 : FIX2INT(_reduced_error);

  HmSearch::LookupResultList matches;
  std::string error_msg;
  if (!HmSearch_ptr(self)->lookup(HmSearch::parse_hexhash(hash), matches, reduced_error, &error_msg)) {
    rb_raise(eHmSearchError, "%s", error_msg.c_str());
  }

  VALUE results = rb_ary_new2(matches.size());

  for (HmSearch::LookupResultList::const_iterator it=matches.begin(); it != matches.end(); ++it) {
    VALUE result = rb_hash_new();

    rb_hash_aset(result, ID2SYM(id_hash), rb_str_new2(HmSearch::format_hexhash(it->hash).c_str()));
    rb_hash_aset(result, ID2SYM(id_distance), INT2NUM(it->distance));

    rb_ary_push(results, result);
  }

  return results;
}

void Init_postgres_ext() {
  VALUE mHmSearch = rb_define_module("HmSearch");
  
  eHmSearchError = rb_define_class_under(mHmSearch, "HmSearchError", rb_eStandardError);

  VALUE cPostgres = rb_define_class_under(mHmSearch, "Postgres", rb_cObject);

  VALUE cPostgresSingleton = rb_singleton_class(cPostgres);

  id_hash = rb_intern("hash");
  id_distance = rb_intern("distance");

  rb_define_method(cPostgresSingleton, "initdb", RUBY_METHOD_FUNC(HmSearch_initdb), 4);

  rb_define_method(cPostgresSingleton, "open",  RUBY_METHOD_FUNC(HmSearch_open), 1);
  // we have to allocate via open
  rb_undef_method(cPostgresSingleton, "new");
  rb_undef_method(cPostgresSingleton, "allocate");

  rb_define_method(cPostgres, "close", RUBY_METHOD_FUNC(HmSearch_close), 0);
  rb_define_method(cPostgres, "insert", RUBY_METHOD_FUNC(HmSearch_insert), 1);
  rb_define_method(cPostgres, "lookup", RUBY_METHOD_FUNC(HmSearch_lookup), -1);
}
