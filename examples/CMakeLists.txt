add_executable(rss_parser rss_parser.cc)
add_executable(atom_parser atom_parser.cc)

set(FEED_PARSER_LIBRARIES
  ${Boost_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${FEED_PARSER_LIBRARY}
  ${CASABLANCA_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries(rss_parser ${FEED_PARSER_LIBRARIES})
target_link_libraries(atom_parser ${FEED_PARSER_LIBRARIES})
