
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${LUA_INCLUDE_DIR}
  )


# ===================================================================
#  ソースファイルの設定
# ===================================================================
set ( my_lua_SOURCES
  my_lua.cc
  )


# ===================================================================
#  release ターゲットの設定
# ===================================================================
add_executable ( my_lua
  ${my_lua_SOURCES}
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_compile_options ( my_lua
  PRIVATE "-O3"
  )

target_compile_definitions ( my_lua
  PRIVATE "-DNDEBUG"
  )

if ( Readline_FOUND )
  target_compile_definitions ( my_lua
    PRIVATE "-DHAS_READLINE"
    )
endif ()

target_link_libraries ( my_lua
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( my_lua
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


# ===================================================================
#  release with debug info ターゲットの設定
# ===================================================================
add_executable ( my_lua_p
  ${my_lua_SOURCES}
  ${YM_SUBMODULE_OBJ_P_LIST}
  )

target_compile_options ( my_lua_p
  PRIVATE "-O2" "-g"
  )

target_compile_definitions ( my_lua_p
  PRIVATE "-DNDEBUG"
  )

if ( Readline_FOUND )
  target_compile_definitions ( my_lua_p
    PRIVATE "-DHAS_READLINE"
    )
endif ()

target_link_libraries ( my_lua_p
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( my_lua_p
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


# ===================================================================
#  debug ターゲットの設定
# ===================================================================
add_executable (my_lua_d
  ${my_lua_SOURCES}
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_options ( my_lua_d
  PRIVATE "-g"
  )

if ( Readline_FOUND )
  target_compile_definitions ( my_lua_d
    PRIVATE "-DHAS_READLINE"
    )
endif ()

target_link_libraries ( my_lua_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( my_lua_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


# ===================================================================
#  インストールターゲットの設定
# ===================================================================

install (
  TARGETS my_lua my_lua_p my_lua_d
  DESTINATION bin
  )
