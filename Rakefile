require 'rake/clean'
require 'rbconfig'

$root_dir = File.expand_path('..', __FILE__)
$build_dir = File.join($root_dir, 'build')
$vendor_dir = File.join($build_dir, 'vendor')
$lib_dir = File.join($root_dir, 'lib/hmsearch')
$extconf = File.join($root_dir, 'ext/hmsearch/extconf.rb')
$libpqxx_url = "http://pqxx.org/download/software/libpqxx/libpqxx-4.0.1.tar.gz"
$libpqxx_tar = "#{$build_dir}/libpqxx-4.0.1.tar.gz"
$libpqxx_dir = "#{$build_dir}/libpqxx-4.0.1"
$libpqxx_header = "#{$vendor_dir}/include/pqxx/pqxx"
$libpqxx_include = "#{$vendor_dir}/include"
$libpqxx_lib = "#{$vendor_dir}/lib"
$dlext = RbConfig::CONFIG['DLEXT']
$build_ext_dylib = "#{$build_dir}/postgres_ext.#{$dlext}"
$lib_ext_dylib = "#{$lib_dir}/postgres_ext.#{$dlext}"

CLEAN.include($build_dir)
CLOBBER.include($lib_ext_dylib)

task default: :test

directory $build_dir

directory $vendor_dir

file $libpqxx_tar => $build_dir do
  chdir $build_dir do
    sh "curl -O #{$libpqxx_url}"
  end
end

file $libpqxx_dir => $libpqxx_tar do
  chdir $build_dir do
    sh 'tar xf libpqxx-4.0.1.tar.gz'
  end
end

file $libpqxx_header => [$vendor_dir,  $libpqxx_dir] do
  chdir $libpqxx_dir do
    sh "./configure --enable-shared --prefix=#{$vendor_dir}"
    sh 'make'
    sh 'make install'
  end
end

file $build_ext_dylib => $libpqxx_header do
  chdir $build_dir do
    sh "ruby ../ext/hmsearch/extconf.rb -- --with-pqxx-include=#{$libpqxx_include}\ --with-pqxx-lib=#{$libpqxx_lib}"
    sh 'make'
  end
end

file $lib_ext_dylib => $build_ext_dylib do
  cp $build_ext_dylib, $lib_ext_dylib
end

task build: $lib_ext_dylib

desc 'run tests'
task test: $lib_ext_dylib do
  $:.unshift('lib')
  FileList.new('./test/*_test.rb').each do |test|
    load test
  end
end
