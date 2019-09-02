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
    case jewel_type::RED  :  oss << "red";   break;
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

void jewel::invariant() const
{
  assert(!(_animation != jewel_animation_type::COLLAPSING) || _current.w == 32);
  assert(!(_animation != jewel_animation_type::COLLAPSING) || _current.h == 32);
  assert(!(_animation == jewel_animation_type::IDLE) || _current.w == 32);
}	

jewel::jewel(
 const jewel_type& type,
 const std::string& spritesheet,
 const SDL_Rect& position,
 int velocity
):
  _type{type}
, _animation{jewel_animation_type::IDLE }
, sheet(spritesheet)
, _current{ .x = position.x, position.y, 
            position.w,      position.h  }
, _destination {  .x = position.x, position.y,
                  position.w,      position.h }
, _velocity{velocity}
{
  int x = 0, y = 0, w = 32, h = 32;
  y = static_cast< uint32_t >(_type)*2*h; 
  sheet[_type][jewel_animation_type::COLLAPSING].map_all_of(14, x, y, w, h);  
  sheet[_type][jewel_animation_type::IDLE].map(x,y,w,h);
  sheet[_type][jewel_animation_type::GLOWING].map_all_of(14,x,y+h,h,w);
  invariant();
}

jewel::jewel(jewel&& other):
  _type(std::move(other._type))
, _animation(std::move(other._animation))
, sheet(std::move(other.sheet))
, _current{ .x = other._current.x, other._current.y, 
            other._current.w,      other._current.h }
, _destination{ .x = other._current.x, other._current.y, 
                other._current.w,      other._current.h }
, _velocity(std::move(other._velocity))
{
  invariant();
}

jewel::jewel(const jewel& other):
  _type(other._type)
, _animation(other._animation)
, sheet(other.sheet)
, _current{ .x = other._current.x, other._current.y, 
            other._current.h,      other._current.w }
, _destination{ .x = other._current.x, other._current.y, 
                other._current.h,      other._current.w }
, _velocity(other._velocity)
{
  invariant();
}

jewel& jewel::operator=(jewel&& other)
{
  _type = (std::move(other._type));
  _animation = (std::move(other._animation));
  sheet = (std::move(other.sheet));
  _current = other._current;
  _destination = other._destination;
  _velocity = (std::move(other._velocity));
  invariant();
  return *this;
}

jewel& jewel::operator=(const jewel& other)
{
  _type = other._type;
  _animation = other._animation;
  sheet = other.sheet;
  _current = other._current;
  _destination = other._destination;
  _velocity = other._velocity;

  invariant();
  return *this;
}

jewel_type jewel::type() const { return _type; }
void jewel::type(const jewel_type& v) { _type = v; }

jewel_animation_type jewel::animation() const { return _animation; }
void jewel::animation(jewel_animation_type v) { _animation = v; }

int  jewel::x() const { return _current.x; }
void jewel::x(int _x) { _current.x = _x;   }

int  jewel::y() const { return _current.y; }
void jewel::y(int _y) { _current.y = _y;   }

int jewel::w() const { return _current.w;  }
void jewel::w(int _w) { _current.w = _w; }

int jewel::h() const { return _current.h; }
void jewel::h(int _h) { _current.h = _h; }

int jewel::vel() const { return _velocity; }
void jewel::vel(int v) { _velocity = v; }

int jewel::dx() const { return _destination.x; }
void jewel::dx(int x) { _destination.x = x; }

int jewel::dy() const { return _destination.y; }
void jewel::dy(int y) { _destination.y = y; }

void jewel::on_frame()
{
  invariant(); 
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  //std::cout << fn<< "rendering a " << type << " jewel, with animation " << animation << std::endl;
  sheet[_type][_animation].render(&_current);
  invariant();
}

void jewel::tick()
{
	std::string fn{ std::string(__PRETTY_FUNCTION__) + ": " };
  invariant();

  switch(_animation)
  {
    case jewel_animation_type::COLLAPSING:
      _current.w = ((_current.w > 0) ? _current.w - 1: 0);
		  _current.h = ((_current.h > 0) ? _current.h - 1: 0);
      _current.x = ((_current.w % 2) ? _current.x + 1: _current.x);
      _destination.x = _current.x;
      _current.y = ((_current.h % 2) ? _current.y + 1: _current.y);
      _destination.y = _current.y;

      break;
    default:
      ;
  }

	sheet[_type][_animation].tick();


	if (_destination.x < _current.x)
		_current.x = ((_current.x - _velocity) < _destination.x ?
			             _destination.x:
			             _current.x - _velocity);
	else if (_destination.x > _current.x)
		_current.x = ((_current.x + _velocity) > _destination.x ?
			             _destination.x:
			             _current.x + _velocity);
	else if (_destination.y < _current.y)
		_current.y = ((_current.y - _velocity) < _destination.y ?
			             _destination.y :
			             _current.y - _velocity);
	else if (_destination.y > _current.y)
		_current.y = ((_current.y + _velocity) > _destination.y ?
			             _destination.y:
			             _current.y + _velocity);
	invariant();
}

bool jewel::is_moving() const
{
	bool r;
	invariant();
	r = (_current.x != _destination.x || _current.y != _destination.y) &&
      _velocity != 0 ;
	invariant();
	return r;
}

bool jewel::has_arrived() const
{
	bool r;
	invariant();
	r = _current.x == _destination.x && 
      _current.y == _destination.y;
	invariant();
	return r;
}

bool jewel::is_collapsing() const
{
  bool r;
  invariant();
  r = ( _animation == jewel_animation_type::COLLAPSING && 
        _current.w > 0 &&
        _current.h > 0 );
  invariant();
  return r;
}

bool jewel::has_collapsed() const
{
	bool r;
	invariant();
	r = (_animation == jewel_animation_type::COLLAPSING && _current.w == 0 && _current.h == 0);
	invariant();
  return r;
}

bool jewel::is_collapsable() const
{
  bool r = false;
  invariant();
  r = !has_collapsed() && !is_collapsing() && has_arrived();
  invariant();
  return r;
}

} // interface1
} // widgets
} // bejeweled
	
