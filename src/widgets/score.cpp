#include <yage/graphics/graphics_manager.hpp>
#include <yage/graphics/font_manager.hpp>

#include <bejeweled/widgets/score.hpp>

namespace bejeweled {
namespace widgets {
namespace interface1 {

score::score(int x, int y):
  _x{x}
, _y{y}
, _label(yage::graphics::font_manager::instance().load("assets/arcade_classic.ttf",18),"Score",(SDL_Color){ .r =0,0,0,0},  18)
, _value(yage::graphics::font_manager::instance().load("assets/arcade_classic.ttf",18),"0", (SDL_Color){ .r =0,0,0,0}, 18)
{}

void score::on_score_change(int score)
{
  _value.text(std::to_string(score));
}

void score::on_frame()
{
  SDL_Rect label_r { .x= _x, _y, 18*6, 18 };
  SDL_Rect score_r { .x= _x+ 18*8, _y , 18*6, 18 };

  _label.render( &label_r );
  _value.render( &score_r );
}


}
}
}
