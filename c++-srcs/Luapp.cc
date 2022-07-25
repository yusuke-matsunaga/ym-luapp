
/// @file Luapp.cc
/// @brief Luapp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Luapp.h"
#include <libgen.h> // for dirname()
#ifdef HAS_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif


BEGIN_NAMESPACE_LUAPP

BEGIN_NONAMESPACE

bool
get_line(
  const string& prompt,
  string& linebuf
)
{
#ifdef HAS_READLINE
  auto line_read = readline(prompt.c_str());
  if ( line_read == nullptr ) {
    // EOF
    return false;
  }
  if ( line_read[0] ) {
    add_history(line_read);
  }
  linebuf = string{line_read};
  free(line_read);
  return true;
#else
  cerr << prompt;
  cerr.flush();
  if ( getline(cin, linebuf) ) {
    return true;
  }
  else {
    return false;
  }
#endif
}

END_NONAMESPACE


// @brief インタプリタのメイン処理を行う．
int
Luapp::main_loop(
  int argc,   ///< [in] コマンドラインの引数の数
  char** argv ///< [in] コマンドラインの引数の配列
)
{
  if ( argc == 1 ) {
    // インタラクティブモード
    const char* prompt{"% "};
    for ( ; ; ) {
      string linebuf;
      if ( !get_line(prompt, linebuf) ) {
	break;
      }
      int err = L_loadstring(linebuf.c_str()) || pcall(0, 0, 0);
      if ( err ) {
	cerr << to_string(-1) << endl;
	pop(1);
      }
    }
  }
  else if ( argc >= 2 ) {
    // 引数を lua のグローバル変数 "arg" にセットする．
    create_table(argc, 0);
    for ( int i = 0; i < argc; ++ i ) {
      push_string(argv[i]);
      set_table(-2, i);
    }
    set_global("arg");
    // 先頭の引数をスクリプトファイルとみなして実行する．
    char* init_file = argv[1];
    string script{argv[1]};
    // スクリプトファイルの場所を変数 "script_dir" にセットする．
    char* dir_name = dirname(init_file);
    push_string(dir_name);
    set_global("script_dir");
    if ( script != string{} && L_dofile(script.c_str()) ) {
      cerr << to_string(-1) << endl;
      return 1;
    }
  }
  return 0;
}

// @brief 指定したフィールドを整数に変換する．
Luapp::RetType
Luapp::get_int_field(
  int index,
  const char* key,
  lua_Integer& val
)
{
  RetType ret = ERROR;
  int type = get_field(index, key);
  if ( type == LUA_TNIL ) {
    // 存在しなかった．
    ret = NOT_FOUND;
  }
  else {
    bool isnum;
    tie(isnum, val) = to_integer(-1);
    if ( isnum ) {
      ret = OK;
    }
    else {
      // 異なる型だった．
      ret = ERROR;
      cerr << "Error: '" << key << "' should be an integer." << endl;
    }
  }

  // get_field() で積まれた要素を捨てる．
  pop(1);

  return ret;
}

// @brief 指定したフィールドをブール値に変換する．
Luapp::RetType
Luapp::get_boolean_field(
  int index,
  const char* key,
  bool& val
)
{
  RetType ret = ERROR;
  int type = get_field(index, key);
  if ( type == LUA_TNIL ) {
    // 存在しなかった．
    ret = NOT_FOUND;
  }
  else {
    // 全ての型は boolean に変換可能
    val = to_boolean(-1);
    ret = OK;
  }

  // get_field() で積まれた要素を捨てる．
  pop(1);

  return ret;
}

// @brief 指定したフィールドを浮動小数点に変換する．
Luapp::RetType
Luapp::get_float_field(
  int index,
  const char* key,
  lua_Number& val
)
{
  RetType ret = ERROR;
  int type = get_field(index, key);
  if ( type == LUA_TNIL ) {
    // 存在しなかった．
    ret = NOT_FOUND;
  }
  else {
    bool isnum;
    tie(isnum, val) = to_number(-1);
    if ( isnum ) {
      ret = OK;
    }
    else {
      // 異なる型だった．
      ret = ERROR;
      cerr << "Error: '" << key << "' should be a float number." << endl;
    }
  }

  // get_field() で積まれた要素を捨てる．
  pop(1);

  return ret;
}

