#if !defined(BEJEWELED_BEJEWELED_ENGINE)
#  define BEJEWELED_BEJEWELED_ENGINE

# include <yage/engine/game_engine.hpp>
# include <yage/graphics/graphics_manager.hpp>

namespace bejeweled {
namespace interface1 {

class bej_engine: public yage::engine::game_engine< bej_engine >,
	      public yage::engine::runnable
{
private:
  bool continue_execution;
protected:
  bej_engine():
    continue_execution{true}
  {
    using namespace yage::graphics;
    graphics_manager::instance().set_window( 
      window("bejeweled", 640, 480) 
    );  
  }

public:
  static bej_engine& instance() 
  {
    static bej_engine inst{};
    return inst;
  }

  void run() 
  {
    while (continue_execution)
    {
      auto&& current_state = peek();
      current_state->run();
    }
  }

  void interrupt()
  {
    continue_execution = false;
  }

};
	                                                  

}
using namespace interface1;
}

#endif
