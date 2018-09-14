APPEND_SOURCE_FILES(SOURCES)

add_library(HeatConduction ${SOURCES})
install(TARGETS HeatConduction LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(HeatConduction PUBLIC ProcessLib)

include(Tests.cmake)
