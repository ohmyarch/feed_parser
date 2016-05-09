add_executable(parse_podcast main.cc)

target_link_libraries(parse_podcast
  ${Boost_LIBRARIES}
  ${PugiXML_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${FEED_PARSER_LIBRARY}
  ${CASABLANCA_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)
