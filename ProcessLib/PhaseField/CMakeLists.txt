APPEND_SOURCE_FILES(SOURCES)

add_library(PhaseField ${SOURCES})
install(TARGETS PhaseField LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(PhaseField PUBLIC ProcessLib)

include(Tests.cmake)
