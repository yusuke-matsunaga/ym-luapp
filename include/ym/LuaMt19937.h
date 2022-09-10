#ifndef LUAMT19937_H
#define LUAMT19937_H

/// @file ym/LuaMt19937.h
/// @brief LuaMt19937 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2020, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include <lua.hpp>
#include <random>
#include "ym_config.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class LuaMt19937 LuaMt19937.h "LuaMt19937.h"
/// @brief lua の mt19937 拡張
///
/// と言っても static メソッドしか持たない．
//////////////////////////////////////////////////////////////////////
class LuaMt19937
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief mt19937 関係の初期化を行う．
  ///
  /// parent モジュールに "new_mt19937" という生成関数を登録する．
  static
  void
  init(
    lua_State* L,      ///< [in] lua インタプリタ
    const char* parent ///< [in] 親のモジュール名
    = nullptr
  );

  /// @brief 対象が mt19937 の時に true を返す．
  static
  bool
  is_mt19937(
    lua_State* L, ///< [in] lua インタプリタ
    int idx       ///< [in] 対象のインデックス
  )
  {
    auto obj = to_mt19937(L, idx);
    return obj != nullptr;
  }

  /// @brief 対象を mt19937 に変換する．
  /// @return 変換結果を返す．
  ///
  /// 変換に失敗したら nullptr を返す．
  static
  std::mt19937*
  to_mt19937(
    lua_State* L, ///< [in] lua インタプリタ
    int idx       ///< [in] 対象のインデックス
  );

};

END_NAMESPACE_YM

#endif // LUAMT19937_H
