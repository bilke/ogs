APPEND_SOURCE_FILES(SOURCES)

add_library(RichardsMechanics ${SOURCES})
install(TARGETS RichardsMechanics LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(RichardsMechanics PUBLIC ProcessLib RichardsFlow)

include(Tests.cmake)
