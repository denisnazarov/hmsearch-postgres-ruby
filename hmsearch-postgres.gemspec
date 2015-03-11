Gem::Specification.new do |s|
  s.name = 'hmsearch-postgres'
  s.version = '0.1.7'

  s.summary = 'hmsearch postgres client'
  s.description = <<-EOF
    A ruby wrapper for the hmsearch postgres client.
  EOF

  s.files = `git ls-files`.split("\n")
  s.require_path = 'lib'

  s.add_development_dependency 'rake'
  s.add_development_dependency 'pg'

  s.extensions = 'ext/hmsearch/extconf.rb'

  s.authors = ['Kris Selden']
  s.email = 'kris.selden@gmail.com'
  s.homepage = 'https://github.com/denisnazarov/hmsearch-postgres-ruby'
  s.license = 'MIT'
end
