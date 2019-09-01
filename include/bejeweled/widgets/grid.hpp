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
#include <bejeweled/widgets/jewel.hpp>

namespace bejeweled {
	namespace widgets {
		namespace interface1 {


			template < typename map_generator_t > class grid;
			
			template < unsigned int LINES, unsigned int COLUMNS, unsigned int JEWEL_WIDTH, unsigned int JEWEL_HEIGHT >
			class random_generator
			{
			private:
				random_generator();

      protected:
        static void fill_column(
            std::shared_ptr<jewel> map[8][8],
            std::list<  jewel * >& moving,
            int column,
            int x,
            int y
        ){
          int falling_jewels = 0;
          int collapsed_jewels = 0;
          for (auto line = 0; line < 8; line++)
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

//              if (squashed_jewel <= 0)
//              {
                //j = nullptr; //std::make_shared< widgets::jewel >( make_jewel(x + column*JEWEL_WIDTH, y - JEWEL_HEIGHT)  );
//                continue;
//              }
//              else
//              {
                j = map[column][current_falling_jewel];
//              }

//              if (j)
//              {
                assert(!j->is_collapsing() && !j->has_collapsed());
                j->dy(squashed_jewel * JEWEL_HEIGHT + y);
                j->vel(4);
                map[column][squashed_jewel] = j;
                moving.insert(moving.end(), j.get());
//              }
              
              //assert(j->dy() != j->y());
              
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
//            std::cout << fn << "created a new jewel at <" << j->x() << "," << j->y() << "> directed towards <" << j->dx() << ", " << dy <<">." << std::endl;
            moving.insert(moving.end(), j.get());
          }

      }
      public:
				static jewel make_jewel(int x, int y)
				{
          std::string fn { std::string(__PRETTY_FUNCTION__) + ": " };
//          std::cout << fn << "enter. Arguments: { .x=" << x << ", .y=" << y << "}. ";

					SDL_Rect screen
					{
						.x = static_cast<decltype(SDL_Rect::x)>(x),
						.y = static_cast<decltype(SDL_Rect::y)>(y),
						.w = static_cast<decltype(SDL_Rect::w)>(JEWEL_WIDTH),
						.h = static_cast<decltype(SDL_Rect::h)>(JEWEL_HEIGHT)
					};

					jewel_type type = jewel_type::YELLOW;

          auto itype = rand() % static_cast< unsigned int >(jewel_type::JEWEL_TYPE_COUNT);
          type = static_cast<jewel_type>(itype);
//          std::cout << "created a jewel at <" << screen.x << ", " << screen.y << ">" << std::endl;

          return jewel(type, "assets/gems.spritesheet.transparent.png", screen, 0);

        }

      static void make_map(std::shared_ptr< jewel > map[LINES][COLUMNS], unsigned int subgroups, unsigned int x, unsigned int y)
      {
        srand(time(nullptr));
        assert(0 <= x && x + JEWEL_WIDTH * COLUMNS <= 640);
        assert(0 <= y && y + JEWEL_HEIGHT * LINES  <= 480);

        for (auto col = 0; col < COLUMNS; col++) {
          for (auto lin = 0; lin < LINES; lin++) {
            map[col][lin] = std::make_shared<jewel>(make_jewel(x+col*JEWEL_WIDTH, y+lin*JEWEL_HEIGHT));
          }
        }
      }

      static void generate_jewels_in_columns_with_empty_spaces(
          std::shared_ptr< jewel > map[8][8],
          std::list<  jewel * >& moving,
          int x,
          int y
      ) {
        std::string fn ( std::string( __PRETTY_FUNCTION__) + std::string(": "));
        //std::cout << fn << "enter. ";

        for (auto&& col = 0; col < 8; col++) 
        {
/*
            if (columns_with_free_slots[col].size() == 0)
              continue;
            std::cout << "column " << col << " is marked with " << columns_with_free_slots[col].size() << " empty spaces. ";
 */
          fill_column(map, moving, col, x, y);
        }
        //std::cout << "exit." << std::endl;
      }
    };

    template < typename map_generator_t >
    class grid
    {
    public:
      static const uint32_t LINES = 8, COLUMNS = 8, SPACES = LINES * COLUMNS, JEWEL_WIDTH = 32, JEWEL_HEIGHT = 32;
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
          
