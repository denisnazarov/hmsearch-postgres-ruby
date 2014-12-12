require 'hmsearch/postgres'
require 'minitest/autorun'
require 'pg'

class TestHmsearchPostgres < MiniTest::Unit::TestCase
  def setup
    conn = PG::Connection.open('host=localhost port=5432')
    conn.exec('drop database hmsearch_test') rescue nil
    conn.exec('create database hmsearch_test')
  end

  def test_hmsearch
    HmSearch::Postgres.initdb('host=localhost port=5432 dbname=hmsearch_test', 256, 10, 100000000)

    conn = HmSearch::Postgres.open('host=localhost port=5432 dbname=hmsearch_test');

    conn.insert('6e6fb315fa8c43fe9c2687d5be14575abb7252104236747d571b97e003563df0')

    # we can't do more than one op per open because of a bug in underlying library
    # warning: adding 'int' to a string does not append to the string [-Wstring-plus-int]
    # pqxx::prepare::invocation prep = n.prepared("select_multiple_"+i);
    #
    # instead of appending to the string this just moves the char pointer
    # happens to work out ok one run through but not subsequent
    conn.close

    HmSearch::Postgres.open('host=localhost port=5432 dbname=hmsearch_test') do |conn|

      actual = conn.lookup('6e6fb315fa8c43fe9c2687d5be14575abb7252104236747d571b97e003563df2', -1) 
      
      expected = [{
        hash: '6e6fb315fa8c43fe9c2687d5be14575abb7252104236747d571b97e003563df0',
        distance: 1
      }]

      assert_equal(expected, actual)

    end
  end
end
