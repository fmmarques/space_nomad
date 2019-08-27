#if !defined(BEJEWELED_WIDGETS_JEWEL_HPP)
#  define BEJEWELED_WIDGETS_JEWEL_HPP

#include <SDL2/SDL.h>

#include <yage/graphics/spritesheet.hpp>



namespace bejeweled {
namespace widgets {
namespace interface1 {

enum class jewel_type
{
  YELLOW=0, GRAY, BLUE, RED, PURPLE
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
  SDL_Rect screen;
  int col, lin;
  int velocity;

  void invariant();
public:
  jewel(const jewel_type& type, 
        const std::string& spritesheet, 
	const SDL_Rect& screen_coordinates, 
	int velocity,
	int map_x, int map_y,
	int grid_x, int grid_y);
  jewel(jewel&& other);
  jewel(const jewel& other);
  jewel& operator=(const jewel& other);
  jewel& operator=(jewel&& other);

  virtual ~jewel() {}

  inline jewel_type type() const { return _type; }
  inline void type(const jewel_type& v) { _type = v; }

  inline jewel_animation_type animation() const { return _animation; }
  inline void animation(jewel_animation_type v) { _animation = v; }

  inline int  x() const { return screen.x; }
  inline void x(int _x) { screen.x = _x;   }
  
  inline int  y() const { return screen.y; }
  inline void y(int _y) { screen.y = _y;   }

  inline int w() const { return screen.w;  }
  inline void w(int _w) { screen.w = _w; }

  inline int h() const { return screen.h; }
  inline void h(int _h) { screen.h = _h; }


  inline int  map_x() const { return col; }
  inline void map_x(int _x) { col = _x;   }
  
  inline int  map_y() const { return lin; }
  inline void map_y(int _y) { lin = _y;   }



  inline int vel() const { return velocity; }
  inline void vel(int v) { velocity = v; }


  void tick();
  

  bool has_arrived() const;
  bool has_collapsed() const;

  void on_frame(); 
};

}
  using namespace interface1;
}
}

std::ostream& operator<<(std::ostream& oss, const bejeweled::widgets::jewel_type jewel);
std::ostream& operator<<(std::ostream& oss, const bejeweled::widgets::jewel_animation_type jewel);

#endif
