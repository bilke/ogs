APPEND_SOURCE_FILES(SOURCES)

add_library(ComponentTransport ${SOURCES})
install(TARGETS ComponentTransport LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_link_libraries(ComponentTransport
    PUBLIC ProcessLib
)

include(Tests.cmake)
