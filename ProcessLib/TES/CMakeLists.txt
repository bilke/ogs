APPEND_SOURCE_FILES(SOURCES)

add_library(TES ${SOURCES})
install(TARGETS TES LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(TES PUBLIC ProcessLib)

include(Tests.cmake)
