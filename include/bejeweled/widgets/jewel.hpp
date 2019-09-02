#if !defined(BEJEWELED_WIDGETS_JEWEL_HPP)
#  define BEJEWELED_WIDGETS_JEWEL_HPP

#include <SDL2/SDL.h>

#include <yage/graphics/spritesheet.hpp>



namespace bejeweled {
namespace widgets {
namespace interface1 {

enum class jewel_type
{
  YELLOW=0, GRAY, BLUE, RED, PURPLE,

  JEWEL_TYPE_COUNT
};


enum class jewel_animation_type 
{
  IDLE = 0, COLLAPSING, GLOWING
};


class jewel
{
private:
  static const uint8_t MOVEMENT_VELOCITY = 1;
private:
  
  jewel_type _type;
  jewel_animation_type _animation;
  yage::graphics::spritesheet<jewel_type, jewel_animation_type> sheet;
  SDL_Rect _current;
  SDL_Rect _destination;
  int _velocity;
  void invariant() const;
public:
  jewel(const jewel_type& type, 
        const std::string& spritesheet, 
        const SDL_Rect& coordinates, 
	      int velocity);
  jewel(jewel&& other);
  jewel(const jewel& other);
  jewel& operator=(const jewel& other);
  jewel& operator=(jewel&& other);

  virtual ~jewel() {}

  jewel_type type() const;
  void type(const jewel_type& v);

  jewel_animation_type animation() const;
  void animation(jewel_animation_type v);

  int  x() const; 
  void x(int _x); 
  
  int  y() const; 
  void y(int _y); 

  int w() const;
  void w(int _w);

  int h() const;
  void h(int _h);


  int vel() const;
  void vel(int v);

  int dx() const;
  void dx(int x);

  int dy() const;
  void dy(int y); 
  
  void tick();
  
  bool is_moving() const;
  bool has_arrived() const;
  bool is_collapsing() const;
  bool has_collapsed() const;
  bool is_collapsable() const;

  void on_frame(); 
};

}
  using namespace interface1;
}
}

std::ostream& operator<<(std::ostream& oss, const bejeweled::widgets::jewel_type jewel);
std::ostream& operator<<(std::ostream& oss, const bejeweled::widgets::jewel_animation_type jewel);

#endif
