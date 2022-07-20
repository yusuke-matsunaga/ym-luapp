
/// @file LuaMt19937.cc
/// @brief LuaMt19937 に関する実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2020, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/LuaMt19937.h"
#include "ym/Luapp.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// mt13997 用のシグネチャ
const char* MT19937_SIGNATURE{"Luapp.mt19937"};

// mt19937 を作る．
int
mt19937_new(
  lua_State* L
)
{
  Luapp lua{L};

  // メモリ領域は Lua で確保する．
  void* p = lua.new_userdata(sizeof(std::mt19937));

  int n = lua.get_top();
  if ( n == 1 ) {
    // 引数なしの場合
    new (p) std::mt19937;
  }
  else {
    return lua.error_end("Error: new_mt19937() expects no arguments.");
  }

  // mt19937 の metatable を取ってくる．
  luaL_getmetatable(lua.lua_state(), MT19937_SIGNATURE);

  // それを今作った userdata にセットする．
  lua.set_metatable(-2);

  return 1;
}

// デストラクタ
int
mt19937_gc(
  lua_State* L
)
{
  auto obj = LuaMt19937::to_mt19937(L, 1);
  if ( obj ) {
    obj->~mersenne_twister_engine();
  }

  // メモリは Lua が解放する．
  return 0;
}

// mt19937::seed()
int
mt19937_seed(
  lua_State* L
)
{
  Luapp lua{L};

  // このメソッドは mt19937 と一つの整数を引数に取る(オプション)．
  int n = lua.get_top();
  if ( n > 2 ) {
    return lua.error_end("Error: mt19937:seed() expects at most one argument.");
  }

  auto obj = LuaMt19937::to_mt19937(L, 1);
  ASSERT_COND ( obj != nullptr );

  if ( n == 1 ) {
    obj->seed();
  }
  else { // n == 2
    bool ret;
    int val;
    tie(ret, val) = lua.to_integer(2);
    if ( !ret ) {
      return lua.error_end("Error: mt19937::seed(): Arg#2 should be an integer.");
    }
    obj->seed(val);
  }

  lua.push_boolean(true);
  return 1;
}

// mt19937::eval()
int
mt19937_eval(
  lua_State* L
)
{
  Luapp lua{L};

  // このメソッドは mt19937 を引数として受け取る．
  int n = lua.get_top();
  if ( n != 1 ) {
    return lua.error_end("Error: mt19937:eval() expects no arguments.");
  }

  auto obj = LuaMt19937::to_mt19937(L, 1);
  ASSERT_COND( obj != nullptr );

  auto val = obj->operator()();

  lua.push_integer(val);
  return 1;
}

END_NONAMESPACE

void
LuaMt19937::init(
  lua_State* L,
  vector<struct luaL_Reg>& mylib
)
{
  static const struct luaL_Reg mt [] = {
    {"seed", mt19937_seed},
    {"eval", mt19937_eval},
    {nullptr, nullptr}
  };

  Luapp lua{L};
  lua.reg_metatable(MT19937_SIGNATURE, mt19937_gc, mt);

  // 生成関数を登録する．
  mylib.push_back({"new_mt19937", mt19937_new});
}

// @brief 対象を mt19937 に変換する．
// @param[in] idx 対象のインデックス
//
// 変換に失敗したら nullptr を返す．
std::mt19937*
LuaMt19937::to_mt19937(
  lua_State* L,
  int idx
)
{
  Luapp lua{L};
  auto p = lua.L_checkudata(idx, MT19937_SIGNATURE);
  return reinterpret_cast<std::mt19937*>(p);
}

END_NAMESPACE_YM
