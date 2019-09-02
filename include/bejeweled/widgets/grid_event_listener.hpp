#if !defined(BEJEWELED_WIDGETS_GRID_EVENT_LISTENER_HPP)
#  define BEJEWELED_WIDGETS_GRID_EVENT_LISTENER_HPP

namespace bejeweled {
namespace widgets {
namespace interface1 {

struct grid_event_listener
{
  virtual void on_score_change(int new_score) = 0;
  virtual void on_game_over() = 0;
};

}
using namespace interface1;
}
}


#endif
