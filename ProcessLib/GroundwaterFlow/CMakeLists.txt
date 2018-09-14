APPEND_SOURCE_FILES(SOURCES)

add_library(GroundwaterFlow ${SOURCES})
install(TARGETS GroundwaterFlow LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_link_libraries(GroundwaterFlow
    PUBLIC ProcessLib
)

include(Tests.cmake)
