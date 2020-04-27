#include <iostream>

#include <yage/input/input_manager.hpp>
#include <yage/graphics/texture_manager.hpp>
#include <yage/graphics/graphics_manager.hpp>
#include <yage/graphics/font_manager.hpp>

#include <bejeweled/states/base_game.hpp>
#include <bejeweled/engine.hpp>

namespace bejeweled {
namespace states {
namespace interface1 {

base_game::base_game():
  background{ yage::graphics::texture_manager::instance().load("assets/bejeweled.background.1.jpg") }
, score{ 
    yage::graphics::graphics_manager::instance().get_window().w()/2 - bejeweled::widgets::grid::JEWEL_WIDTH * 3,
    yage::graphics::graphics_manager::instance().get_window().h()/2 - bejeweled::widgets::grid::JEWEL_HEIGHT * 5 }
, grid(
    "assets/grid.background.png", 
    (SDL_Rect){
      .x = yage::graphics::graphics_manager::instance().get_window().w()/2 - bejeweled::widgets::grid::JEWEL_WIDTH * 3, 
           yage::graphics::graphics_manager::instance().get_window().h()/2 - bejeweled::widgets::grid::JEWEL_HEIGHT * 4, 
           bejeweled::widgets::grid::JEWEL_WIDTH * bejeweled::widgets::grid::COLUMNS, 
           bejeweled::widgets::grid::JEWEL_HEIGHT * bejeweled::widgets::grid::LINES })
, is_gameover{ false }
, hint(
    0,
    0,
    2,
    2,
    yage::graphics::font_manager::instance().load("assets/arcade_classic.ttf", 32),
    "Help me!",
    [this]{ std::cout << "hint me!" << std::endl; this->grid.on_hint(); },
    (SDL_Color){ 0xFF, 0xFF, 0xFF, 0x0 },
    (SDL_Color){ 0xFF, 0x0, 0x0, 0x00 }
  )
, gameover{
    0,
    0,
    1,
    1,
    yage::graphics::font_manager::instance().load("assets/arcade_classic.ttf", 52),
    "Game over",
    [this]{ 
      std::cout << "goto main menu" << std::endl;
      bejeweled::engine::instance().pop();
    },
    (SDL_Color){ 0xFF, 0xFF, 0xFF, 0x0 },
    (SDL_Color){ 0xFF, 0x0, 0x0, 0x00 }
  }
{
  grid.subscribe((grid_event_listener*)&score);
  grid.subscribe((grid_event_listener*)this);
}

base_game::~base_game ()
{
  grid.unsubscribe(&score);
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
  if (!is_gameover)
  {
    SDL_RenderCopy(r, background, NULL, r);
    score.on_frame();
    grid.on_frame();
    hint.render();
  }
  else
  {
    gameover.render();
  }
}

void base_game::on_keycode_pressed(const SDL_Keysym& keysym) 
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "key sym: " << keysym.sym << std::endl;
}

void base_game::on_keycode_released(const SDL_Keysym& keysym) 
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  switch(keysym.sym)
  {
    case SDLK_ESCAPE:
    case SDLK_BACKSPACE:
      bejeweled::engine::instance().pop();
    default:
      std::cout << fn << "key sym: " << keysym.sym << std::endl;
  }
}

void base_game::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  using yage::input::mouse;
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "button: " << std::to_string(button.button) << "; state: " << std::to_string(button.state) << "; clicks: " << std::to_string(button.clicks) << "; <x,y>=<" << button.x << "," <<button.y << ">"  << std::endl;

  auto&& m = mouse::instance();
  std::cout << "is game over: " << std::boolalpha << is_gameover;
  std::cout << "hovers: " << m.hovers(gameover);
  if (is_gameover && m.hovers(gameover))
  {
    gameover.on_click();
    return;
  }

  if ( m.hovers(grid)) 
  {
    grid.on_clicked(button);
    return;
  }
  grid.lose_focus();

  
  if (m.hovers(hint))
  {
    hint.on_click();
    return;
  }

  
}

void base_game::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  using yage::input::mouse;

  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "button: " << std::to_string(button.button) << "; state: " << std::to_string(button.state) << "; clicks: " << std::to_string(button.clicks) << "; <x,y>=<" << button.x << "," <<button.y << ">" << std::endl;
  
  auto&& m = mouse::instance();
  if (m.hovers(grid)) 
  {
    grid.on_clicked(button);
  }
  else
  {
    grid.lose_focus();
  }
}

void base_game::on_mouse_movement(const SDL_MouseMotionEvent& motion)
{
}

void base_game::on_score_change(int new_score) {}
void base_game::on_game_over() 
{
  is_gameover = true;
}

    }
  
  }
}
