#pragma once

#include <iostream>
#include <iomanip>

#define DEBUG 1

#if DEBUG
#  define COUT(x) std::cout << x;
#  define CERR(x) std::cerr << x;
#else
#  define COUT(x)
#  define CERR(x)
#endif