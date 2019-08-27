#if !defined(BEJEWELED_STATES_GAME_HPP)
#  define BEJEWELED_STATES_GAME_HPP

#  include <list>
#  include <yage/input/input_listener.hpp>
#  include <yage/input/mouse.hpp>
#  include <yage/engine/game_state.hpp>
#  include <yage/graphics/texture.hpp>

#  include <bejeweled/widgets/grid.hpp>
#  include <bejeweled/widgets/jewel.hpp>

namespace bejeweled {
  namespace states {
    namespace interface1 {

class base_game: 
   public virtual yage::input::input_listener
 , public virtual yage::engine::game_state
 , public yage::engine::base_game_state< base_game >
{
private:
  yage::input::mouse mouse;
  yage::graphics::texture background;
  bejeweled::widgets::grid< bejeweled::widgets::random_generator > grid;
protected:

public:
  base_game();
  virtual ~base_game();

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



};	

    }
    using namespace interface1;
  }
}

#endif
