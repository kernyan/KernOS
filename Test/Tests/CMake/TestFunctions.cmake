function(add_utest targetName)
  add_executable(${targetName} ${ARGN})
  target_link_libraries(${targetName}
    PRIVATE
      gtest
    )
  add_test(NAME ${targetName}
    COMMAND ${targetName}
    )
endfunction()
