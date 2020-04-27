#include <cassert>
#include <list>

#include <bejeweled/widgets/grid.hpp>

namespace bejeweled {
  namespace widgets {
    namespace interface1 {

void grid::move_jewel( jewel* j, 
                       const map_coordinates& map_destination)
{
  assert(j != nullptr);
  assert(0 <= map_destination.first && map_destination.first < COLUMNS);
  assert(0 <= map_destination.second && map_destination.second < LINES);

  auto&& jewel_map_coords = make_map_coordinates_from_jewel(j);
  if (jewel_map_coords.first == -1 || jewel_map_coords.second == -1)
          return ;
  assert( jewel_map_coords.first != map_destination.first ||
          jewel_map_coords.second != map_destination.second);
        
  invariant();
  auto screen_destination = make_screen_coordinates_from_map_coords(map_destination);
  
  j->dx(screen_destination.first);
  j->dy(screen_destination.second);
  j->vel(4);

  moving.insert(moving.end(), j);
  invariant();
}

    }
  }
}
