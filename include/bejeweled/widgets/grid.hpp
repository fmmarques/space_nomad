#if !defined(BEJEWELED_WIDGETS_GRID_HPP)
#  define BEJEWELED_WIDGETS_GRID_HPP

#include <cassert>
#include <set>
#include <queue>
#include <algorithm>
#include <array>

#include <yage/graphics/spritesheet.hpp>
#include <yage/input/mouse.hpp>


#include <bejeweled/widgets/grid_event_listener.hpp>
#include <bejeweled/widgets/grid_generators.hpp>
#include <bejeweled/widgets/jewel.hpp>

namespace bejeweled {
namespace widgets {
    namespace interface1 {


    class grid
    {
    public:
      static const int LINES = 8, COLUMNS = 8, SPACES = LINES * COLUMNS, JEWEL_WIDTH = 32, JEWEL_HEIGHT = 32;
      using map_coordinates  = std::pair< uint8_t, uint8_t> ;
      using screen_coordinates  = std::pair<int, int> ;
    private:
      yage::graphics::spritesheet< jewel_type, jewel_animation_type > sheet;
      SDL_Rect screen;

      /// the logical map
      std::shared_ptr< jewel > map[COLUMNS][LINES];

      /// the counter of free slot per column
      //std::array< std::queue< int >, COLUMNS > columns_with_free_slots;

      /// the jewels in motion
      std::list<  jewel * > moving;

      /// the jewels collapsing
      std::list<  jewel * > collapsing;

      /// the next subgroups
      std::list< jewel * > hints;

      /// the jewels selected by the user;
      std::vector< std::weak_ptr< jewel > > selected;

      int _score;

      std::list< grid_event_listener * > event_listeners;


      void invariant()
      {
        assert(_score >= 0);
        for (auto&& j : collapsing)
        {
          assert(j->is_collapsing() || j->has_collapsed());
          assert(j->has_arrived() && !j->is_moving());
        }

        for (auto&& j : moving)
        {
          assert(j->is_moving() || j->has_arrived());
          //assert(!(j->is_moving()) || j->vel() == 1);
          assert(!j->has_collapsed() && !j->is_collapsing());
        }

      }

protected:
// Score related methods
        // Increments the score.
        // @arg the score
        void score(int increment) 
        {
          
          std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
          assert(increment > 0);
          assert(_score >= 0);
          std::cout << fn << "incrementing the score in " << increment << " yielding " << _score << "." << std::endl;
          _score += increment;
          for (auto&& listener : event_listeners)
            listener->on_score_change(_score);
        }

// Coordinate related methods,
        // Creates map coordinates from map's column and lin
        // @arg col the column
        // @arg lin the line
        // @returns { -1 , -1 } if these are not map coordinates, and { col, lin } otherwise.
        map_coordinates make_map_coordinates_from_col_and_lin(uint8_t col, uint8_t lin)
        {
          map_coordinates result{ -1, -1 };
          
          //assert(col >= 0 && col <= 7 && lin >= 0 && lin <= 7);

          result.first = col;
          result.second = lin;

          return result;
        }

        // Creates map coordinates from screen coordinates.
        // @x int the x screen coordinate
        // @y int the y screen coordinate
        // @returns { -1, -1 } if screen coordinates are outside of the map. 
        //                     if screen coordinates are inside the map, returns an { col, lin } coordinate where 0 <= col, lin <= 7.
        map_coordinates make_map_coordinates_from_screen(int x, int y)
        {
          map_coordinates result { -1, -1 };
          uint8_t col = -1, lin = -1;

          col = static_cast< uint8_t >((x - screen.x ) / JEWEL_WIDTH);
          lin = static_cast< uint8_t >((y - screen.y ) / JEWEL_HEIGHT);

          return make_map_coordinates_from_col_and_lin(col, lin);
        }


        // Creates coordinates from screen coordinates.
        // @see make_map_coordinates_from(int, int)
        map_coordinates make_map_coordinates_from_screen_coords(const screen_coordinates& coords)
        {
          return make_map_coordinates_from_screen( coords.first, coords.second);
        }

        // Creates coordinates from screen coordinates.
        // @see make_map_coordinates_from(int, int)
        map_coordinates make_map_coordinates_from_jewel(const jewel * const j)
        {
          return make_map_coordinates_from_screen(j->x(), j->y());
        }

