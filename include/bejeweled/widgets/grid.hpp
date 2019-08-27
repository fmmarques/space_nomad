#if !defined(BEJEWELED_WIDGETS_GRID_HPP)
#  define BEJEWELED_WIDGETS_GRID_HPP

#include <cassert>
#include <set>
#include <queue>
#include <algorithm>

#include <yage/graphics/spritesheet.hpp>
#include <bejeweled/widgets/jewel.hpp>

namespace bejeweled {
namespace widgets {
namespace interface1 {

template < typename map_generator_t > class grid;
class random_generator
{
private:
public:
  random_generator();
  void operator()(std::list< std::shared_ptr< jewel > >&, jewel *[8][8], uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
};

template < typename map_generator_t >
class grid
{
public:
  static const uint32_t LINES=8, COLUMNS=8, SPACES = LINES*COLUMNS;
private:
  yage::graphics::spritesheet< jewel_type, jewel_animation_type > sheet;
  SDL_Rect screen;
  uint32_t jewel_width, jewel_height;
  uint32_t lines, columns;
  std::list< std::shared_ptr< jewel > > store;
  
  std::shared_ptr< jewel > map[8][8];
 
  std::array< std::shared_ptr< jewel >, 2 > selected;  
  std::list<  std::shared_ptr< jewel > > moving;
  std::list<  std::shared_ptr< jewel > > collapsing;

  void invariant()
  {
    bool no_jewel_in_collapsing_is_in_store = true;
    bool no_jewel_in_moving_is_in_store = true;
    bool every_renderable_jewel_is_within_map = true;

    for( auto&& collapsing_jewel : collapsing )
    {
//	no_jewel_in_collapsing_is_in_store = !(std::find(std::begin(store), std::end(store), *collapsing_jewel) == std::end(store));
	every_renderable_jewel_is_within_map &= screen.x <= collapsing_jewel->x() && screen.x+screen.w >= collapsing_jewel->x() && screen.y <= collapsing_jewel->y() && screen.y+screen.h >= collapsing_jewel->y();
    }

    for (auto&& moving_jewel: moving)
    {
//	no_jewel_in_moving_is_in_store = !(std::find(std::begin(store), std::end(store), *moving_jewel) == std::end(store));
	every_renderable_jewel_is_within_map &= screen.x <= moving_jewel->x() && screen.x+screen.w >= moving_jewel->x() && screen.y <= moving_jewel->y() && screen.y+screen.h >= moving_jewel->y();
    
    }

/*    for (auto&& jewel: store)
    {
	every_renderable_jewel_is_within_map &= screen.x <= jewel->x() && screen.x+screen.w >= jewel->x() && screen.y <= jewel->y() && screen.y+screen.h >= jewel->y();
	*/
    }

    assert(no_jewel_in_collapsing_is_in_store);
    assert(no_jewel_in_moving_is_in_store);
    assert(every_renderable_jewel_is_within_map);
  }
public:
  grid(  const std::string& spritesheet, 
	 const SDL_Rect& screen,  
	 uint32_t jewel_width, 
	 uint32_t jewel_height, 
	 uint32_t lines, 
	 uint32_t columns ):
  sheet{spritesheet}
, screen{screen}
, jewel_width{ jewel_width }
, jewel_height{ jewel_height }
, lines{ lines }
, columns{ columns }
, store {}
, map{}
, selected{nullptr, nullptr}
, moving{}
, collapsing{}
{
  std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
  std::cout << fn << "enter";
  assert(0 <= screen.x && screen.x <= 640);
  assert(0 <= screen.y && screen.y <= 480);
  assert(0 < screen.h);
  assert(0 < screen.w); 
  assert(0 < jewel_width);
  assert(0 < jewel_height);
  assert(0 < lines);
  assert(0 < columns);

  uint32_t subgroups_to_create = 5;
  
  map_generator_t()(store, map, (uint32_t) 5, screen.x, screen.y, jewel_width, jewel_height, lines, columns); 
  
  auto&& groups_to_collapse = make_groups();
  for(auto&& group : groups_to_collapse)
  {
    for(auto&& jewel : group)
    {
      collapsing.emplace_back(*jewel);
      map[jewel->map_x()][jewel->mapy()] = nullptr;
      //store.remove(*jewel);
    }
  }
  
  std::cout << fn << "exit"; 
}

