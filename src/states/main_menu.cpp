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
,  options{ "New game", "Continue", "Exit" }
,  selected_option{option::UNDEFINED}
,  background{ yage::graphics::texture_manager::instance().load("assets/bejeweled.background.2.jpg") } //main_menu.background.1.jpg") }
,  newgame {
   yage::graphics::font_manager::instance().load("assets/arcade_classic.ttf", 18),
    "New game", 18, 
    []{ 
      bejeweled::engine::instance().push< bejeweled::states::base_game >(); 
    }
   }
,  quitgame{
    yage::graphics::font_manager::instance().load("assets/arcade_classic.ttf", 18),
    "Exit", 18, 
    []{ 
      bejeweled::engine::instance().pop(); 
      SDL_Event q { .type = SDL_QUIT };
      SDL_PushEvent(&q);
    }
   }
,  newgame_r { .x=640/2 - newgame.w()/2, 
                  480/2 - newgame.h()/2 - 2 - newgame.h()/2,
                  newgame.w(),
                  newgame.h() }
,  quitgame_r { .x=640/2 - newgame.w()/2, 
                   480/2 - newgame.h()/2 - 2 + newgame.h()*2,
                   newgame.w(),
                   newgame.h()}
 
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
  newgame.render(&newgame_r);
  quitgame.render(&quitgame_r);
}
/*
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
*/
void main_menu::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{

  SDL_Rect m_click { button.x, button.y, 1, 1 };
  if ( newgame_r.x <= button.x && newgame_r.x + newgame_r.w >= button.x && 
       newgame_r.y <= button.y && newgame_r.y + newgame_r.h >= button.y   )
    newgame.on_click();
  if ( quitgame_r.x <= button.x && quitgame_r.x + quitgame_r.w >= button.x && 
       quitgame_r.y <= button.y && quitgame_r.y + quitgame_r.h >= button.y   )
    quitgame.on_click();
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
