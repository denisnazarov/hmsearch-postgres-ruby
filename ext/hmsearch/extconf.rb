require 'mkmf'
require 'rbconfig'

dir_config('pqxx')

have_library('pqxx')

with_cppflags('--std=c++11') do
	try_cpp(<<-CPP)
		#include <string>
		int main() {
			std::to_string(0);
			return 0;
		}
	CPP
end

create_makefile("hmsearch/postgres_ext")
