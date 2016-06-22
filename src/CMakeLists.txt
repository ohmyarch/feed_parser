if(UNIX)
  add_definitions(-DHAS_REMOTE_API=0)
endif()

add_library(feedparser ../feed/date_time/tz.cpp atom_parser.cc rss_parser.cc)

target_link_libraries(feedparser
  ${OPENSSL_LIBRARIES}
  ${CASABLANCA_LIBRARIES}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${Boost_ATOMIC_LIBRARY}
  ${Boost_CHRONO_LIBRARY}
  ${Boost_RANDOM_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
  )

if(WIN32)
  set_target_properties(feedparser PROPERTIES
    OUTPUT_NAME "${PROJECT_VERSION_MAJOR}_${PROJECT_VERSION_MINOR}_${PROJECT_VERSION_PATCH}")
else()
  set_target_properties(feedparser PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

  install(
    TARGETS feedparser
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
endif()