          assert(col >= 0 && col <= 7 && lin >= 0 && lin <= 7);

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
          assert(0 <= map_destination.first && map_destination.first <= 7);
          assert(0 <= map_destination.second && map_destination.second <= 7);

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
         // Returns a group from the grid coordinate given
         // @arg col the column
         // @arg lin the lin
         // @return a vector with the jewels to collapse
         // @note this is an auxiliary method to determine which jewels should collapse
				std::vector< jewel * > make_collapsing_group_from(int col, int lin)
				{
					std::array < jewel *, 8 > tmp;
					std::vector < jewel * > result{};
          assert(col < 8 &&  lin < 8);
					if (map[col][lin]->has_collapsed() || map[col][lin]->is_collapsing() || map[col][lin]->is_moving())
						return result;


					int pos = 0;
					for ( int coln = col - 1;
                coln >= 0 && 
                !map[coln][lin]->has_collapsed() && !map[coln][lin]->is_collapsing() && !map[coln][lin]->is_moving() && 
                map[coln][lin]->type() == map[col][lin]->type();
						    coln--, pos++)
          {
						tmp[pos] = map[coln][lin].get();
          }

					for ( int coln = col + 1;
						    coln < 8 && 
                !map[coln][lin]->has_collapsed() && !map[coln][lin]->is_collapsing() && !map[coln][lin]->is_moving() && 
                map[coln][lin]->type() == map[col][lin]->type();
						    coln++, pos++) 
          {
						tmp[pos] = map[coln][lin].get();
          }

					if (pos + 1 >= 3)
						result.insert(result.begin(), tmp.begin(), tmp.begin() + pos);

					pos = 0;
					for ( int linn = lin - 1;
						    linn >= 0 && 
                !map[col][linn]->has_collapsed() && !map[col][linn]->is_collapsing() && !map[col][linn]->is_moving() && 
                map[col][linn]->type() == map[col][lin]->type();
						    linn--, pos++)
          {
					  tmp[pos] = (map[col][linn]).get();
          }

					for ( int linn = lin + 1;
						    linn < 8 && 
                !map[col][linn]->has_collapsed() && !map[col][linn]->is_collapsing() && !map[col][linn]->has_arrived() && 
                map[col][linn]->type() == map[col][lin]->type();
						    linn++, pos++)
          {
						tmp[pos] = (map[col][linn]).get();
          }

					if (pos + 1 >= 3)
						result.insert(result.begin(), tmp.begin(), tmp.begin() + pos);

					if (result.size() >= 2)
						result.insert(result.begin(), map[col][lin].get());

          { // invariants assert
            for ( auto&& j : result )
            {
              assert(!j->is_collapsing());
              assert(!j->is_moving());
              assert(j->has_arrived());
              assert(!j->has_collapsed());
            }
          }
					return result;
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
                assert(curr_coords.first >= 0 && curr_coords.first <= 7);
                assert(curr_coords.second >= 0 && curr_coords.second <= 7); 
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
            score(increment);
          }
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
					map_generator_t::make_map(map, (uint32_t)5, screen.x, screen.y);

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
              assert(!j->has_arrived() || ((0 <= coords.first && coords.first <= 7) && (0 <= coords.second && coords.second <= 7)));
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
              assert(coords.first >= 0 && coords.first <= 7);
              assert(coords.second >= 0 && coords.second <= 7); 

							jewel_it = collapsing.erase(jewel_it);
						}
            invariant();
					}
          
          if (collapsing.size() == 0  && moving.size() == 0)
          {
            collapse_existing_groups();
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
						map_generator_t::generate_jewels_in_columns_with_empty_spaces(map, moving, screen.x, screen.y);
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

                  assert(first_coords.first >= 0 && first_coords.first <= 7);
                  assert(first_coords.first >= 0 && first_coords.first <= 7);
                  assert(second_coords.second >= 0 && second_coords.second <= 7); 
                  assert(second_coords.second >= 0 && second_coords.second <= 7); 
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
          if (!((0 <= jewel_coordinates.first && jewel_coordinates.first <= 7) && 
                (0 <= jewel_coordinates.second && jewel_coordinates.second <= 7)))
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
          if (!((0 <= jewel_coordinates.first && jewel_coordinates.first <= 7) && 
                (0 <= jewel_coordinates.second && jewel_coordinates.second <= 7)))
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

          assert(first.first >= 0 && first.first <= 7);
          assert(first.second >= 0 && first.second <= 7); 
          
          
          assert(second.first >= 0 && second.first <= 7);
          assert(second.second >= 0 && second.second <= 7); 
          
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

			};


		}
		using namespace interface1;
	}
}

#endif
