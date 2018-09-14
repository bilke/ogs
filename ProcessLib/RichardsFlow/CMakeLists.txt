APPEND_SOURCE_FILES(SOURCES)

add_library(RichardsFlow ${SOURCES})
install(TARGETS RichardsFlow LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(RichardsFlow PUBLIC ProcessLib)

include(Tests.cmake)