        // Creates a screen coordinate from map coordinates
        // @arg coords map coordinates
        // @return {-1,-1} if the map coordinates are invalid; { x, y }, where x and y are the mapped to screen coords.
        screen_coordinates make_screen_coordinates_from_map_coords(const map_coordinates& coords)
        {
          screen_coordinates result { -1, -1 };
          auto x = coords.first * JEWEL_WIDTH + screen.x;
          auto y = coords.second  * JEWEL_HEIGHT + screen.y;

          if (x > 0 && y > 0)
          {
            result.first = x;
            result.second = y;
          }
          return result;
        }



// Jewel related methods,
        // Initiates jewel movement by setting jewel's destination and velocity, and adding it to the moving bag
        // @param j the jewel to be moved
        // @param map_destination the map coordinates holding the destination.
        void move_jewel(jewel* j, const map_coordinates& map_destination)
        {
          assert(j != nullptr);
          assert(0 <= map_destination.first && map_destination.first <= COLUMNS);
          assert(0 <= map_destination.second && map_destination.second <= LINES);

          auto&& map_origin = make_map_coordinates_from_jewel(j);
          if (map_origin.first == -1 || map_origin.second == -1)
            return ;
          assert(map_origin.first != map_destination.first || map_origin.second != map_destination.second);
          
          invariant();
          auto screen_destination = make_screen_coordinates_from_map_coords(map_destination);
          
          j->dx(screen_destination.first);
          j->dy(screen_destination.second);
          j->vel(4);
          
          moving.insert(moving.end(), j);
          
          invariant();
        }

        // Collapses a jewel by initiating the collapse animation and adding the jewel to the collapsing bag
        // @arg j the jewel that undergoes the collapse.
        void collapse_jewel(jewel* j)
        {
          assert(j != nullptr);
          //invariant();
          //
          assert(!j->is_moving());
          assert(!j->has_collapsed());
          assert(!j->is_collapsing());
          assert(j->has_arrived());
          j->animation(jewel_animation_type::COLLAPSING);
          collapsing.insert(collapsing.end(), j);

          //invariant();
        }

// Group related methods
        // Searches for first matching jewel of type, in a cruciform pattern, from a given coordinate.
        jewel* get_jewel_in_vicinity_with_type(const map_coordinates& center, const jewel_type& t, const std::vector< map_coordinates > exceptions)
        {
          std::string fn { "get_j_vic: " }; //std::string( __PRETTY_FUNCTION__ ) + ": "};
          if (!coords_are_valid(center))
          {
            std::cout << fn << "ignoring " << (int) center.first << ", " << (int)center.second << std::endl;
            return nullptr;
          }

          auto c = center;
          auto col = c.first;
          auto lin = c.second;
          jewel * result = nullptr;
          assert(map[col][lin]->type() != t);

          if ((col - 1 >= 0) && map[col-1][lin]->type() == t && map[col-1][lin]->is_collapsable())
          {
            c.first -= 1;
            if (std::find(std::cbegin(exceptions), std::cend(exceptions), c) == std::cend(exceptions))
              result = map[col-1][lin].get();
            c.first +=1;
          }

          if (!result && (col + 1 <= COLUMNS) && map[col+1][lin]->type() == t && map[col + 1][lin]->is_collapsable())
          {
            c.first += 1;
            if (std::find(std::cbegin(exceptions), std::cend(exceptions), c) == std::cend(exceptions))
              result = map[col+1][lin].get();
            c.first -=1;
          }
          
          if (!result && (lin - 1 >= 0) && map[col][lin-1]->type() == t && map[col][lin-1]->is_collapsable())
          {  
            c.second -= 1;
            if (std::find(std::cbegin(exceptions), std::cend(exceptions), c) == std::cend(exceptions))
              result = map[col][lin-1].get();
            c.second += 1;
          }
          
          if (!result && (lin + 1 <= COLUMNS) && map[col][lin+1]->type() == t && map[col][lin+1]->is_collapsable())
          {  
            c.second += 1;
            if (std::find(std::cbegin(exceptions), std::cend(exceptions), c) == std::cend(exceptions))
              result = map[col][lin+1].get();
            c.second -= 1;
          }

          if (result)
          {
            auto m = make_map_coordinates_from_jewel(result);
            std::cout << fn << "< < " << (int)col << ", " << (int)lin << " >, < " << (int)m.first << ", " << (int)m.second << "> >, " << t << std::endl;
          } 
          

          return result;
        }

        bool coords_are_valid(const map_coordinates& coords)
        {
          return coords.first >= 0 && coords.first <= COLUMNS && coords.second >= 0 && coords.second <= LINES && map[coords.first][coords.second].get() != nullptr;
        }
        

