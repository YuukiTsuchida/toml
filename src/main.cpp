#include "../include/toml.hpp"

#include <iostream>

int main(int argc, char const* argv[])
{
  toml::bool_value value(false);

  std::cout << value.is<bool>() << std::endl;
  std::cout << value.is<int>() << std::endl;

  toml::array<toml::table> table_array;
  toml::table table; 

  table_array.add(table);
  return 0;
}