// @brief 指定したフィールドを文字列に変換する．
Luapp::RetType
Luapp::get_string_field(
  int index,
  const char* key,
  string& val
)
{
  RetType ret = ERROR;
  int type = get_field(index, key);
  if ( type == LUA_TNIL ) {
    // 存在しなかった．
    ret = NOT_FOUND;
  }
  else {
    if ( type == LUA_TSTRING ) {
      val = string(to_string(-1));
      ret = OK;
    }
    else {
      ret = ERROR;
      cerr << "Error: '" << key << "' should be a string." << endl;
    }
  }

  // get_field() で積まれた要素を捨てる．
  pop(1);

  return ret;
}

// @brief 指定したフィールドを vector<int> に変換する．
Luapp::RetType
Luapp::get_int_list_field(
  int index,
  const char* key,
  vector<int>& val
)
{
  RetType ret = ERROR;
  int type = get_field(index, key);
  val.clear();
  if ( type == LUA_TNIL ) {
    // 存在しなかった．
    ret = NOT_FOUND;
  }
  else if ( type == LUA_TTABLE ) {
    int n = L_len(-1);
    ret = OK;
    for ( int i = 1; i <= n; ++ i ) {
      get_table(-1, i);
      bool isnum;
      int val1;
      tie(isnum, val1) = to_integer(-1);
      if ( isnum ) {
	val.push_back(val1);
      }
      else {
	cerr << "Error: illegal data in table '" << key << "'" << endl;
	ret = ERROR;
      }
      // get_table() で積んだ値を捨てる．
      pop(1);
    }
  }
  else {
    // エラー
    cerr << "Error: illegal type in '" << key << "'" << endl;
  }

  // get_field() で積まれた要素を捨てる．
  pop(1);

  return ret;
}

// @brief 指定したフィールドを vector<string> に変換する．
Luapp::RetType
Luapp::get_string_list_field(
  int index,
  const char* key,
  vector<string>& val
)
{
  RetType ret = ERROR;
  int type = get_field(index, key);
  val.clear();
  if ( type == LUA_TNIL ) {
    // 存在しなかった．
    ret = NOT_FOUND;
  }
  else if ( type == LUA_TTABLE ) {
    int n = L_len(-1);
    ret = OK;
    for ( int i = 1; i <= n; ++ i ) {
      get_table(-1, i);
      string val1 = to_string(-1);
      val.push_back(val1);
      // get_table() で積んだ値を捨てる．
      pop(1);
    }
  }
  else {
    // エラー
    ret = ERROR;
    cerr << "Error: illegal type in '" << key << "'" << endl;
  }

  // get_field() で積まれた要素を捨てる．
  pop(1);

  return ret;
}

// @brief テーブルのキーのリストを作る．
vector<string>
Luapp::get_table_keys(
  int index
)
{
  vector<string> key_list;
  push_nil();
  while ( next(index) != 0 ) {
    key_list.push_back(to_string(-2));
    pop(1);
  }
  return key_list;
}

// @brief 指定したフィールドを整数に変換してセッター関数を呼ぶ．
Luapp::RetType
Luapp::get_int_field(
  int index,
  const char* key,
  std::function<bool(int)> setter
)
{
  RetType ret = ERROR;
  int type = get_field(index, key);
  if ( type == LUA_TNIL ) {
    // 存在しなかった．
    ret = NOT_FOUND;
  }
  else {
    bool isnum;
    lua_Integer tmp;
    tie(isnum, tmp) = to_integer(-1);
    if ( isnum ) {
      if ( setter(tmp) ) {
	ret = OK;
      }
    }
    else {
      // 異なる型だった．
      cerr << "Error: '" << key << "' should be an integer." << endl;
    }
  }

  // get_field() で積まれた要素を捨てる．
  pop(1);

  return ret;
}

