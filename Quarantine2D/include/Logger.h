#pragma once

#define LOGF(x) //TODO: Log x to the log file

#ifdef _DEBUG
#include <iostream>
#define Q2D_LOGC(x) std::cout << x << std::endl
#else
#define Q2D_LOGC(x)
#endif