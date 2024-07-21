set(SKITE_SOURCE_FILES ${SKITE_SOURCE_FILES}
    basic-code-cursor.cc
    lexer.cc
    main.cc
    token.cc
)

add_executable(skitec ${SKITE_SOURCE_FILES})
target_include_directories(skitec PUBLIC ${SKITE_INCLUDE_DIRS})
