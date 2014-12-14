require 'rake/clean'

CLEAN.include('lib/hmsearch/Makefile')

CLOBBER.include('lib/hmsearch/postgres_ext.*')

task default: :test

desc 'build native extension'
task :build do
  mkdir_p 'tmp'
  chdir 'tmp' do
    sh 'ruby ../ext/hmsearch/extconf.rb'
    sh 'make'
  end

  require 'rbconfig'
  dlext = RbConfig::CONFIG['DLEXT']
  cp "tmp/postgres_ext.#{dlext}", "lib/hmsearch/"
end

desc 'run tests'
task test: :build do
  $:.unshift('lib')
  FileList.new('./test/*_test.rb').each do |test|
    load test
  end
end
