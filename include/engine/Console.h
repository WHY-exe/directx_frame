#pragma once
#include "InitWin.h"
#include <iostream>
#ifdef _DEBUG
#define ADD_CONSOLE()                                                          \
  AllocConsole();                                                              \
  FILE *ConsoleStream;                                                         \
  freopen_s(&ConsoleStream, "CONOUT$", "w", stdout)
#define FREE_CONSOLE()                                                         \
  system("pause");                                                             \
  FreeConsole();                                                               \
  fclose(stdout)
#else
#define ADD_CONSOLE()
#endif // !DEBUG
