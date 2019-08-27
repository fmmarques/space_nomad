#include <cassert>

#include <bejeweled/widgets/jewel.hpp>



std::ostream& operator<<(std::ostream& oss, const bejeweled::widgets::jewel_type jewel)
{
  using bejeweled::widgets::jewel_type;
  switch(jewel)
  {
    case jewel_type::YELLOW: oss << "yellow";    break;
    case jewel_type::GRAY:   oss << "gray"; break;
    case jewel_type::BLUE :  oss << "blue";  break;
    case jewel_type::RED  :  oss << "green";   break;
    case jewel_type::PURPLE: oss << "purple";   break;
    default    : oss.setstate(std::ios_base::failbit);
  }
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const bejeweled::widgets::jewel_animation_type anim)
{
  using bejeweled::widgets::jewel_animation_type;
  switch(anim)
  {
    case jewel_animation_type::IDLE:       oss << "idle";    break;
    case jewel_animation_type::COLLAPSING: oss << "collapsing"; break;
    case jewel_animation_type::GLOWING :   oss << "glowing";  break;
    default    : oss.setstate(std::ios_base::failbit);
  }
  return oss;
}


namespace bejeweled {
namespace widgets {
namespace interface1 {

void jewel::invariant()
{
  assert(screen.x >= 0 && screen.x <= 640);
  assert(screen.y >= 0 && screen.y <= 480);
  assert(animation != jewel_animation_type::COLLAPSING && screen.w > 0);
  assert(animation != jewel_animation_type::COLLAPSING && screen.h > 0);
}	

jewel::jewel(
 const jewel_type& type,
 const std::string& spritesheet_filename,
 const SDL_Rect& screen_pos,
 int velocity,
 int map_line,
 int map_column,
 int grid_x,
 int grid_y
):
  _type{type}
, animation{jewel_animation_type::IDLE }
, sheet{spritesheet_filename}
, screen{ .x=screen_pos.x, .y=screen_pos.y, .w=screen_pos.w, .h=screen_pos.h}
, grid_x{grid_x}
, grid_y{grid_y}
, col{map_column}
, lin{map_line}
, velocity{velocity}
{
  int x = 0, y = 0, w = 32, h = 32;
  y = static_cast< uint32_t >(_type)*2*h; 
  sheet[_type][jewel_animation_type::COLLAPSING].map_all_of(14, x, y, w, h);  
  sheet[_type][jewel_animation_type::IDLE].map(x,y,w,h);
  sheet[_type][jewel_animation_type::GLOWING].map_all_of(14,x,y+h,h,w);
  invariant();
}

//jewel::jewel()
//{}

jewel::jewel(jewel&& other):
  _type(std::move(other._type))
, animation(std::move(other.animation))
, sheet(std::move(other.sheet))
, screen(std::move(other.screen))
, grid_x(std::move(other.grid_x))
, grid_y(std::move(other.grid_y))
, col(std::move(other.col))
, lin(std::move(other.lin))
, velocity(std::move(other.velocity))
{invariant();}

jewel::jewel(const jewel& other):
  _type(other._type)
, animation(other.animation)
, sheet(other.sheet)
, screen(other.screen)
, grid_x(other.grid_x)
, grid_y(other.grid_y)
, col(other.col)
, lin(other.lin)
, velocity(other.velocity)
{invariant();}

jewel& jewel::operator=(jewel&& other)
{
  _type = (std::move(other._type));
  animation = (std::move(other.animation));
  sheet = (std::move(other.sheet));
  screen = (std::move(other.screen));
  lin = (std::move(other.lin));
  col = (std::move(other.col));
  velocity = (std::move(other.velocity));
  invariant();
  return *this;
}

jewel& jewel::operator=(const jewel& other)
{
  _type = other._type;
  animation = other.animation;
  sheet = other.sheet;
  screen = other.screen;
  lin = other.lin;
  col = other.col;
  velocity = other.velocity;
  invariant();
  return *this;
}

void jewel::on_frame()
{
  invariant(); 
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  //std::cout << fn<< "rendering a " << type << " jewel, with animation " << animation << std::endl;
  sheet[_type][animation].render(&screen);
  invariant();
}

void jewel::tick()
{
	std::string fn{ std::string(__PRETTY_FUNCTION__) + ": " };
	
	if (animation == jewel_animation_type::COLLAPSING)
	{
		screen.w = ((screen.w > 0) ? screen.w - 1; screen.w);
		screen.h = ((screen.h > 0) ? screen.h - 1; screen.h);
	}

	sheet[_type][animation].tick();

	if (col*screen.w == screen.x && lin*screen.h == screen.y)
		return;

	if (col*screen.w < screen.x)
		screen.x = ((screen.x - velocity) < col*screen.w ?
			col * screen.w :
			screen.x - velocity);
	else if (col*screen.w > screen.x)
		screen.x = ((screen.x + velocity) > col*screen.w ?
			col * screen.w :
			screen.x + velocity);
	else if (lin*screen.h < screen.y)
		screen.y = ((screen.y + velocity) > col*screen.h ?
			col * screen.h :
			screen.x + velocity);
	else if (col*screen.h > screen.y)
		screen.y = ((screen.y + velocity) > col*screen.h ?
			col * screen.h :
			screen.x + velocity);
	
	if (has_arrived())
		velocity = 0;

	invariant();
}


bool jewel::has_arrived() const
{
	bool r;
	invariant();
	r = screen.x == grid_x + col * screen.w && screen.y == grid_y + lin * screen.h;
	invariant();
	return r;
}

bool jewel::has_collapsed const()
{
	bool r;
	invariant();
	r = (_animation == jewel_animation_type::COLLAPSING && screen.w == 0 && screen.h == 0);
	invariant();
}


} // interface1
} // widgets
} // bejeweled
	
