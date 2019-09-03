
#include <bejeweled/widgets/grid_generators.hpp>

namespace bejeweled {
namespace widgets {
namespace interface1 {


void random_generator::fill_column( std::shared_ptr<jewel> map[COLUMNS][LINES], std::list<  jewel * >& moving, int column, int x, int y)
{
  int falling_jewels = 0;
  int collapsed_jewels = 0;
  for (auto line = 0; line < LINES; line++)
  {
    if (!map[column][line]->has_collapsed())
    {
      falling_jewels++;
      continue;
    }
    collapsed_jewels++;

    for (auto jewels_fallen = 0 ; jewels_fallen < falling_jewels; jewels_fallen++)
    {
      auto current_falling_jewel = line - jewels_fallen - 1 ;
      auto squashed_jewel = line - jewels_fallen;
      std::shared_ptr< jewel > j {};

      j = map[column][current_falling_jewel];

        assert(!j->is_collapsing() && !j->has_collapsed());
        j->dy(squashed_jewel * JEWEL_HEIGHT + y);
        j->vel(4);
        map[column][squashed_jewel] = j;
        moving.insert(moving.end(), j.get());
    }
  }
  
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  for ( auto inserted_jewel = collapsed_jewels; inserted_jewel > 0; inserted_jewel--)
  {
    auto&& j = std::make_shared< widgets::jewel >(make_jewel( x + column * JEWEL_WIDTH, y - inserted_jewel*JEWEL_HEIGHT ));
    auto dy = j->y()+collapsed_jewels * JEWEL_HEIGHT;
    j->dy(dy);
    j->vel(4);
    map[column][collapsed_jewels - inserted_jewel] = j;
    assert(j->dy() != j->y());
    moving.insert(moving.end(), j.get());
  }

}

jewel random_generator::make_jewel(int x, int y)
{
  std::string fn { std::string(__PRETTY_FUNCTION__) + ": " };
  SDL_Rect screen
  {
    .x = static_cast<decltype(SDL_Rect::x)>(x),
    .y = static_cast<decltype(SDL_Rect::y)>(y),
    .w = static_cast<decltype(SDL_Rect::w)>(random_generator::JEWEL_WIDTH),
    .h = static_cast<decltype(SDL_Rect::h)>(random_generator::JEWEL_HEIGHT)
  };
  jewel_type type = jewel_type::YELLOW;
  auto itype = rand() % static_cast< unsigned int >(jewel_type::JEWEL_TYPE_COUNT);
  //auto itype = (x+y*COLUMNS) % static_cast< unsigned int >(jewel_type::JEWEL_TYPE_COUNT);
  type = static_cast<jewel_type>(itype);
  return jewel(type, "assets/gems.spritesheet.transparent.png", screen, 0);
}


}
}
}
