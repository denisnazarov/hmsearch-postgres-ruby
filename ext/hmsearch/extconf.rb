require 'mkmf'
require 'rbconfig'

dir_config('pqxx')

have_library('pqxx')

try_cppflags('--std=c++11')
try_ldflags('--std=c++11')

create_makefile("hmsearch/postgres_ext")
