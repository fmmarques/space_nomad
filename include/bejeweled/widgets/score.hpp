#if !defined(BEJEWELED_WIDGETS_SCORE_HPP)
#  define BEJEWELED_WIDGETS_SCORE_HPP

#include <yage/graphics/font.hpp>
#include <yage/widgets/label.hpp>

#include <bejeweled/widgets/grid_event_listener.hpp>

namespace bejeweled {
namespace widgets {
namespace interface1 {

class score:
  public virtual grid_event_listener
{
private:
  int _x, _y;
  yage::widgets::label _label, _value;

public:
  score(int x, int y);

  void on_score_change(int score);
  void on_game_over() {};
  void on_frame();
};




}
using namespace interface1;
}
}

#endif
