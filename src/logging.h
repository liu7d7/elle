#pragma once

#include <iostream>
#include <iomanip>

#define DEBUG 0

#if DEBUG
#  define COUT(x) std::cout << x;
#  define CERR(x) std::cerr << x;
#else
#  define COUT(x)
#  define CERR(x)
#endif