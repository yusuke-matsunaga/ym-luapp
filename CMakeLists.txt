# ===================================================================
# CMAKE のおまじない
# ===================================================================


# ===================================================================
# プロジェクト名，バージョンの設定
# ===================================================================


# ===================================================================
# パッケージの検査
# ===================================================================

ym_init_lua()
ym_init_readline ()


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${LUA_INCLUDE_DIR}
  )


# ===================================================================
#  ソースファイルの設定
# ===================================================================


# ===================================================================
#  ターゲットの設定
# ===================================================================
ym_add_object_library ( ym_luapp
  c++-srcs/Luapp.cc
  c++-srcs/LuaMt19937.cc
  )

if ( Readline_FOUND )
  foreach ( _obj obj obj_d obj_p )
    target_include_directories ( "ym_luapp_${_obj}"
      PRIVATE ${Readline_INCLUDE_DIR}
      )
    target_compile_definitions ( "ym_luapp_${_obj}"
      PRIVATE "-DHAS_READLINE"
      )
  endforeach ()
endif ( Readline_FOUND )

list ( APPEND YM_LIB_DEPENDS ${LUA_LIBRARIES} )
if ( Readline_FOUND )
  list ( APPEND YM_LIB_DEPENDS ${Readline_LIBRARY} )
endif ( Readline_FOUND )
set ( YM_LIB_DEPENDS ${YM_LIB_DEPENDS} PARENT_SCOPE )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( tests/programs )
