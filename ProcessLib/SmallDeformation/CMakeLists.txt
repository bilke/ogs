APPEND_SOURCE_FILES(SOURCES)

add_library(SmallDeformation ${SOURCES})
install(TARGETS SmallDeformation LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(SmallDeformation PUBLIC ProcessLib)

include(Tests.cmake)
