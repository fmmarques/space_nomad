#if !defined(BEJEWELED_WIDGETS_GRID_EVENT_LISTENER_HPP)
#  define BEJEWELED_WIDGETS_GRID_EVENT_LISTENER_HPP


struct grid_event_listener
{
  virtual void on_score_change(int new_score) = 0;
};


#endif
