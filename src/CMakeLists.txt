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
    TARGETS feedparser EXPORT feed_parser-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
    )

  if(FEED_PARSER_INSTALL_HEADERS)
    file(GLOB FEED_PARSER_HEADERS_FEED ../feed/*.h)
    install(
      FILES ${FEED_PARSER_HEADERS_FEED}
      DESTINATION include/feed
      COMPONENT Devel
    )
    file(GLOB FEED_PARSER_HEADERS_DATE_TIME ../feed/date_time/tz.h ../feed/date_time/date.h ../feed/date_time/tz_private.h)
    install(
      FILES ${FEED_PARSER_HEADERS_DATE_TIME}
      DESTINATION include/feed/date_time
      COMPONENT Devel
    )

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
      "${CMAKE_CURRENT_BINARY_DIR}/feed_parser/feed_parser-config-version.cmake"
      COMPATIBILITY AnyNewerVersion
    )

    export(EXPORT feed_parser-targets
      FILE "${CMAKE_CURRENT_BINARY_DIR}/feed_parser/feed_parser-targets.cmake"
    )

    set(ConfigPackageLocation lib/cmake/feed_parser)
    configure_package_config_file(../feed_parser-config.cmake.in
      ${CMAKE_CURRENT_BINARY_DIR}/feed_parser/feed_parser-config.cmake
      INSTALL_DESTINATION ${ConfigPackageLocation}
      NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )

    install(EXPORT feed_parser-targets
      FILE
        feed_parser-targets.cmake
      DESTINATION
        ${ConfigPackageLocation}
    )
    install(
      FILES
        "${CMAKE_CURRENT_BINARY_DIR}/feed_parser/feed_parser-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/feed_parser/feed_parser-config-version.cmake"
      DESTINATION
        ${ConfigPackageLocation}
      COMPONENT
        Devel
    )

    export(PACKAGE feed_parser)
  endif()
endif()