  virtual ~grid() {}

  operator SDL_Rect&() 
  { 
    return screen;
  }

  void swap(int f_col, int f_lin, int s_col, int s_lin)
  {
    using bejeweled::widgets::jewel;

    jewel e(jewel_type::YELLOW, 0, 0, 0, 0);
    e = *map[f_col][f_lin];
    *map[f_col][f_lin] = *map[s_col][s_lin];
    *map[s_col][s_lin] = e;

    auto x = map[f_col][f_lin]->map_x();
    auto y = map[f_col][f_lin]->map_y();
    map[f_col][f_lin]->map_x(map[s_col][s_lin]->map_x());
    map[f_col][f_lin]->map_y(map[s_col][s_lin]->map_y());
    map[s_col][s_lin]->map_x(x);
    map[s_col][s_lin]->map_y(y);
  }


  /* Returns a group (3 or more jewels in a line or column) from x,y */
  std::vector< jewel * > make_group_from(int x, int y)
  {
    std::array < jewel *, 8 > tmp;
    std::vector < jewel * > result{};

    int pos = 0;
    for ( int xn = x - 1; xn >= 0 && 
	  map[xn][y]->type() == map[x][y]->type(); 
	  xn--, pos++ )
      tmp[pos] = map[xn][y];

    for ( int xn = x + 1; 
	  xn < 8 && map[xn][y]->type() == map[x][y]->type(); 
	  xn++, pos++ )
      tmp[pos] = map[xn][y];

    if (pos + 1 >= 3)
      result.insert(result.begin(), tmp.begin(), tmp.begin() + pos);

    pos = 0;
    for ( int yn = y - 1; 
	  yn >= 0 && map[x][yn]->type() == map[x][y]->type(); 
	  yn--, pos++ )
      tmp[pos] = (map[x][yn]);

    for ( int yn = y + 1; 
	  yn < 8 && map[x][yn]->type() == map[x][y]->type(); 
	  yn++, pos++ )
      tmp[pos] = (map[x][yn]);

    if (pos + 1 >= 3)
      result.insert(result.begin(), tmp.begin(), tmp.begin() + pos);

    if (result.size() >= 2)
      result.insert(result.begin(), map[x][y]);

    return result;
  }

  /* returns all the groups on the map */
  std::vector< std::vector< jewel * > > make_groups()
  {
    std::vector< std::vector< jewel * > > groups{};
    std::set< jewel * > seen{};

    for (int lin = 0; lin < 8; lin++)
    {
      for (int col = 0; col < 8; col++)
      {
        std::queue< jewel * > next{};
        std::vector< jewel* > group{}, current_group{};

        group.reserve(3);
        current_group.reserve(3);

        if (seen.find(map[col][lin]) != seen.end())
          continue;

        next.emplace(map[col][lin]);
        do 
        {
          auto curr = next.front();
          next.pop();

          current_group = make_group_from(curr->x(), curr->y());
          if (current_group.size() > group.size())
            group = current_group;
          for (auto&& jewel : current_group)
          {
            if (seen.find(jewel) == seen.end())
              next.emplace(jewel);
          }
          seen.emplace(curr);
         } while (!next.empty());
         
	if (group.size() > 0)
          groups.emplace_back(group);
      }
    }
    return groups;
  }
 

