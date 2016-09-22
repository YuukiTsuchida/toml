#include "../include/toml.hpp"

#include <iostream>

void value_test()
{
    {
        toml::int_value int_value(5);
        int_value = 100;
        toml::int_value int_value2(int_value);

        std::cout << int_value << std::endl;
        std::cout << int_value2 << std::endl;
    }

    {
        toml::float_value float_value(5.0f);
        toml::float_value float_value2(std::move(float_value));
        std::cout << float_value2 << std::endl;
    }

    {
        toml::string_value string_value("aaaaa");
        toml::string_value string_value2("bbbb");
        std::cout << string_value << ":" << string_value2 << std::endl;
    }

    {
        toml::bool_value bool_value(true);
        std::cout << bool_value << ":";
        bool_value = false;
        std::cout << bool_value << std::endl;
    }

    {
        toml::date_time_value date_time(toml::date_time::now_from_local());
        std::cout << date_time << std::endl;
    }

}

void array_test()
{
  toml::array array_data;
  array_data.add(toml::int_value(100));
  array_data.add(toml::int_value(200));

  array_data.add(1000);
  array_data.add(1.1f);
  array_data.add("aaaaaaaa");

  std::cout << array_data << std::endl;
}

void table_test()
{
  toml::table table_data;
  table_data.add("aa", toml::int_value(1000));
  table_data.add("aaa", toml::string_value("aaaa"));
  table_data.add("aaaa", toml::date_time_value(toml::date_time::now_from_local()));

  std::cout << table_data << std::endl;
}

void table_in_table_test()
{
  toml::table table_data;
  table_data.add("aa", toml::int_value(1000));
  table_data.add("aaa", toml::string_value("aaaa"));
  table_data.add("aaaa", toml::date_time_value(toml::date_time::now_from_local()));

  toml::table table_data2;
  table_data2.add("table", table_data);
  table_data2.add("value", toml::int_value(1));
  table_data2.add("str", toml::string_value("abcdefg"));

  toml::table root;
  root.add("table_in_table2", table_data2);

  std::cout << root << std::endl;
}

void table_in_array_test()
{
  toml::array array_data;
  toml::table table_data;
  table_data.add("aa", toml::int_value(1000));
  table_data.add("aaa", toml::string_value("aaaa"));
  table_data.add("aaaa", toml::date_time_value(toml::date_time::now_from_local()));
}

int main()
{
  std::cout << "<value_test>" << std::endl;
  value_test();
  std::cout << "<array_test>" << std::endl;
  array_test();
  std::cout << "<table_test>" << std::endl;
  table_test();
  std::cout << "<table_in_table_test>" << std::endl;
  table_in_table_test();

  std::string toml = "\
number = +1_00  #aaaaa\n\
number2 = -1_00#aaaaa\n\
number3 = +0.11111#aaaaa\n\
#table = \"TOOO\"\
";

  auto parse_table = toml::parse::parse_str(toml);
  std::cout << parse_table << std::endl;
  return 0;
}


