#include <cassert>
#include <list>

#include <bejeweled/widgets/grid.hpp>

namespace bejeweled {
namespace widgets {
namespace interface1 {

random_generator::random_generator() 
{}

void random_generator::operator()( 
  std::vector< std::shared_ptr< jewel > >& store,
  jewel *map[8][8],
  uint32_t subgroups_to_create,
  uint32_t map_x_pos,
  uint32_t map_y_pos,
  uint32_t jewel_width,
  uint32_t jewel_height,  
  uint32_t lines,
  uint32_t columns
) {
  const uint8_t JEWEL_TYPES = 5;
  assert(0 <= map_x_pos && map_x_pos <= 640);
  assert(0 <= map_y_pos && map_y_pos <= 480);
  assert(0 < jewel_width);
  assert(0 < jewel_height);
  assert(0 < lines);
  assert(0 < columns);
  
  store.clear();
  srand(time(NULL));
  

  for ( uint32_t line = 0; line < lines; line++ )
  {
    for( uint32_t column = 0; column < columns; column++ )
    {
      SDL_Rect screen 
      {
        .x = static_cast< decltype(SDL_Rect::x)>(map_x_pos + column * jewel_width),
        .y = static_cast< decltype(SDL_Rect::y)>(map_y_pos + line * jewel_height),
	.w = static_cast< decltype(SDL_Rect::w)>(jewel_width),
	.h = static_cast< decltype(SDL_Rect::h)>(jewel_height)
      };

      jewel_type type = jewel_type::YELLOW;
      
      type = static_cast< jewel_type >(rand() % JEWEL_TYPES);

      jewel current(type, "assets/gems.spritesheet.transparent.png", screen, 0, line, column, lines, columns);
      
      store.emplace_back(new jewel(current));
      map[column][line] = store[ column + line*columns ].get();
    }
  }
}


}
}
}