  void on_frame()
  {
    std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
    std::cout << fn << "enter"; 

    { // iterate through the store, ticking and rendering every jewel
 /*     auto&& jewel_it = store.begin();
      while (store.end() != jewel_it)
      {
        (*jewel_it)->tick();
        (*jewel_it)->on_frame();
        jewel_it++;
      }
*/
      iterar pelo map em invés da store;
      comentar referências à store.
    }

    { // iterate through the moving collection, ticking and rendering
      auto&& jewel_it = moving.begin();
      while (moving.end() != jewel_it)
      {
        (*jewel_it)->tick(); 
        (*jewel_it)->on_frame();
        if (!(*jewel_it)->has_arrived())
        {
          jewel_it++;
          continue;
        }	
      
        make_group_from((*jewel_it)->map_x(), (*jewel_it)->map_y());
     
        jewel_it = moving.erase(jewel_it);
      }
    }

    { // now do the same to the collapsing collection ticking and rendering
      auto&& jewel_it = collapsing.begin();
      while (jewel_it != collapsing.end())
      {
        (*jewel_it)->tick(); 
        (*jewel_it)->on_frame();
        if (!(*jewel_it)->has_collapsed())
        {
          jewel_it++;
          continue;
        }	

        int x_map, y_map;
        x_map = (*jewel_it)->map_x();
	y_map = (*jewel_it)->map_y();
      
	while (--y_map > 0)
        {
          map[x_map][y_map]->map_y(y_map+1);
	  map[x_map][y_map+1] = map[x_map][y_map];
        }
        jewel_it = collapsing.erase(jewel_it);
      }
    }
    std::cout << fn << "exit";
  }

  void on_dragged(const SDL_MouseButtonEvent& b)
  {
    std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
    std::cout << fn << "enter";
    if (moving.size() >= 0 || collapsing.size() >= 0)
    {
      std::cout << fn << "exit";
      return;
    } 
    std::cout << fn << "exit";
  }

  void on_clicked(const SDL_MouseButtonEvent& b)
  {
    std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
    std::cout << fn << "enter";
    
    if (moving.size() >= 0 || collapsing.size() >= 0)
    {
      std::cout << fn << "exit";
      return;

    }

    auto jewel_column = (b.x - screen.x) / jewel_width;
    auto jewel_line = (b.y - screen.y) / jewel_height ; 
    std::cout << fn << "clicked on <" << jewel_column << ", " << jewel_line<< ">" << std::endl;
    
    if (selected[0] == nullptr) 
    {
      std::cout << fn << "setting first moving piece.";
      //selected[0] = store[jewel_column + jewel_line*columns];
      selected[1] = std::find(map[jewel_column][jewel_line];
    }
    else
    {
      std::cout << fn << "setting second moving piece.";
      //selected[1] = store[jewel_column + jewel_line*columns];
      selected[1] = map[jewel_column][jewel_line];
 
      selected[0]->map_x(selected[1]->map_x()); 
      selected[0]->map_y(selected[1]->map_y());

      selected[1]->map_x(selected[0]->map_x());
      selected[1]->map_y(selected[0]->map_y());

  //  tirar da store?
  // Actualizo o mapa
      {
        int f_x, f_y, s_x, s_y;
        f_x = selected[0]->map_x();
        f_y = selected[0]->map_y();
        s_x = selected[1]->map_x();
        s_y = selected[1]->map_y();
        auto exchange = map[f_x][f_y];
	map[f_x][f_y] = map[s_x][s_y];
	map[s_x][s_y] = exchange;
      } 
   //  Troco a peca,
      {
        auto exchange(*selected[0]);
        *selected[0] = *selected[1];
        *selected[1] = exchange;
             
        moving.insert(moving.cend(),selected[0]);
        moving.insert(moving.cend(),selected[1]);
      }

    }
    std::cout << fn << "exit" << std::endl;
  }

  void lose_focus()
  {
    std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
    std::cout << fn << "enter";

    std::cout << fn << "exit";
  	
  }
  };


}
using namespace interface1;
}
}

#endif
