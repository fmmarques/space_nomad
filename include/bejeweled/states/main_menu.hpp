#if !defined(STATES_MAIN_MENU_HPP)
#  define STATES_MAIN_MENU_HPP

#  include <yage/engine/game_state.hpp>

namespace bejeweled {
  namespace states {
    namespace interface1 {

class main_menu: public virtual yage::engine::game_state
{
private:
  std::list< std::string > options;
  unsigned int selected_option;
protected:
  main_menu();
  main_menu(main_menu &&) = delete;

public:
  static main_menu& instance();
  
  virtual void on_initialize() override;

  virtual void on_pause();
  virtual void on_continue();
  virtual void on_deinitialize();
  virtual void on_render() = 0;
  virtual void on_update() = 0;

  virtual ~main_menu();


};	

    }
    using interface1::main_menu;
  }
}

#endif
