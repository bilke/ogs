APPEND_SOURCE_FILES(SOURCES)

add_library(HT ${SOURCES})
install(TARGETS HT LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(HT PUBLIC ProcessLib)

include(Tests.cmake)
