function(add_utest targetName)
  add_executable(${targetName} ${ARGN})
  target_link_libraries(${targetName}
    PRIVATE
      gtest
      MOD_kernel
    )
  add_test(NAME ${targetName}
    COMMAND ${targetName}
    )
endfunction()
