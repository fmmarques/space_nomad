#if !defined(BEJEWELED_WIDGETS_SCORE_HPP)
#  define BEJEWELED_WIDGETS_SCORE_HPP

#include <yage/graphics/label.hpp>
#include <yage/graphics/font.hpp>

#include <bejeweled/widgets/grid_listener.hpp>

namespace bejeweled {
namespace widgets {
namespace interface1 {

class score:
  public yage::widgets::label
{
private:
  yage
public:
	score(font f);
};




}
using namespace interface1;
}
}

#endif
