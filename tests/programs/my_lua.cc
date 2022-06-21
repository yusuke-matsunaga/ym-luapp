
/// @file my_lua.cc
/// @brief my_lua の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include <unistd.h> // for getopt()
#include <libgen.h> // for basename()
#include <stdio.h>
#ifdef HAS_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif
#include "ym/Luapp.h"


BEGIN_NONAMESPACE

void
usage(
  char** argv
)
{
  using namespace std;

  cerr << "Usage: " << basename(argv[0])
       << " <script-file>" << endl;
}

END_NONAMESPACE


int
main(
  int argc,
  char** argv
)
{
  using namespace std;
  using namespace YM_NAMESPACE;

  Luapp lua;

  lua.L_openlibs();

  if ( argc == 1 ) {
    const char* prompt{"% "};
    for ( ; ; ) {
#ifdef HAS_READLINE
      auto line_read = readline(prompt);
      if ( line_read == nullptr ) {
	// EOF
	break;
      }
      if ( line_read[0] ) {
	add_history(line_read);
      }
      string linebuf{line_read};
      free(line_read);
#else
      cerr << prompt;
      cerr.flush();
      string linebuf;
      if ( !getline(cin, linebuf) ) {
	break;
      }
#endif
      int err = lua.L_loadstring(linebuf.c_str()) || lua.pcall(0, 0, 0);
      if ( err ) {
	cerr << lua.to_string(-1) << endl;
	lua.pop(1);
      }
    }
  }
  else if ( argc >= 2 ) {
    // 引数を lua のグローバル変数 "arg" にセットする．
    lua.create_table(argc, 0);
    for ( int i = 0; i < argc; ++ i ) {
      lua.push_string(argv[i]);
      lua.set_table(-2, i);
    }
    lua.set_global("arg");
    // 先頭の引数をスクリプトファイルとみなして実行する．
    string init_file = argv[1];
    if ( init_file != string{} && lua.L_dofile(init_file.c_str()) ) {
      cerr << lua.to_string(-1) << endl;
      return 1;
    }
  }
  else {
    usage(argv);
    return 1;
  }

  return 0;
}
