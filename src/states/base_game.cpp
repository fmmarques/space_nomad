#include <iostream>

#include <bejeweled/states/base_game.hpp>
#include <yage/input/input_manager.hpp>
#include <yage/graphics/texture_manager.hpp>
#include <yage/graphics/graphics_manager.hpp>
#include <yage/graphics/font_manager.hpp>

namespace bejeweled {
namespace states {
namespace interface1 {

base_game::base_game():
  background{ yage::graphics::texture_manager::instance().load("assets/bejeweled.background.1.jpg") }
, m{ yage::graphics::font_manager::instance().load("assets/PAC-FONT.TTF", 18),"Hi!", 18 }
, grid("assets/grid.background.png", (SDL_Rect){
    .x=640/2 - 32*4 + 102, 480/2-32*4, .w=32*8, .h=32*8})
{
}

base_game::~base_game ()
{
  yage::input::input_manager::instance().unsubscribe(this); 
}

void base_game::on_initialize()
{
  std::string fn{ std::string(__PRETTY_FUNCTION__) + std::string(": ")};

  std::cout << fn << "initializing the game.\n";
  std::cout << fn << "registering the game as keyboard and mouse listener."<< std::endl;
  yage::input::input_manager::instance().subscribe(this); 
}

void base_game::on_pause()
{
  
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "pausing the game.\n";
  std::cout << fn << "unsubscribing the game from events." << std::endl;
  yage::input::input_manager::instance().unsubscribe(this);
}

void base_game::on_continue()
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << "resuming the game.\n";
  std::cout << "reregistering the game as an event listener." << std::endl;
  yage::input::input_manager::instance().subscribe(this);  
}

void base_game::on_deinitialize()
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "deinitializing the game.\n";
  std::cout << fn << "unsubscribing the game from events." << std::endl;
  yage::input::input_manager::instance().unsubscribe(this);
}

void base_game::on_interrupt()
{
}

void base_game::on_frame()
{
  auto&& r = yage::graphics::graphics_manager::instance().get_window();

  SDL_Rect rect { .x=0, 0, 100, 18 };
  SDL_RenderCopy(r, background, NULL, r);
  m.render(&rect);
  grid.on_frame();
}

void base_game::on_keycode_pressed(const SDL_Keysym& keysym) 
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "key sym: " << keysym.sym << std::endl;
}

void base_game::on_keycode_released(const SDL_Keysym& keysym) 
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "key sym: " << keysym.sym << std::endl;
}

void base_game::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "button: " << std::to_string(button.button) << "; state: " << std::to_string(button.state) << "; clicks: " << std::to_string(button.clicks) << "; <x,y>=<" << button.x << "," <<button.y << ">"  << std::endl;

}

void base_game::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  using yage::input::mouse;

  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "button: " << std::to_string(button.button) << "; state: " << std::to_string(button.state) << "; clicks: " << std::to_string(button.clicks) << "; <x,y>=<" << button.x << "," <<button.y << ">" << std::endl;
  
  auto&& m = mouse::instance();
  if (m.hovers(grid)) 
  {
    if (m.is_dragging())
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
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "motion: <" << motion.x << ", " << motion.y << ">" << std::endl; 
}


    }
  
  }
}
