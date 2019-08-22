#if !defined(BEJEWELED_BEJEWELED_ENGINE)
#  define BEJEWELED_BEJEWELED_ENGINE

# include <mutex>
# include <shared_mutex>

# include <yage/engine/game_engine.hpp>
# include <yage/graphics/graphics_manager.hpp>

# include <bejeweled/states/main_menu.hpp>

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
    std::cout << "engine::engine(): enter" << std::endl;
    std::cout << "engine::engine(): initializing game's window at 640x480 px" << std::endl;
    yage::graphics::graphics_manager::instance().set_window( 
      yage::graphics::window("bejeweled", 640, 480) 
    );
    push< bejeweled::states::main_menu >();  
    std::cout << "engine::engine(): exit" << std::endl;
  }

public:
  virtual ~engine() 
  {
      
  };
  static engine& instance() 
  {
    static engine inst{};
    std::cout << "engine::instance()" << std::endl;
    return inst;
  }

  void on_loop() 
  {
    
    std::cout << "engine::on_loop(): entry" << std::endl;
    constexpr auto TICKS_IN_SECOND = 1000;
    constexpr auto FRAMES_PER_SECOND = 30;
    constexpr auto TICKS_PER_FRAME = TICKS_IN_SECOND/FRAMES_PER_SECOND;
    bool continue_to_iterate = true;

    {
      std::shared_lock< std::shared_mutex > lock(mutex);
      continue_to_iterate = continue_execution && length() >= 1;
    }

    unsigned int end_tick, start_tick;
    
    auto&& window_and_renderer =  yage::graphics::graphics_manager::instance().get_window();
    while (continue_to_iterate)
    {

      SDL_RenderClear(window_and_renderer);
      start_tick = SDL_GetTicks();	
      auto&& current_state = peek();
      current_state->run();

      
      continue_to_iterate = length() >= 1;
      
      {
      	std::shared_lock< std::shared_mutex > lock(mutex);
        continue_to_iterate &= continue_execution;
        continue_execution = continue_to_iterate;
      }

      end_tick = SDL_GetTicks();

      auto&& elapsed_ticks = end_tick - start_tick;
      if (elapsed_ticks < TICKS_PER_FRAME)
        SDL_Delay(TICKS_PER_FRAME - elapsed_ticks);
      SDL_RenderPresent(window_and_renderer);
    }

    std::cout << "engine::on_loop(): joining event_manager's thread." << std::endl;
    yage::events::event_manager::instance().interrupt();
    std::cout << "engine::on_loop(): exit" << std::endl;
  }

  void on_interrupt()
  {
    std::cout << "engine::on_interrupt(): enter." << std::endl;
    std::shared_lock< std::shared_mutex > lock(mutex);
    continue_execution = false;
    std::cout << "engine::on_interrupt(): exit." << std::endl;
  }

  void on_event(const SDL_Event& event)
  {
    std::cout << "engine::on_event(): enter." << std::endl;
    switch(event.type) 
    {
      case SDL_QUIT:
	std::cout << "engine::on_event(): received SDL_QUIT. Interrupting the engine." << std::endl;
	on_interrupt();
	break;
      default:
	std::cout << "engine::on_event(): received an unhandled event of type " << event.type << "." << std::endl;
    }
    std::cout << "engine::on_event(): exit." << std::endl;
  }
};
	                                                  

}
using namespace interface1;
}

#endif
