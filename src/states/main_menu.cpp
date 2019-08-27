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
  yage::input::input_manager::instance().subscribe(this); 
}

main_menu::~main_menu ()
{
  yage::input::input_manager::instance().unsubscribe(this); 
}

void main_menu::on_initialize()
{
  std::cout << "main_menu::on_initialize: enter" << std::endl;
  std::cout << "main_menu::on_initialize: exit" << std::endl;
}

void main_menu::on_pause()
{
  std::cout << "main_menu::on_pause(): enter" << std::endl;	
  std::cout << "main_menu::on_pause(): exit" << std::endl;	
}

void main_menu::on_continue()
{
  std::cout << "main_menu::on_continue(): enter" << std::endl;	
  std::cout << "main_menu::on_continue(): exit" << std::endl;	
}

void main_menu::on_deinitialize()
{
  std::cout << "main_menu::on_deinitialize(): enter" << std::endl;	
  std::cout << "main_menu::on_deinitialize(): exit" << std::endl;	
}

void main_menu::on_interrupt()
{
  std::cout << "main_menu::on_interrupt(): enter" << std::endl;	
  std::cout << "main_menu::on_interrupt(): exit" << std::endl;	
}

void main_menu::on_frame()
{
//  std::cout << "main_menu::on_frame(): enter" << std::endl;	
//  std::cout << "main_menu::on_frame(): exit" << std::endl;	
  auto&& window_and_renderer = yage::graphics::graphics_manager::instance().get_window();

  SDL_RenderCopy(window_and_renderer, background, NULL, window_and_renderer);
}

void main_menu::on_keycode_pressed(const SDL_Keysym& keysym) 
{
  std::cout << "main_menu::on_keycode_pressed: " << keysym.sym << std::endl;
}

void main_menu::on_keycode_released(const SDL_Keysym& keysym) 
{
  std::cout << "main_menu::on_keycode_released: " << keysym.sym << std::endl;

}

void main_menu::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  std::cout << "main_menu::on_mouse_button_down: button: " << std::to_string(button.button) << "; state: " << std::to_string(button.state) << "; clicks: " << std::to_string(button.clicks) << "; <x,y>=<" << button.x << "," <<button.y << std::endl;
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