        std::list< jewel * > make_hints_from_jewel(jewel *j)
        {
/* This function follows the diamond search pattern, in the fig below. We first search for the diagonals (B)
 * and if any are the same type as A, we then search for C's, clockwise
        C
       BOB
      COAOC
       BOB
        C
   
*/
          std::string fn { "hint from jewel: " }; 
          
          std::list< jewel * > subgroup {};
          assert(j != nullptr);
          if (!j->is_collapsable())
            return subgroup;
          
          std::vector < jewel * > exceptions {};

          auto cc = make_map_coordinates_from_jewel(j);
          auto ulc = make_map_coordinates_from_col_and_lin( cc.first - 1, cc.second - 1 );
          auto urc = make_map_coordinates_from_col_and_lin( cc.first + 1, cc.second - 1 );
          auto dlc = make_map_coordinates_from_col_and_lin( cc.first - 1, cc.second + 1 );
          auto drc = make_map_coordinates_from_col_and_lin( cc.first + 1, cc.second + 1 );
          auto tlc = make_map_coordinates_from_col_and_lin( cc.first - 2, cc.second);
          auto tuc = make_map_coordinates_from_col_and_lin( cc.first, cc.second - 2 );
          auto trc = make_map_coordinates_from_col_and_lin( cc.first + 2, cc.second );
          auto tdc = make_map_coordinates_from_col_and_lin( cc.first, cc.second + 2 );

          
          //subgroup.insert(subgroup.end(), j);
          if (coords_are_valid(ulc) && j->type() == map[ulc.first][ulc.second]->type() && map[ulc.first][ulc.second]->is_collapsable())
          // Up left quadrant
          {
            //subgroup.insert(subgroup.end(), map[ulc.first][ulc.second].get());
            if (coords_are_valid(tuc) && j->type() == map[tuc.first][tuc.second]->type() && map[tuc.first][tuc.second]->is_collapsable())
            {
              fn += "ulc, tuc";
              subgroup.insert(subgroup.end(), map[ulc.first][ulc.second].get());
              subgroup.insert(subgroup.end(), map[ulc.first + 1][ulc.second].get());
              goto print_subgroup;
            }
            else if (coords_are_valid(tlc) && j->type() == map[tlc.first][tlc.second]->type() && map[tlc.first][tlc.second]->is_collapsable())
            {
              fn += "ulc, tlc";
              subgroup.insert(subgroup.end(), map[ulc.first][ulc.second].get());
              subgroup.insert(subgroup.end(), map[ulc.first][tlc.second + 1].get());
              goto print_subgroup;
            }
          }
          else if (coords_are_valid(urc) && j->type() == map[urc.first][urc.second]->type() && map[urc.first][urc.second]->is_collapsable())
          // Up right quadrant
          {
            if (coords_are_valid(tuc) && j->type() == map[tuc.first][tuc.second]->type() && map[tuc.first][tuc.second]->is_collapsable())
            {
              fn += "urc, tuc";
              subgroup.insert(subgroup.end(), map[urc.first][urc.second].get());
              subgroup.insert(subgroup.end(), map[urc.first - 1][urc.second].get());
            }
            else if ( coords_are_valid(trc) && j->type() == map[trc.first][trc.second]->type() && map[trc.first][trc.second]->is_collapsable())
            {
              fn += "urc, trc";
              subgroup.insert(subgroup.end(), map[urc.first][urc.second].get());
              subgroup.insert(subgroup.end(), map[urc.first][urc.second + 1].get());
              goto print_subgroup;
            }
          }
          else if (coords_are_valid(drc) && j->type() == map[drc.first][drc.second]->type() && map[drc.first][drc.second]->is_collapsable())
          // Down right quadrant
          {
            if (coords_are_valid(trc) && j->type() == map[trc.first][trc.second]->type() && map[trc.first][trc.second]->is_collapsable())
            {
              fn += "drc, trc";
              subgroup.insert(subgroup.end(), map[drc.first][drc.second].get());
              subgroup.insert(subgroup.end(), map[drc.first][drc.second - 1].get());
            }
            else if ( coords_are_valid(tdc) && j->type() == map[tdc.first][tdc.second]->type() && map[tdc.first][tdc.second]->is_collapsable())
            {
              fn += "drc, tdc";
              subgroup.insert(subgroup.end(), map[drc.first][drc.second].get());
              subgroup.insert(subgroup.end(), map[drc.first - 1][drc.second].get());
              goto print_subgroup;
            }
          }
          else if (coords_are_valid(dlc) && j->type() == map[dlc.first][dlc.second]->type() && map[dlc.first][dlc.second]->is_collapsable())
          // Down left quadrant
          {
            if (coords_are_valid(tdc) && j->type() == map[tdc.first][tdc.second]->type() && map[tdc.first][tdc.second]->is_collapsable())
            {
              fn += "dlc, tdc";
              subgroup.insert(subgroup.end(), map[dlc.first][dlc.second].get());
              subgroup.insert(subgroup.end(), map[dlc.first + 1][dlc.second].get());
            }
            else if (coords_are_valid(tlc) && j->type() == map[tlc.first][tlc.second]->type() && map[tlc.first][tlc.second]->is_collapsable())
            {
              fn += "dlc, tlc";
              subgroup.insert(subgroup.end(), map[dlc.first][dlc.second].get());
              subgroup.insert(subgroup.end(), map[dlc.first][dlc.second - 1].get());
              goto print_subgroup;
            }
          }




print_subgroup:
          if (!subgroup.empty())
          {
          std::cout << fn << " { ";
          for ( auto j : subgroup )
          {
            auto m = make_map_coordinates_from_jewel(j);
            std::cout << "{ .col " << (int) m.first << " .lin " << (int) m.second << " } ";
          }
          std::cout << " } " << std::endl;
          }
          return subgroup;

        }
       
