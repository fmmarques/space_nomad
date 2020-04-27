#include <yage/graphics/graphics_manager.hpp>
#include <yage/graphics/font_manager.hpp>

#include <bejeweled/widgets/score.hpp>

namespace bejeweled {
namespace widgets {
namespace interface1 {

score::score(int x, int y):
  _x{x}
, _y{y}
, _label(
    x,
    y,
    yage::graphics::font_manager::instance().load("assets/arcade_classic.ttf",24),
    "Score",
    (SDL_Color){ .r =0,0,0,0}
  )
, _value(
    _x,
    _y,
    yage::graphics::font_manager::instance().load("assets/arcade_classic.ttf",24),
    "0",
    (SDL_Color){ .r =0,0,0,0}
  )
{}

void score::on_score_change(int score)
{
  _value.text(std::to_string(score));
}

void score::on_frame()
{
  _label.render();
  _value.render();
}


}
}
}
