#include <yage/graphics/graphics_manager.hpp>
#include <yage/engine/game_engine.hpp>

#include <bejeweled.hpp>


int main(int argc, const char *argv[])
{
  using namespace yage::graphics;
  
  window game_window("bejeweled", 640, 480);
  graphics_manager::instance().set_window( game_window );
}
