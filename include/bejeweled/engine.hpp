#if !defined(BEJEWELED_BEJEWELED_ENGINE)
#  define BEJEWELED_BEJEWELED_ENGINE

# include <mutex>
# include <shared_mutex>

# include <yage/engine/game_engine.hpp>
# include <yage/graphics/graphics_manager.hpp>

# include <bejeweled/states/main_menu.hpp>
# include <bejeweled/states/base_game.hpp>

namespace bejeweled {
namespace interface1 {

class engine: 
  public yage::engine::game_engine< engine >
{
private:
  bool continue_execution;
  mutable std::shared_mutex mutex;
protected:
  engine():
    continue_execution{true}
  {
    std::string fn{ std::string(__PRETTY_FUNCTION__) + std::string(": ") };
    std::cout << fn << "initializing game's window at 640x480 px" << std::endl;
    yage::graphics::graphics_manager::instance().set_window( 
      yage::graphics::window("bejeweled", 640, 480) 
    );
    push< bejeweled::states::main_menu >(); 
    push< bejeweled::states::base_game >();
  }

public:
  virtual ~engine() 
  {
      
  };
  static engine& instance() 
  {
    static engine inst{};
    return inst;
  }

  bool on_loop() 
  {
    std::string fn{ std::string( __PRETTY_FUNCTION__ ) + std::string(": ") };

    constexpr auto TICKS_IN_SECOND = 1000;
    constexpr auto FRAMES_PER_SECOND = 15;
    constexpr auto TICKS_PER_FRAME = TICKS_IN_SECOND/FRAMES_PER_SECOND;


    unsigned int end_tick, start_tick;
    
    auto&& window_and_renderer =  yage::graphics::graphics_manager::instance().get_window();

    SDL_RenderClear(window_and_renderer);
    
    start_tick = SDL_GetTicks();
    if (length() == 0)
    {
      std::cout << fn << "no more states; returning false";
      return false;
    }
    auto&& current_state = peek();
    current_state->run();
    end_tick = SDL_GetTicks();

    auto&& elapsed_ticks = end_tick - start_tick;
    if (elapsed_ticks < TICKS_PER_FRAME)
    {
      SDL_Delay(TICKS_PER_FRAME - elapsed_ticks);
    }
    else 
    { 
      std::cout << fn << "warning: computation took too long and the next frame was missed" << std::endl;
    }
    SDL_RenderPresent(window_and_renderer);

    return true;
  }

  void on_interrupt()
  {
    std::string fn { std::string(__PRETTY_FUNCTION__) + std::string(": ") };

    std::shared_lock< std::shared_mutex > lock(mutex);
    std::cout << fn << "interrupting execution." << std::endl;
    continue_execution = false;
  }
};

}
using namespace interface1;
}

#endif
