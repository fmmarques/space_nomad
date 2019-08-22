#include <bejeweled.hpp>


int main(int argc, const char *argv[])
{
  bejeweled::engine& bejeweled_engine = bejeweled::engine::instance();
  bejeweled_engine.run();
  return 0;  
}
