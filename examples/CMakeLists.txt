add_executable(rss_parser main.cc)

target_link_libraries(rss_parser
  ${Boost_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${FEED_PARSER_LIBRARY}
  ${CASABLANCA_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)
