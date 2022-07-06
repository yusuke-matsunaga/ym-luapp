
/// @file my_lua.cc
/// @brief Luapp を用いた簡単なスタンドアロンインタプリタ
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Luapp.h"


int
main(
  int argc,
  char** argv
)
{
  YM_NAMESPACE::Luapp lua;

  lua.L_openlibs();

  return lua.main_loop(argc, argv);
}
