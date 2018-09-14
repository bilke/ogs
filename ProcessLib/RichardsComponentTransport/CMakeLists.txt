APPEND_SOURCE_FILES(SOURCES)

add_library(RichardsComponentTransport ${SOURCES})
install(TARGETS RichardsComponentTransport LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(RichardsComponentTransport PUBLIC ProcessLib)

include(Tests.cmake)