        std::list< jewel * > make_hints_from_horizontal_subgroup(const std::vector< jewel * >& subgroup)
        {
          std::string fn { "make_hints_hor " };//std::string( __PRETTY_FUNCTION__ ) + ": "};
          std::list< jewel * > result {};
          jewel *center = nullptr, *right = nullptr, *candidate = nullptr;
          map_coordinates center_coords, right_coords;

          assert (subgroup.size() <= 2);
          
          right = center = *(subgroup.begin());
          right_coords = center_coords = make_map_coordinates_from_jewel( center );
          for (auto curr_jewel : subgroup)
          {
            auto curr_coords = make_map_coordinates_from_jewel(curr_jewel);
            if (right_coords.first < curr_coords.first)
            {
              right = curr_jewel;
              right_coords = make_map_coordinates_from_jewel(right);
            }
            else if (center_coords.first > curr_coords.first)
            {
              center = curr_jewel;
              center_coords = make_map_coordinates_from_jewel(center);
            }
            std::cout << "<" << (int)curr_coords.first << ", " << (int)curr_coords.second << ">\n";
          }
          assert( (right != nullptr) ); //it's either a vertical or a horizontal subgroup.
          assert( (center != right) );
          assert( center->type() == right->type());
          assert( center_coords.first < right_coords.first );
          assert( right->is_collapsable() && center->is_collapsable());
          
          map_coordinates one_left_coords = center_coords, 
                          one_right_coords = right_coords;
          one_left_coords.first -= 1;
          one_right_coords.first += 1;

          if (one_left_coords.first >= 0 && (candidate = get_jewel_in_vicinity_with_type(one_left_coords, center->type(), { center_coords }))) 
          // we found a candidate for swapping with the jewel above the subgroup
          {
            result.insert(result.end(), map[one_left_coords.first][one_left_coords.second].get());
            result.insert(result.end(), candidate);
          }
          else if (one_right_coords.first <= COLUMNS && (candidate = get_jewel_in_vicinity_with_type(one_right_coords, center->type(), { right_coords } )))
          // we found a candidate for swapping with the jewel below the subgroup
          {
            result.insert(result.end(), map[one_right_coords.first][one_right_coords.second].get());
            result.insert(result.end(), candidate);
          }

          std::cout << fn << "(" << (int)center_coords.first << ", " << (int)center_coords.second << "){ ";
          for (auto&& j: result)
          {
            auto m = make_map_coordinates_from_jewel( j);
            std::cout << " { .col " << (int) m.first << " .lin " << (int) m.second << " } ";
          }
          std::cout << " } " << std::endl;
          return result;
        }
 
        std::list< jewel * > make_hints_from_vertical_subgroup(const std::vector< jewel * >& subgroup)
        {
          std::string fn { "make_hints_ver" };//std::string( __PRETTY_FUNCTION__ ) + ": "};
          std::list< jewel * > result {};
          jewel *center = nullptr, *bottom = nullptr, *candidate = nullptr;
          map_coordinates center_coords, bottom_coords ;

          assert (subgroup.size() <= 2);
          
          bottom = center = *(subgroup.begin());
          bottom_coords = center_coords = make_map_coordinates_from_jewel( center );
          for (auto&& curr_jewel : subgroup)
          {

            auto curr_coords = make_map_coordinates_from_jewel(curr_jewel);
            if (bottom_coords.second < curr_coords.second)
            {
              bottom = curr_jewel;
              bottom_coords = make_map_coordinates_from_jewel(bottom);
            }
            else if (center_coords.second > curr_coords.second)
            {
              center = curr_jewel;
              center_coords = make_map_coordinates_from_jewel(center);
            }

          }
          assert( (bottom != nullptr) );
          assert( bottom != center );
          assert( center_coords.second < bottom_coords.second  && center_coords.first == bottom_coords.first );

          map_coordinates above_coords = center_coords, below_coords = bottom_coords;
          above_coords.second -= 1;
          below_coords.second += 1;
          if (above_coords.second >= 0 && (candidate = get_jewel_in_vicinity_with_type(above_coords, center->type(), { center_coords } ))) 
          // we found a candidate for swapping with the jewel above the subgroup
          {
            result.insert(result.end(), map[above_coords.first][above_coords.second].get());
            result.insert(result.end(), candidate);
          }
          else if (below_coords.first <= COLUMNS && (candidate = get_jewel_in_vicinity_with_type(below_coords, center->type(), { bottom_coords })))
          // we found a candidate for swapping with the jewel below the subgroup
          {
            result.insert(result.end(), map[below_coords.first][below_coords.second].get());
            result.insert(result.end(), candidate);
          }
          
          std::cout << fn << "(" << (int)center_coords.first << ", " << (int)center_coords.second << "){ ";
          for ( auto j : result )
          {
            auto m = make_map_coordinates_from_jewel(j);
            std::cout << " { .col " <<  (int) m.first << " .lin " << (int) m.second << " } ";
          }
          std::cout << " } " << std::endl;
          return result;
        }

