#if !defined(BEJEWELED_BEJEWELED_ENGINE)
#  define BEJEWELED_BEJEWELED_ENGINE

# include <yage/engine/game_engine.hpp>

namespace bejeweled {
namespace engine {
namespace interface1 {

class bejeweled_engine: public yage::engine::game_engine< bejeweled_engine >
{

protected:
  bejeweled_engine() 
  {
  }

public:
  static bejeweled_engine& instance() {
    static bejeweled_engine engine{};
    return engine;
  }

}
	                                                  

}
using namespace interface1;
}
}

#endif
