#include <iostream>
#include <bejeweled/states/main_menu.hpp>

namespace bejeweled {
  namespace states {
    namespace interface1 {
main_menu::main_menu():
	options{ "New game", "Continue", "exit" },
	selected_option{0}
{
  
}

main_menu& main_menu::instance() 
{
  static main_menu instance{};
  return instance;
}

void main_menu::on_initialize()
{
  std::cout << "main_menu::on_initialize." << std::endl;
}

void main_menu::on_render()
{
	
}
    }
  
  }
}