         // Returns a group from the grid coordinate given
         // @arg col the column
         // @arg lin the lin
         // @return a vector with the jewels to collapse
         // @note this is an auxiliary method to determine which jewels should collapse
        std::vector< jewel * > make_collapsing_group_from(int col, int lin)
        {

          std::array < jewel *, 8 > subgroup {};
          std::vector < jewel * > group{};
          std::list< jewel *> hint{};
          assert(col < 8 &&  lin < 8);
          if (map[col][lin]->has_collapsed() || map[col][lin]->is_collapsing() || map[col][lin]->is_moving())
            return group;


          int pos = 0, top_most_subgroup_element = 0, bottom_most_subgroup_element = 0;
          for ( int coln = col - 1;
                coln >= 0 && 
                !map[coln][lin]->has_collapsed() && !map[coln][lin]->is_collapsing() && !map[coln][lin]->is_moving() && 
                map[coln][lin]->type() == map[col][lin]->type();
                coln--, pos++, top_most_subgroup_element++)
          {
            subgroup[pos] = map[coln][lin].get();
          }

          for ( int coln = col + 1;
                coln < 8 && 
                !map[coln][lin]->has_collapsed() && !map[coln][lin]->is_collapsing() && !map[coln][lin]->is_moving() && 
                map[coln][lin]->type() == map[col][lin]->type();
                coln++, pos++, bottom_most_subgroup_element++) 
          {
            subgroup[pos] = map[coln][lin].get();
          }
 
          if (pos >= 2) // yey, a group
          {
            group.insert(group.begin(), subgroup.begin(), subgroup.begin() + pos);
          }
          else if (pos && hints.empty())
          {
            auto subgroup_copy = std::vector< jewel *>{ map[col][lin].get(), subgroup[0] };
            hint = make_hints_from_horizontal_subgroup(subgroup_copy);
          }
          else if (hints.empty())
          {
            hint = make_hints_from_jewel( map[col][lin].get() );
          }

          pos = 0;
          for ( int linn = lin - 1;
                linn >= 0 && 
                !map[col][linn]->has_collapsed() && !map[col][linn]->is_collapsing() && !map[col][linn]->is_moving() && 
                map[col][linn]->type() == map[col][lin]->type();
                linn--, pos++)
          {
            subgroup[pos] = (map[col][linn]).get();
          }

          for ( int linn = lin + 1;
                linn < 8 && 
                !map[col][linn]->has_collapsed() && !map[col][linn]->is_collapsing() && !map[col][linn]->is_moving() && 
                map[col][linn]->type() == map[col][lin]->type();
                linn++, pos++)
          {
            subgroup[pos] = (map[col][linn]).get();
          }

          if (pos >= 2) // yey, a group
          {
            group.insert(group.begin(), subgroup.begin(), subgroup.begin() + pos);
          }
          else if (pos && hints.empty())
          {
            auto subgroup_copy = std::vector< jewel *>{ map[col][lin].get(), subgroup[0] };
            hint = make_hints_from_vertical_subgroup(subgroup_copy);
          }
          else if (hints.empty())
          {
            hints = make_hints_from_jewel( map[col][lin].get() );
          }
          
          if (group.size() >= 2)
          {
            group.insert(group.begin(), map[col][lin].get());
            hint.clear();
          }
          hints.insert(hints.end(), hint.begin(), hint.end());

          { // invariants assert
            for ( auto&& j : group )
            {
              assert(!j->is_collapsing());
              assert(!j->is_moving());
              assert(j->has_arrived());
              assert(!j->has_collapsed());
            }

          }
          return group;
        }

        // Initiates the collapsing sequence for all the groups on the map.
        // @note calls the collapse_jewel for every jewel on the group
        void collapse_existing_groups()
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

              if (seen.find(map[col][lin].get()) != seen.end())
                continue;

              next.emplace(map[col][lin].get());
              do
              {
                auto curr = next.front();
                next.pop();

                if (curr->is_moving() || curr->is_collapsing() || curr->has_collapsed())
                  continue;

                auto curr_coords = make_map_coordinates_from_jewel(curr);
                assert(curr_coords.first >= 0 && curr_coords.first <= COLUMNS);
                assert(curr_coords.second >= 0 && curr_coords.second <= LINES); 
                current_group = make_collapsing_group_from(curr_coords.first, curr_coords.second);
                if (current_group.size() > group.size())
                  group = current_group;
                for (auto&& jewel : current_group)
                {
                  if (seen.find(jewel) == seen.end())
                  {
                    next.emplace(jewel);
                  }
                }
                seen.emplace(curr);
              } while (!next.empty());

