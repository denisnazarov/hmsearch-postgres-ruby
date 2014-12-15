require 'mkmf'
require 'rbconfig'

dir_config('pqxx')

have_library('pqxx')

$CPPFLAGS += ' --std=c++11'
$LDFLAGS += ' --std=c++11'

create_makefile("hmsearch/postgres_ext")
