#include <iostream>

#include <bejeweled/states/main_menu.hpp>
#include <yage/input/input_manager.hpp>
#include <yage/graphics/texture_manager.hpp>
#include <yage/graphics/graphics_manager.hpp>

namespace bejeweled {
namespace states {
namespace interface1 {

main_menu::main_menu():
  yage::engine::game_state()
  ,options{ "New game", "Continue", "Exit" }
  ,selected_option{option::UNDEFINED}
  ,background{ yage::graphics::texture_manager::instance().load("assets/gems.spritesheet.png") } //main_menu.background.1.jpg") }
{
}

main_menu::~main_menu ()
{
}

void main_menu::on_initialize()
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "registered to events." << std::endl;
  yage::input::input_manager::instance().subscribe(this); 
}

void main_menu::on_pause()
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  yage::input::input_manager::instance().unsubscribe(this); 
  std::cout << fn << "deregistered from events." << std::endl;	
}

void main_menu::on_continue()
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  yage::input::input_manager::instance().subscribe(this); 
  std::cout << fn << "registered to events" << std::endl;	
}

void main_menu::on_deinitialize()
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  yage::input::input_manager::instance().unsubscribe(this); 
  std::cout << fn << "deregisted from events." << std::endl;	
}

void main_menu::on_interrupt()
{
}

void main_menu::on_frame()
{
  auto&& r = yage::graphics::graphics_manager::instance().get_window();

  SDL_RenderCopy(r, background, NULL, r);
}

void main_menu::on_keycode_pressed(const SDL_Keysym& keysym) 
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "key code: " << keysym.sym << std::endl;
}

void main_menu::on_keycode_released(const SDL_Keysym& keysym) 
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "key code: " << keysym.sym << std::endl;

}

void main_menu::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "button: " << std::to_string(button.button) << "; state: " << std::to_string(button.state) << "; clicks: " << std::to_string(button.clicks) << "; <x,y>=<" << button.x << "," <<button.y << ">" << std::endl;
}

void main_menu::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  std::cout << "main_menu::on_mouse_button_up: button: " << std::to_string(button.button) << "; state: " << std::to_string(button.state) << "; clicks: " << std::to_string(button.clicks) << "; <x,y>=<" << button.x << "," <<button.y << std::endl;
}

void main_menu::on_mouse_movement(const SDL_MouseMotionEvent& motion)
{
  std::cout << "main_menu::on_mouse_movement: motion: <" << motion.x << ", " << motion.y << ">" << std::endl; 
}


    }
  
  }
}
