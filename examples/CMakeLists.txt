add_executable(parser_rss parser_rss.cc)
add_executable(parser_atom parser_atom.cc)

set(FEED_PARSER_LIBRARIES
  ${Boost_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${FEED_PARSER_LIBRARY}
  ${CASABLANCA_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries(parser_rss ${FEED_PARSER_LIBRARIES})
target_link_libraries(parser_atom ${FEED_PARSER_LIBRARIES})