              if (group.size() > 0)
                groups.emplace_back(group);
            }
          }
        
           
          auto increment = 0;
          for (auto&& group : groups) 
          {
            auto group_points = 0, points = 0;
            for (auto&& jewel : group) {
              if (!jewel->is_collapsing())
                collapse_jewel(jewel);
              points += points + 3;
            }
            group_points = points * group.size();
            increment += group_points;
          }
          if (increment)
          {
            for (auto j : hints)
              if (!(j->is_collapsing() || j->has_collapsed()))
                j->animation(jewel_animation_type::IDLE);
            hints.clear();
            score(increment);
          }
          /*
          else 
          {
            for (auto j : hints)
              if (!(j->is_collapsing() || j->has_collapsed()))
                j->animation(jewel_animation_type::GLOWING);
          }
          */
        }

public:
        grid(const std::string& spritesheet, const SDL_Rect& screen):
            sheet{ spritesheet }
          , screen{ screen }
          , map{}
          //, columns_with_free_slots{}
          , selected{2}
          , moving{}
          , collapsing{}
          , _score{0}
          , event_listeners {}
        {
          std::string fn{ std::string(__PRETTY_FUNCTION__) + ": " };
          assert(0 <= screen.x && screen.x <= 640);
          assert(0 <= screen.y && screen.y <= 480);
          assert(screen.h == JEWEL_WIDTH * 8);
          assert(screen.w == JEWEL_HEIGHT * 8);

          uint32_t subgroups_to_create = 5;

          std::cout << fn << "map was generated and resides at <" << screen.x << ", " << screen.y << ">."<< std::endl;
          random_generator::make_map(map, (uint32_t)5, screen.x, screen.y);

          yage::input::mouse& m = yage::input::mouse::instance();
        }

        virtual ~grid() {}

        operator SDL_Rect&()
        {
          return screen;
        }

