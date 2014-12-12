require 'mkmf'
require 'rbconfig'

$LDFLAGS += ' -lm -lpqxx'

create_makefile("hmsearch/postgres_ext")
