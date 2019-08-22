#if !defined(STATES_MAIN_MENU_HPP)
#  define STATES_MAIN_MENU_HPP

#  include <list>
#  include <yage/input/input_listener.hpp>
#  include <yage/engine/game_state.hpp>
#  include <yage/graphics/texture.hpp>
namespace bejeweled {
  namespace states {
    namespace interface1 {

enum class option 
{
  UNDEFINED=-1
 ,NEW_GAME 
 ,CONTINUE
 ,EXIT
 
};
	
class main_menu: 
   public virtual yage::input::input_listener
 , public virtual yage::engine::game_state
 , public yage::engine::base_game_state< main_menu >
{
private:
  std::list< std::string > options;
  option selected_option;

  yage::graphics::texture background;
protected:

public:
  main_menu();

// Game state callbacks, 
  void on_initialize() override;
  void on_pause() override;
  void on_continue() override;
  void on_deinitialize() override;
  void on_interrupt();   // not on the interface...
  void on_frame();       // not on the interface...

// keyboard listener callbacks,
  void on_keycode_pressed(const SDL_Keysym& sym) override;
  void on_keycode_released(const SDL_Keysym& sym) override;
// mouse listener callbacks,
  void on_mouse_button_down(const SDL_MouseButtonEvent& button) override;
  void on_mouse_button_up(const SDL_MouseButtonEvent& button) override;
  void on_mouse_movement(const SDL_MouseMotionEvent& button) override;

  virtual ~main_menu();


};	

    }
    using interface1::main_menu;
  }
}

#endif
