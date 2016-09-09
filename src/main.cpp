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
  value_test();
  array_test();
  table_test();
  table_in_table_test();
  return 0;
}