        void on_frame()
        {

          std::string fn{ std::string(__PRETTY_FUNCTION__) + ": " };
          { // assertions, to be removed
            auto collapsing_it = collapsing.begin();
            while (collapsing_it != collapsing.end()) 
            {
              // a collapsing jewel did not, by definition, collapsed and is not moving
              assert( 
                  ((*collapsing_it)->is_collapsing() && !(*collapsing_it)->has_collapsed()) && 
                  ((*collapsing_it)->has_arrived() && !(*collapsing_it)->is_moving()));
              collapsing_it++;
            }
            auto moving_it = moving.begin();
            while (moving_it != moving.end())
            {
              // a moving jewel has, by definition, not arrived and cannot have collapsed nor being in process of collapsing
              assert( 
                ((*moving_it)->is_moving() && !(*moving_it)->has_arrived()) && 
                (!(*moving_it)->has_collapsed() && !(*moving_it)->is_collapsing()));
              moving_it++;
            }
          }


          {
            invariant();
            // iterate through the store, ticking and rendering every jewel
            for (int lin = 0; lin < LINES; lin++)
            {
              for (int col = 0; col < COLUMNS; col++) 
              {
                map[lin][col]->tick();
                map[lin][col]->on_frame();
              }
            }
            invariant();
          }

          {
            invariant();
            // iterate through the moving collection, removing those who reached their destination
            auto&& jewel_it = moving.begin();
            while (moving.end() != jewel_it)
            {
              jewel* j = *jewel_it;
              if (!j->has_arrived())
              {
                jewel_it++;
                continue;
              }
              auto coords = make_map_coordinates_from_jewel(j);
              assert(!j->has_arrived() || ((0 <= coords.first && coords.first <= COLUMNS) && (0 <= coords.second && coords.second <= LINES)));
              assert(j == map[coords.first][coords.second].get());
              j->vel(0);
              jewel_it = moving.erase(jewel_it);
            }

            invariant();
          }


          {
            invariant();
            // iterate through the collapsing collection, removing already collapsed
            auto&& jewel_it = collapsing.begin();
            while (jewel_it != collapsing.end())
            {
              
              if (!(*jewel_it)->has_collapsed())
              {
                jewel_it++;
                continue;
              }

              auto coords = make_map_coordinates_from_jewel((*jewel_it)); 
              //columns_with_free_slots[coords.first].push(coords.second);
              assert(coords.first >= 0 && coords.first <= COLUMNS);
              assert(coords.second >= 0 && coords.second <= LINES); 

              jewel_it = collapsing.erase(jewel_it);
            }
            invariant();
          }
          
          if (collapsing.size() == 0  && moving.size() == 0)
          {
            collapse_existing_groups();
            if (hints.empty())
            {
              for ( auto listener : event_listeners )
                listener->on_game_over();
            }
              
          }

          { 
            invariant();
            // assertions, to be removed
            auto collapsing_it = collapsing.begin();
            while (collapsing_it != collapsing.end()) 
            {
              // a collapsing jewel did not, by definition, collapsed and is not moving
              assert( 
                  ((*collapsing_it)->is_collapsing() && !(*collapsing_it)->has_collapsed()) && 
                  ((*collapsing_it)->has_arrived() && !(*collapsing_it)->is_moving()));
              collapsing_it++;
            }
            
            auto moving_it = moving.begin();
            while (moving_it != moving.end())
            {
           
              // a moving jewel has, by definition, not arrived and cannot have collapsed nor being in process of collapsing
              assert( 
                ((*moving_it)->is_moving() && !(*moving_it)->has_arrived()) && 
                (!(*moving_it)->has_collapsed() && !(*moving_it)->is_collapsing()));
              
              /*
              assert(
                ((*moving_it)->vel() == 1) 
                == 
                (((*moving_it)->dx() != (*moving_it)->x()) || ((*moving_it)->dy() != (*moving_it)->y()) 
              ));
*/
              //assert( (*moving_it)->vel() == 1 );
              moving_it++;
            }
            invariant();
          }
          
          {
            random_generator::generate_jewels_in_columns_with_empty_spaces(map, moving, screen.x, screen.y);
          }


          {
            // deal with the selected jewels, if any
            std::shared_ptr< jewel > first{}, second{};
            if ((first = selected[0].lock()) && (second = selected[1].lock()))
            {
              if ( first->has_arrived() && second->has_arrived() ) 
              {
                if ( !first->is_collapsing() && !second->is_collapsing() )  
                {
                  auto first_coords = make_map_coordinates_from_jewel(first.get());
                  auto second_coords = make_map_coordinates_from_jewel(second.get());

                  assert(first_coords.first >= 0 && first_coords.first <= COLUMNS);
                  assert(first_coords.first >= 0 && first_coords.first <= COLUMNS);
                  assert(second_coords.second >= 0 && second_coords.second <= LINES); 
                  assert(second_coords.second >= 0 && second_coords.second <= LINES); 
                  swap(first_coords,second_coords);
                  selected[0].reset();
                  selected[1].reset();
                }
                else
                {
                  selected[0].reset();
                  selected[1].reset();
                }
             }
          }
         /*   else if (selected[0].expired())
            {
              selected[0].reset(); 
              selected[1].reset();
            }
            */
          }
          
          


//        std::cout << "logical map\n";
        for (int lin = 0; lin < 8; lin++)
        {
          for (int col = 0; col < 8; col++)
          {
            if (map[col][lin]->is_collapsing())
            {
              assert( std::find(collapsing.begin(), collapsing.end(), map[col][lin].get() ) != collapsing.end() );
            }

            if (map[col][lin]->is_moving())
            {
              assert( std::find(moving.begin(), moving.end(), map[col][lin].get() ) != moving.end());
            }
//            std::cout << map[col][lin]->type() << "\t\t";

          }
//          std::cout << "\n";
        }
//        std::cout << std::endl;

        }

        void on_dragged(const SDL_MouseButtonEvent& b)
        {
          std::string fn{ std::string(__PRETTY_FUNCTION__) + ": " };
          std::cout << fn << "enter. ";
          if (moving.size() >= 0 || collapsing.size() >= 0)
          {
            std::cout << fn << "exit";
            return;
          }
          
          auto&& jewel_coordinates = make_map_coordinates_from_screen(b.x, b.y);
          if (!((0 <= jewel_coordinates.first && jewel_coordinates.first <= COLUMNS) && 
                (0 <= jewel_coordinates.second && jewel_coordinates.second <= LINES)))
          {
            std::cout << fn << "received a drag event from outside the grid.\n";
            return ;
          }
          auto column = jewel_coordinates.first;
          auto line = jewel_coordinates.second;
          std::cout << fn << "dragged <" << (int)column << ", " << (int)line << ">\n";

          if (selected[0].use_count() <= 0)
          {
            std::cout << fn << "use_count is " << selected[0].use_count()<< "\n";
            std::cout << fn << "setting first moving piece.\n";
            std::weak_ptr< jewel > first{ map[column][line] };
            selected[0].swap(first);
          }
          else if (map[column][line] == selected[0].lock())
          {
            std::cout << fn << "unsetting first moving piece.\n";
            selected[0].reset();
          }
          else
          {
            std::cout << fn << "setting second moving piece.\n";
            selected[1] = map[column][line];

            swap(selected[0].lock().get(), selected[1].lock().get());
          }
          std::cout << "exit." << std::endl;;
        }

