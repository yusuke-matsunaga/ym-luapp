はじめに
=========

`ym-luapp` は Lua の C-API を C++ で使いやすくしたラッパクラスである．
通常の `lua_XXX` 形式の関数および大部分の `luaL_XXX` 形式の関数をサポー
トしている．一部の `luaL_XXX` 関数は実際にはマクロとして実装されており，
そのままでは C++ のメンバ関数化するのが難しいため除外されている．

ym-luapp を用いることで，容易に C++ のクラスを Lua の拡張型として使えるように
することができる．


ym-luapp の組み込み方
----------------------

`ym-luapp` のソースレポジトリは
git@github.com:yusuke-matsunaga/ym-luapp.git である．
ただし， `ym-luapp` は他のプログラムのサブモジュールとして使用されるこ
とを仮定しているので，通常はメインのプロジェクトに Git のサブモジュー
ルとして追加して用いる．
また， `ym-common` が同様にサブモジュールとして追加されているものと仮
定している．

`ym-luapp` はビルドシステムとして `CMake` を用いるので， `ym-luapp`
を用いるためには `CMakeLists.txt` にその設定を記述する必要がある．
具体的にはプロジェクト直下の `CMakeLists.txt` に以下の記述が必要となる．


::

   list ( APPEND CMAKE_MODULE_PATH
   "${PROJECT_SOURCE_DIR}/ym-common/cmake"
   )

   include (YmUtils)

   ym_init_lua ()


   ym_init_submodules (
     .
     .
     ym-luapp
     .
     .
   )
