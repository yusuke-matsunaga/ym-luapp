
/// @file Luapp.cc
/// @brief Luapp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Luapp.h"


BEGIN_NAMESPACE_LUAPP

BEGIN_NONAMESPACE

struct luaL_Reg* table = nullptr;
SizeType table_size = 0;

int
luaopen_mylib(
  lua_State* L
)
{
  // luaL_newlib とほぼ同様のコード
  // ただし sizeof(table) が使えないので table_size を使う．
  luaL_checkversion(L);
  lua_createtable(L, 0, table_size);
  luaL_setfuncs(L, table, 0);
  return 1;
}

END_NONAMESPACE

// @brief モジュールを登録する関数
void
Luapp::reg_module(
  const char* name,
  const vector<struct luaL_Reg>& mylib,
  bool auto_require
)
{

  // C API には struct luaL_reg の配列が必要
  table_size = mylib.size();
  table = new luaL_Reg[table_size + 1];
  for ( SizeType i = 0; i < table_size; ++ i ) {
   table[i] = mylib[i];
  }
  // end-marker
  table[table_size] = {NULL, NULL};

  // lua 上で
  // package.prelad[name] = luaopen_mylib
  // と実行するのと同様
  L_requiref("_G", luaopen_base, 1);
  L_requiref("package", luaopen_package, 1);
  pop(2);

  get_global("package");
  get_field(-1, "preload");
  push_cfunction(luaopen_mylib);
  set_field(-2, name);
  pop(2);

  if ( auto_require ) {
    // 自動的に require "name" を実行しておく
    L_requiref(name, luaopen_mylib, 1);
    pop(1);
  }
}

END_NAMESPACE_LUAPP
