#if !defined(BEJEWELED_WIDGETS_GRID_GENERATORS_HPP)
#  define BEJEWELED_WIDGETS_GRID_GENERATORS_HPP
#include <cassert>
#include <list>
#include <bejeweled/widgets/jewel.hpp>

namespace bejeweled {
namespace widgets {
namespace interface1 {

class gameover_generator
{
public:
  static const int COLUMNS = 8, LINES = 8;
  static const int JEWEL_WIDTH = 32, JEWEL_HEIGHT = 32;
private:
  static void fill_column( std::shared_ptr<jewel> map[COLUMNS][LINES], std::list<  jewel * >& moving, int column, int x, int y);
  
public:
  gameover_generator();
  static jewel make_jewel(int x, int y);

      static void make_map(std::shared_ptr< jewel > map[COLUMNS][LINES], unsigned int subgroups, unsigned int x, unsigned int y)
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

        for (auto&& col = 0; col < 8; col++) 
        {
          fill_column(map, moving, col, x, y);
        }
      }
};


class random_generator
{
public:
  static const int COLUMNS = 8, LINES = 8;
  static const int JEWEL_WIDTH = 32, JEWEL_HEIGHT = 32;
private:
  static void fill_column( std::shared_ptr<jewel> map[COLUMNS][LINES], std::list<  jewel * >& moving, int column, int x, int y);
  
public:
  random_generator();
  static jewel make_jewel(int x, int y);

      static void make_map(std::shared_ptr< jewel > map[COLUMNS][LINES], unsigned int subgroups, unsigned int x, unsigned int y)
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

        for (auto&& col = 0; col < 8; col++) 
        {
          fill_column(map, moving, col, x, y);
        }
      }
};


}
}
}
#endif

