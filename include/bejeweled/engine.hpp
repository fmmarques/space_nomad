#if !defined(BEJEWELED_BEJEWELED_ENGINE)
#  define BEJEWELED_BEJEWELED_ENGINE

# include <yage/engine/game_engine.hpp>
# include <yage/graphics/graphics_manager.hpp>

namespace bejeweled {
namespace interface1 {

class engine: public yage::engine::game_engine< engine >,
	      public yage::engine::runnable
{
private:
  bool continue_execution;
protected:
  engine():
    continue_execution{true}
  {
    using namespace yage::graphics;
    graphics_manager::instance().set_window( 
      window("bejeweled", 640, 480) 
    );  
  }

public:
  static engine& instance() 
  {
    static engine inst{};
    return inst;
  }

  void run() 
  {
    while (continue_execution)
    {
      auto current_state = peek();
      continue_execution = continue_execution & 
    }
  }

  void interrupt()
  {
    continue_execution = false;
  }

}
	                                                  

}
using namespace interface1;
}
}

#endif