// @brief 指定したフィールドをブール値に変換してセッター関数を呼ぶ．
Luapp::RetType
Luapp::get_boolean_field(
  int index,
  const char* key,
  std::function<bool(bool)> setter
)
{
  RetType ret = ERROR;
  int type = get_field(index, key);
  if ( type == LUA_TNIL ) {
    // 存在しなかった．
    ret = NOT_FOUND;
  }
  else {
    // 全ての型は boolean に変換可能
    if ( setter(to_boolean(-1)) ) {
      ret = OK;
    }
  }

  // get_field() で積まれた要素を捨てる．
  pop(1);

  return ret;
}

// @brief 指定したフィールドを文字列に変換してセッター関数を呼ぶ．
Luapp::RetType
Luapp::get_string_field(
  int index,
  const char* key,
  std::function<bool(const string&)> setter
)
{
  RetType ret = ERROR;
  int type = get_field(index, key);
  if ( type == LUA_TNIL ) {
    // 存在しなかった．
    ret = NOT_FOUND;
  }
  else if ( type == LUA_TSTRING ) {
    if ( setter(string(to_string(-1))) ) {
      ret = OK;
    }
  }
  else {
    cerr << "Error: '" << key << "' should be a string." << endl;
  }

  // get_field() で積まれた要素を捨てる．
  pop(1);

  return ret;
}

// @brief 指定したフィールドを vector<int> に変換してセッター関数を呼ぶ．
Luapp::RetType
Luapp::get_int_list_field(
  int index,
  const char* key,
  std::function<bool(const vector<int>&)> setter
)
{
  RetType ret = ERROR;
  int type = get_field(index, key);
  if ( type == LUA_TNIL ) {
    // 存在しなかった．
    ret = NOT_FOUND;
  }
  else if ( type == LUA_TTABLE ) {
    int n = L_len(-1);
    vector<int> val;
    val.reserve(n);
    bool break_flag = false;
    for ( int i = 1; i <= n; ++ i ) {
      get_table(-1, i);
      bool isnum;
      int val1;
      tie(isnum, val1) = to_integer(-1);
      if ( isnum ) {
	val.push_back(val1);
      }
      else {
	cerr << "Error: illegal data in table '" << key << "'" << endl;
	break_flag = true;
	break;
      }
      // get_table() で積んだ値を捨てる．
      pop(1);
    }
    if ( !break_flag && setter(val) ) {
      ret = OK;
    }
  }
  else {
    // エラー
    cerr << "Error: illegal type in '" << key << "'" << endl;
  }

  // get_field() で積まれた要素を捨てる．
  pop(1);

  return ret;
}

// @brief 指定したフィールドを vector<string> に変換してセッター関数を呼ぶ．
Luapp::RetType
Luapp::get_string_list_field(
  int index,
  const char* key,
  std::function<bool(const vector<string>&)> setter
)
{
  RetType ret = ERROR;
  int type = get_field(index, key);
  if ( type == LUA_TNIL ) {
    // 存在しなかった．
    ret = NOT_FOUND;
  }
  else if ( type == LUA_TTABLE ) {
    int n = L_len(-1);
    vector<string> val;
    val.reserve(n);
    for ( int i = 1; i <= n; ++ i ) {
      get_table(-1, i);
      string val1 = to_string(-1);
      val.push_back(val1);
      // get_table() で積んだ値を捨てる．
      pop(1);
    }
    if ( setter(val) ) {
      ret = OK;
    }
  }
  else {
    // エラー
    cerr << "Error: illegal type in '" << key << "'" << endl;
  }

  // get_field() で積まれた要素を捨てる．
  pop(1);

  return ret;
}

// @brief lua 上のクラスを定義するためのメタテーブルを登録する関数
void
Luapp::reg_metatable(
  const char* signature,
  lua_CFunction gc_func,
  const luaL_Reg* func_table
)
{
  // metatable を作る．
  L_newmetatable(signature);

  // metatable 自身を __index に登録する．
  push_value(-1);
  set_field(-2, "__index");

  // デストラクタを __gc に登録する．
  push_cfunction(gc_func);
  set_field(-2, "__gc");

  // 関数テーブルを登録する．
  L_setfuncs(func_table, 0);
}

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
