require 'mkmf'
require 'rbconfig'

dir_config('pqxx')

have_library('pqxx')

create_makefile("hmsearch/postgres_ext")
