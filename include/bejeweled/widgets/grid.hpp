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
			
      template < unsigned int LINES, unsigned int COLUMNS, unsigned int JEWEL_WIDTH, unsigned int JEWEL_HEIGTH >
			class random_generator
			{
			private:
				random_generator();

				static jewel make_jewel(int col, int lin, int map_x_pos, int map_y_pos)
				{
          std::string fn { std::string(__PRETTY_FUNCTION__) + ": " };
          std::cout << fn << "enter ";

					SDL_Rect screen
					{
						.x = static_cast<decltype(SDL_Rect::x)>(map_x_pos + col * JEWEL_WIDTH),
						.y = static_cast<decltype(SDL_Rect::y)>(map_y_pos + lin * JEWEL_HEIGTH),
						.w = static_cast<decltype(SDL_Rect::w)>(JEWEL_WIDTH),
						.h = static_cast<decltype(SDL_Rect::h)>(JEWEL_HEIGTH)
					};

					jewel_type type = jewel_type::YELLOW;

          auto itype = rand() % static_cast< unsigned int >(jewel_type::JEWEL_TYPE_COUNT);
          type = static_cast<jewel_type>(itype);
          std::cout << "created a jewel at <" << screen.x << ", " << screen.y << " at <" << col << ", " << lin << "> " << std::endl;

        return jewel(type, "assets/gems.spritesheet.transparent.png", screen, 0, col, lin, map_x_pos, map_y_pos);

      }
    public:

      static void generate_map(std::shared_ptr< jewel > map[LINES][COLUMNS], unsigned int subgroups, unsigned int x, unsigned int y)
      {
        srand(time(nullptr));
        assert(0 <= x && x + JEWEL_WIDTH * COLUMNS <= 640);
        assert(0 <= y && y + JEWEL_HEIGTH * LINES  <= 480);

        for (auto&& lin = 0; lin < LINES; lin++)
          for (auto&& col = 0; col < COLUMNS; col++)
            map[col][lin] = std::make_shared<jewel>(make_jewel(col, lin, x, y));
      }

      static void generate_jewels_in_columns_with_empty_spaces(
          std::shared_ptr< jewel > map[8][8], 
          std::list<  jewel * >& moving, 
          std::array< std::queue < int >, 8 >& columns_with_free_slots,
          int map_x_coor,
          int map_y_coor)
      // TODO: modify the jewel to be sure that it'll be rendered falling towards it's cell and not just appearing there.
      //       remove the subtraction of the map_y as its a weak replacement.
      {
        std::string fn ( std::string( __PRETTY_FUNCTION__) + std::string(": "));
        std::cout << fn << "enter. ";
        for (auto&& col = 0; col < 8; col++) 
        {
          if (columns_with_free_slots[col].size() == 0)
            continue;
          std::cout << "column " << col << " is marked with " << columns_with_free_slots[col].size() << " empty spaces. ";
          
          auto column = columns_with_free_slots[col];
          while (column.size() > 0)
          {
            auto&& lin = column.front() - 1;
            column.pop();
          
            for ( ; lin >= 0; lin-- ) 
            {
              std::cout << "moving the piece at <" << col << ", " << lin <<"> one below. ";
              *map[col][lin+1] = *map[col][lin];
            }
          
            assert(lin == -1);
          }

          column = columns_with_free_slots[col];
          while (column.size() > 0)
          {
            auto&& lin = column.front() - 1;
            column.pop();

            for ( ; lin >= 0; lin --)
            {
              std::cout << "filling <" << col << ", " << lin << "> with a new piece. ";
              *map[col][lin] = make_jewel(col, lin, map_x_coor, map_y_coor);
              //moving.insert(moving.end(), map[col][lin].get());
            }
          }       
          //assert(columns_with_free_slots[col].size() == 0);
          
        }
      }

    };

    template < typename map_generator_t >
    class grid
    {
    public:
      static const uint32_t LINES = 8, COLUMNS = 8, SPACES = LINES * COLUMNS, JEWEL_WIDTH = 32, JEWEL_HEIGHT = 32;
    private:
      yage::graphics::spritesheet< jewel_type, jewel_animation_type > sheet;
      SDL_Rect screen;

      /// the logical map
      std::shared_ptr< jewel > map[COLUMNS][LINES];

      /// the counter of free slot per column
      std::array< std::queue< int >, COLUMNS > columns_with_free_slots;

      /// the jewels in motion
      std::list<  jewel * > moving;

      /// the jewels collapsing
      std::list<  jewel * > collapsing;

      /// the jewels selected by the user;
      std::array< jewel *, 2 > selected;

      void invariant()
      {
        bool no_jewel_in_collapsing_is_in_store = true;
        bool no_jewel_in_moving_is_in_store = true;
        bool every_renderable_jewel_is_within_map = true;

        for (auto&& collapsing_jewel : collapsing)
        {
          every_renderable_jewel_is_within_map &= screen.x <= collapsing_jewel->x() && screen.x + screen.w >= collapsing_jewel->x() && screen.y <= collapsing_jewel->y() && screen.y + screen.h >= collapsing_jewel->y();
        }

        for (auto&& moving_jewel : moving)
        {
          every_renderable_jewel_is_within_map &= screen.x <= moving_jewel->x() && screen.x + screen.w >= moving_jewel->x() && screen.y <= moving_jewel->y() && screen.y + screen.h >= moving_jewel->y();
					}

					assert(no_jewel_in_collapsing_is_in_store);
					assert(no_jewel_in_moving_is_in_store);
					assert(every_renderable_jewel_is_within_map);
				}
			protected:
				/// moves a jewel
				/// causes a jewel to gain velocity towards its destination and to be added to the moving
				void move_jewel(jewel* j, int col, int lin)
				{
					assert(j != nullptr);
					assert(j->map_x() != col || j->map_y() != lin);
          invariant();

					j->map_x(col);
					j->map_y(lin);

         // j->animation(jewel_animation_type::IDLE);

					j->vel(1);

					moving.insert(moving.end(), j);

					invariant();
				}

				/// 
				/// causes a jewel to use the collapsing animation and to be added to the collapsing 
				void collapse_jewel(jewel* j)
				{
					assert(j != nullptr);
					invariant();

					j->animation(jewel_animation_type::COLLAPSING);
					collapsing.insert(collapsing.end(), j);

					invariant();
				}

        void swap(jewel* first, jewel* second)
        {
          swap(first->map_x(), first->map_y(), second->map_x(), second->map_y());
        }
				/// Swaps two adjacent jewels,
				/// causes the two jewels to change places in logical map and acelerate towards the each others' location.
				void swap(int f_col, int f_lin, int s_col, int s_lin)
				{
					std::string fn{ std::string("\n") + std::string(__PRETTY_FUNCTION__) + ": " };
					using bejeweled::widgets::jewel;

					std::cout << fn << "enter.";
					if (std::abs(f_col - s_col) > 1 || std::abs(f_lin - s_lin) > 1)
						std::cout << fn << "warning: swapping <" << f_col << ", " << f_lin << "> to <" << s_col << ", " << s_lin << ">, that are more than 1 place distance.";

					jewel e( jewel_type::YELLOW, "assets/gems.spritesheet.transparent.png", SDL_Rect{ .x = 0, .y = 0, .w = 0, .h = 0 }, 0, 0, 0, 0, 0);
					e = *map[f_col][f_lin];
					*map[f_col][f_lin] = *map[s_col][s_lin];
					*map[s_col][s_lin] = e;

					auto x = map[f_col][f_lin]->map_x();
					auto y = map[f_col][f_lin]->map_y();
					move_jewel(map[f_col][f_lin].get(), map[s_col][s_lin]->map_x(), map[s_col][s_lin]->map_y());
					move_jewel(map[s_col][s_lin].get(), x, y);

					


					std::cout << fn << "swapped <" << f_col << ", " << f_lin << "> to <" << s_col << ", " << s_lin << ">." << std::endl;
					std::cout << fn << "exit" << std::endl;

				}


				/// Returns a group from <x, y> 
				std::vector< jewel * > make_collapsing_group_from(int x, int y)
				{
					std::array < jewel *, 8 > tmp;
					std::vector < jewel * > result{};

					if (map[x][y]->has_collapsed() || !map[x][y]->has_arrived())
						return std::vector< jewel * >{};


					int pos = 0;
					for (int xn = x - 1;
						xn >= 0 && !(map[xn][y]->has_collapsed()) && (map[xn][y]->has_arrived()) && map[xn][y]->type() == map[x][y]->type();
						xn--, pos++)
						tmp[pos] = map[xn][y].get();

					for (int xn = x + 1;
						xn < 8 && !(map[xn][y]->has_collapsed()) && (map[xn][y]->has_arrived()) && map[xn][y]->type() == map[x][y]->type();
						xn++, pos++)
						tmp[pos] = map[xn][y].get();

					if (pos + 1 >= 3)
						result.insert(result.begin(), tmp.begin(), tmp.begin() + pos);

					pos = 0;
					for (int yn = y - 1;
						yn >= 0 && !(map[x][yn]->has_collapsed()) && (map[x][yn]->has_arrived()) && map[x][yn]->type() == map[x][y]->type();
						yn--, pos++)
						tmp[pos] = (map[x][yn]).get();

					for (int yn = y + 1;
						yn < 8 && !(map[x][yn]->has_collapsed()) && (map[x][yn]->has_arrived()) && map[x][yn]->type() == map[x][y]->type();
						yn++, pos++)
						tmp[pos] = (map[x][yn]).get();

					if (pos + 1 >= 3)
						result.insert(result.begin(), tmp.begin(), tmp.begin() + pos);

					if (result.size() >= 2)
						result.insert(result.begin(), map[x][y].get());

					std::for_each(std::begin(result), std::end(result), [](jewel * j) { j->animation(jewel_animation_type::COLLAPSING); });

						return result;
				}

				/// Adds all the existing groups to collapse.
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

								current_group = make_collapsing_group_from(curr->map_x(), curr->map_y());
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
					
          for (auto&& group : groups) {
            for (auto&& jewel : group) {
              jewel->animation(jewel_animation_type::COLLAPSING);
              collapsing.insert(collapsing.end(), jewel);
              
            }
          }

				}


			public:
				grid(const std::string& spritesheet,
					const SDL_Rect& screen) :
					sheet{ spritesheet }
					, screen{ screen }
					, map{}
					, columns_with_free_slots{}
					, selected{ nullptr, nullptr }
					, moving{}
					, collapsing{}
				{
					std::string fn{ std::string(__PRETTY_FUNCTION__) + ": " };
					std::cout << fn << "enter";
					assert(0 <= screen.x && screen.x <= 640);
					assert(0 <= screen.y && screen.y <= 480);
					assert(screen.h == JEWEL_WIDTH * 8);
					assert(screen.w == JEWEL_HEIGHT * 8);

					uint32_t subgroups_to_create = 5;

					map_generator_t::generate_map(map, (uint32_t)5, screen.x, screen.y);

					collapse_existing_groups();

					std::cout << fn << "exit";
				}

				virtual ~grid() {}

				operator SDL_Rect&()
				{
					return screen;
				}

				void on_frame()
				{
					std::string fn{ std::string(__PRETTY_FUNCTION__) + ": " };

					{
						map_generator_t::generate_jewels_in_columns_with_empty_spaces(map, moving, columns_with_free_slots, screen.x, screen.y);
					}

					{
						// iterate through the store, ticking and rendering every jewel
						for (int lin = 0; lin < LINES; lin++)
							for (int col = 0; col < COLUMNS; col++) 
              {
                map[lin][col]->tick();
								map[lin][col]->on_frame();
              }
					}

					{
						// deal with the selected jewels, if any
						if (selected[0] && selected[1])
						{
							if ( std::find(moving.begin(), moving.end(), selected[0]) == moving.end() &&
								 std::find(moving.begin(), moving.end(), selected[1]) == moving.end() )
							{
								swap(selected[0]->map_x(), selected[0]->map_y(), selected[1]->map_x(), selected[1]->map_y());
							}
						}
					}

					{
						// iterate through the moving collection, removing those who reached their destination
						auto&& jewel_it = moving.begin();
						while (moving.end() != jewel_it)
						{
							if (!(*jewel_it)->has_arrived())
							{
								jewel_it++;
								continue;
							}

							auto&& group = make_collapsing_group_from((*jewel_it)->map_x(), (*jewel_it)->map_y());
							std::for_each(std::begin(group), std::end(group), [this](jewel *it){ this->collapse_jewel(it); });

							int selected_index = -1, pair_selected_index = -1;
							if (selected[0] && selected[1]) 
              {
	              selected_index = (*jewel_it == selected[0]) ? 0 : 1;
							  pair_selected_index = (*jewel_it == selected[0]) ? 1 : 0;

							  auto&& pair_group = make_collapsing_group_from(selected[pair_selected_index]->map_x(), selected[pair_selected_index]->map_y());
							  std::for_each(std::begin(group), std::end(group), [&](jewel *it){ this->collapse_jewel(it); });

							  if (group.size() == 0 && pair_group.size() == 0)
							  {
								  swap(selected[pair_selected_index], selected[selected_index]);
								  selected[0] = selected[1] = nullptr;
							  }

              }
							jewel_it = moving.erase(jewel_it);
						}
					}

					{
						// iterate through the collapsing collection, removing already collapsed
						auto&& jewel_it = collapsing.begin();
						while (jewel_it != collapsing.end())
						{
							if (!(*jewel_it)->has_collapsed())
							{
								jewel_it++;
								continue;
							}
               
              auto col = (*jewel_it)->map_x();
							for (int lin = (*jewel_it)->map_y(); lin > 0; lin--)
							{
								map[col][lin] = map[col][lin - 1];
								move_jewel(map[col][lin].get(), col, lin);
							}
							columns_with_free_slots[col].push( (*jewel_it)->map_y() );

							jewel_it = collapsing.erase(jewel_it);
						}
					}
				}

				void on_dragged(const SDL_MouseButtonEvent& b)
				{
					std::string fn{ std::string(__PRETTY_FUNCTION__) + ": " };
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
					std::string fn{ std::string("\n") + std::string(__PRETTY_FUNCTION__) + ": " };
					std::cout << fn << "enter";

					if (moving.size() >= 0 || collapsing.size() >= 0)
					{
						std::cout << fn << "exit";
						return;

					}

					auto jewel_column = (b.x - screen.x) / JEWEL_WIDTH;
					auto jewel_line = (b.y - screen.y) / JEWEL_HEIGHT;
					std::cout << fn << "clicked on <" << jewel_column << ", " << jewel_line << ">\n";

					if (selected[0] == nullptr)
					{
						std::cout << fn << "setting first moving piece.";
						selected[1] = map[jewel_column][jewel_line].get();
					}
					else
					{
						std::cout << fn << "setting second moving piece.";
						selected[1] = map[jewel_column][jewel_line].get();
					}
					std::cout << fn << "exit" << std::endl;
				}

				void lose_focus()
				{
					std::string fn{ std::string(__PRETTY_FUNCTION__) + ": " };
					std::cout << fn << "enter";

					std::cout << fn << "exit";

				}
			};


		}
		using namespace interface1;
	}
}

#endif
