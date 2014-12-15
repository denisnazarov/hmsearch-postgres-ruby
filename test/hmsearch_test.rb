require 'minitest/autorun'
require 'hmsearch/postgres'
require 'pg'

class TestHmsearchPostgres < MiniTest::Unit::TestCase
  def setup
    conn = PG::Connection.open('host=localhost port=5432 user=postgres')
    conn.exec('drop database hmsearch_test') rescue nil
    conn.exec('create database hmsearch_test')
  end

  def test_hmsearch
    HmSearch::Postgres.initdb('host=localhost port=5432 user=postgres dbname=hmsearch_test', 256, 10, 100000000)

    conn = HmSearch::Postgres.open('host=localhost port=5432 dbname=hmsearch_test');

    conn.insert('6e6fb315fa8c43fe9c2687d5be14575abb7252104236747d571b97e003563df0')
    conn.insert('6e6fb315fa8c43fe9c2687d5be14575a4d7252104236747d571b97e003563df0')

    expected = [{
      hash: '6e6fb315fa8c43fe9c2687d5be14575a4d7252104236747d571b97e003563df0',
      distance: 7
    },{
      hash: '6e6fb315fa8c43fe9c2687d5be14575abb7252104236747d571b97e003563df0',
      distance: 1
    }]

    actual = conn.lookup('6e6fb315fa8c43fe9c2687d5be14575abb7252104236747d571b97e003563df2') 
    
    conn.close

    conn = nil


    assert_equal(expected, actual)

    HmSearch::Postgres.open('host=localhost port=5432 dbname=hmsearch_test') do |conn|
      expected = [{
        hash: '6e6fb315fa8c43fe9c2687d5be14575abb7252104236747d571b97e003563df0',
        distance: 1
      }]
      actual = conn.lookup('6e6fb315fa8c43fe9c2687d5be14575abb7252104236747d571b97e003563df2', 1)
      assert_equal(expected, actual)
    end
  end
end
