#include <chrono>
#include <iostream>
#include <thread>

#include "monitor.h"

int main(int /*argc*/, char const* /*argv*/[])
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  int i = 100;
  std::cout << "Started" << std::endl;
  while (i-- > 0) {
    std::cout << "put" << std::endl;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    Monitor::setStatus(207, 1, 1);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  std::cout << "End" << std::endl;

  return 0;
}
