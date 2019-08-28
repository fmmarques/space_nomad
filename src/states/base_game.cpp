#include <iostream>

#include <bejeweled/states/base_game.hpp>
#include <yage/input/input_manager.hpp>
#include <yage/graphics/texture_manager.hpp>
#include <yage/graphics/graphics_manager.hpp>

namespace bejeweled {
namespace states {
namespace interface1 {

base_game::base_game():
  mouse{}
, background{ yage::graphics::texture_manager::instance().load("assets/bejeweled.background.1.jpg") }
, grid("assets/grid.background.png", (SDL_Rect){.x=0, .y=0, .w=32*8, .h=32*8})
{
  yage::input::input_manager::instance().subscribe(this); 
}

base_game::~base_game ()
{
  yage::input::input_manager::instance().unsubscribe(this); 
}

void base_game::on_initialize()
{
  std::cout << "base_game::on_initialize: enter" << std::endl;
  std::cout << "base_game::on_initialize: exit" << std::endl;
}

void base_game::on_pause()
{
  std::cout << "base_game::on_pause(): enter" << std::endl;	
  yage::input::input_manager::instance().unsubscribe(this);
  std::cout << "base_game::on_pause(): exit" << std::endl;	
}

void base_game::on_continue()
{
  std::cout << "base_game::on_continue(): enter" << std::endl;
  yage::input::input_manager::instance().subscribe(this);  
  std::cout << "base_game::on_continue(): exit" << std::endl;	
}

void base_game::on_deinitialize()
{
  std::cout << "base_game::on_deinitialize(): enter" << std::endl;	
  std::cout << "base_game::on_deinitialize(): exit" << std::endl;	
}

void base_game::on_interrupt()
{
  std::cout << "base_game::on_interrupt(): enter" << std::endl;	
  std::cout << "base_game::on_interrupt(): exit" << std::endl;	
}

void base_game::on_frame()
{
  std::cout << "base_game::on_frame(): enter" << std::endl;	
  auto&& r = yage::graphics::graphics_manager::instance().get_window();

  
  SDL_RenderCopy(r, background, NULL, r);
  grid.on_frame();
  std::cout << "base_game::on_frame(): exit" << std::endl;	
}

void base_game::on_keycode_pressed(const SDL_Keysym& keysym) 
{
  std::cout << "base_game::on_keycode_pressed: " << keysym.sym << std::endl;
}

void base_game::on_keycode_released(const SDL_Keysym& keysym) 
{
  std::cout << "base_game::on_keycode_released: " << keysym.sym << std::endl;

}

void base_game::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{

  std::cout << "base_game::on_mouse_button_down: button: " << std::to_string(button.button) << "; state: " << std::to_string(button.state) << "; clicks: " << std::to_string(button.clicks) << "; <x,y>=<" << button.x << "," <<button.y << std::endl;

}

void base_game::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  std::cout << "base_game::on_mouse_button_up: button: " << std::to_string(button.button) << "; state: " << std::to_string(button.state) << "; clicks: " << std::to_string(button.clicks) << "; <x,y>=<" << button.x << "," <<button.y << std::endl;
  
  if (mouse.hovers(grid)) 
  {
    if (mouse.is_dragging())
      grid.on_dragged(button);
    else 
      grid.on_clicked(button);
  }
  else
  {
    grid.lose_focus();
  }
}

void base_game::on_mouse_movement(const SDL_MouseMotionEvent& motion)
{
  std::cout << "base_game::on_mouse_movement: motion: <" << motion.x << ", " << motion.y << ">" << std::endl; 
}


    }
  
  }
}
