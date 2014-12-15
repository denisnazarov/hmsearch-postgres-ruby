require 'mkmf'
require 'rbconfig'

dir_config('pqxx')

have_library('pqxx')

$CPPFLAGS += ' --std=c++0x'
$LDFLAGS += ' --std=c++0x'

create_makefile("hmsearch/postgres_ext")