        void on_clicked(const SDL_MouseButtonEvent& b)
        {
          std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
          std::cout << fn << "enter" << std::endl;
          yage::input::mouse& m = yage::input::mouse::instance();


          if (moving.size() > 0 || collapsing.size() > 0)
          {
            return;
          }

          auto&& jewel_coordinates = make_map_coordinates_from_screen(b.x, b.y);
          if (!((0 <= jewel_coordinates.first && jewel_coordinates.first <= COLUMNS) && 
                (0 <= jewel_coordinates.second && jewel_coordinates.second <= LINES)))
          {
            std::cout << fn << "received a click from outside the grid." << std::endl;
            return ;
          }
          auto column = jewel_coordinates.first;
          auto line = jewel_coordinates.second;
          std::cout << fn << "clicked on <" << (int)column << ", " << (int)line << ">\n";

          switch(b.type)
          {
            case SDL_MOUSEBUTTONDOWN:
              if (selected[0].use_count() <= 0)  
              {
                std::cout << fn << "setting first moving piece.";
                std::weak_ptr< jewel > first{ map[column][line] };
                selected[0].swap(first);
              }
              else if (map[column][line] == selected[0].lock())
              {
                std::cout << fn << "unsetting first moving piece.";
                selected[0].reset();
              }
              else  // selecting second
              {
                std::cout << fn << "setting second moving piece.";
                selected[1] = map[column][line];
              }
              break;
            case   SDL_MOUSEBUTTONUP:
              if (selected[0].use_count() > 0 && !(map[column][line] == selected[0].lock()) )
              {
                std::cout << fn << "setting second moving piece.";
                selected[1] = map[column][line];
              }

              break;
          }
          if (selected[0].lock() && selected[1].lock())  
            swap(selected[0].lock().get(), selected[1].lock().get());
/*
          if (selected[0].use_count() <= 0)
          {
            std::cout << fn << "use_count is " << selected[0].use_count()<< "\n";
            std::cout << fn << "setting first moving piece.";
            std::weak_ptr< jewel > first{ map[column][line] };
            selected[0].swap(first);
          }
          else if (map[column][line] == selected[0].lock())
          {
            std::cout << fn << "unsetting first moving piece.";
            selected[0].reset();
          }
          else
          {
            std::cout << fn << "setting second moving piece.";
            selected[1] = map[column][line];

            swap(selected[0].lock().get(), selected[1].lock().get());
          }
*/
          std::cout << std::endl;
        }

        void lose_focus()
        {
          std::string fn{ std::string(__PRETTY_FUNCTION__) + ": " };

          selected[0].reset();
          selected[1].reset();
        }
       
        /// Swaps two adjacent jewels,
        /// causes the two jewels to change places in logical map and acelerate towards the each others' location.
        void swap(const map_coordinates& f, const map_coordinates& s)
        {
          std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
          auto col_distance = std::max(f.first,s.first) - std::min(f.first,s.first);
          auto lin_distance = std::max(f.second,s.second) - std::min(f.second,s.second);
          if (col_distance + lin_distance > 1)
          {
            std::cout << fn << "too far away for a swap." << std::endl;
            return ;
          }
          auto fi = map[ f.first ][ f.second ];
          auto se = map[ s.first ][ s.second ];
          auto e = fi;

          map[ f.first ][ f.second ] = se;
          map[ s.first ][ s.second ] = fi;

          move_jewel(fi.get(), s);
          move_jewel(se.get(), f);

        }
        
        void swap(const uint8_t& f_col, const uint8_t& f_lin, const uint8_t& s_col, const uint8_t& s_lin)
        {
          std::string fn { std::string( __PRETTY_FUNCTION__ ) + ": "};
          if ((std::max(f_col,s_col) - std::min(f_col,f_col) > 1) ||
              (std::max(f_lin,s_lin) - std::min(f_lin,s_lin) > 1))
          {
            std::cout << fn << "too far away for a swap." << std::endl;
            return ;
          }
          auto first = make_map_coordinates_from_col_and_lin(f_col, f_lin);
          auto second = make_map_coordinates_from_col_and_lin(s_col, s_lin);

          assert(first.first >= 0 && first.first <= COLUMNS);
          assert(first.second >= 0 && first.second <= LINES); 
          
          
          assert(second.first >= 0 && second.first <= COLUMNS);
          assert(second.second >= 0 && second.second <= LINES); 
          
          swap(first, second);
        }

      
     
    
        void swap(jewel* first, jewel* second)
        {
          swap(make_map_coordinates_from_jewel(first), make_map_coordinates_from_jewel(second));
        }

        int score() const 
        {
          return _score;
        }

        void subscribe(grid_event_listener *listener)
        {
          assert(listener != nullptr);
          event_listeners.insert(event_listeners.end(),listener);
        }
        
        void unsubscribe(grid_event_listener *listener)
        {
          assert(listener != nullptr);
          event_listeners.remove(listener);
        }

        void on_hint()
        {
          if (collapsing.size() > 0 && moving.size() > 0)
            return ;
          collapse_existing_groups();
          for (auto j : hints)
            j->animation( jewel_animation_type::GLOWING );
        }

      };


    }
    using namespace interface1;
  }
}

#endif
