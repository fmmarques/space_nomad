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
  jewel_animation_type animation;
  yage::graphics::spritesheet<jewel_type, jewel_animation_type> sheet;
  SDL_Rect screen;
  int grid_x, grid_y;
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

  inline int  x() const { return screen.x; }
  inline void x(int _x) { screen.x = _x;   }
  
  inline int  y() const { return screen.y; }
  inline void y(int _y) { screen.y = _y;   }

  inline int  map_x() const { return col; }
  inline void map_x(int _x) { col = _x;   }
  
  inline int  map_y() const { return lin; }
  inline void map_y(int _y) { lin = _y;   }



  inline int vel() const { return velocity; }
  inline void vel(int v) { velocity = v; }


  inline void tick() 
  {
    std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
    sheet[_type][animation].tick();
    
    if (col*screen.w == screen.x && lin*screen.h == screen.y)
      return;
   
    if (col*screen.w < screen.x)
      screen.x = ((screen.x - velocity) < col*screen.w ? 
		     col * screen.w: 
		     screen.x - velocity);
    else if (col*screen.w > screen.x)
      screen.x = ((screen.x + velocity) > col*screen.w ? 
		      col * screen.w: 
		      screen.x + velocity);
    else if (lin*screen.h < screen.y)
      screen.y = ((screen.y + velocity) > col*screen.h ? 
		      col * screen.h : 
		      screen.x + velocity);
    else if (col*screen.h > screen.y)
      screen.y = ((screen.y + velocity) > col*screen.h ? 
		      col * screen.h : 
		      screen.x + velocity);
  }

  inline bool has_arrived()
  {
    return screen.x == grid_x + col*screen.w && 
	   screen.y == grid_y + lin*screen.h;
  }

  inline bool has_collapsed()
  {
    return (animation == jewel_animation_type::COLLAPSING && screen.w == 0);
  }

  void on_frame(); 
};

}
  using namespace interface1;
}
}

std::ostream& operator<<(std::ostream& oss, const bejeweled::widgets::jewel_type jewel);
std::ostream& operator<<(std::ostream& oss, const bejeweled::widgets::jewel_animation_type jewel);

#endif
