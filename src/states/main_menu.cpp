#include <iostream>

#include <yage/input/input_manager.hpp>
#include <yage/graphics/texture_manager.hpp>
#include <yage/graphics/graphics_manager.hpp>
#include <yage/graphics/font_manager.hpp>

#include <bejeweled/engine.hpp>
#include <bejeweled/states/main_menu.hpp>
#include <bejeweled/states/base_game.hpp>

#include <yage/input/mouse.hpp>
#include <yage/utility.hpp>


namespace bejeweled {
namespace states {
namespace interface1 {

main_menu::main_menu():
  yage::engine::game_state()
, options{ "New game", "Continue", "Exit" }
, selected_option{option::UNDEFINED}
, title {
    yage::graphics::graphics_manager::instance().get_window().w() / 2,
    yage::graphics::graphics_manager::instance().get_window().h() / 2,
    yage::graphics::font_manager::instance().load("assets/pac-font.ttf",32),
    "Space Nomad",
    (SDL_Color){ .r = 128, 128, 128, 0 }
  }
, background{ yage::graphics::texture_manager::instance().load("assets/bejeweled.background.2.jpg") } //main_menu.background.1.jpg") }
, newgame {
    yage::graphics::graphics_manager::instance().get_window().w() / 2,
    yage::graphics::graphics_manager::instance().get_window().h() / 2,
    1,
    1,
    yage::graphics::font_manager::instance().load("assets/arcade_classic.ttf", 18),
    "New game", 
    [] {  bejeweled::engine::instance().push< bejeweled::states::base_game >(); },
    (SDL_Color) { .r = 0x00, 0x00, 0x00, 0xff },
    (SDL_Color) { .r = 0xf0, 0xf0, 0xf0, 0xff }
   }
,  quitgame {
    yage::graphics::graphics_manager::instance().get_window().w() / 2,
    yage::graphics::graphics_manager::instance().get_window().h() / 2,
    1,
    1,
    yage::graphics::font_manager::instance().load("assets/arcade_classic.ttf", 18),
    "Exit", 
    []{  bejeweled::engine::instance().pop(); 
         SDL_Event q { .type = SDL_QUIT };
         SDL_PushEvent(&q);                   },
    (SDL_Color) { .r = 0x00, 0x00, 0x00, 0xff },
    (SDL_Color) { .r = 0xf0, 0xf0, 0xf0, 0xff }
   }
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
  title.render();
  newgame.render();
  quitgame.render();

}

void main_menu::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
}

void main_menu::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
}

void main_menu::on_mouse_movement(const SDL_MouseMotionEvent& motion)
{
}


    }
  
  }
}
