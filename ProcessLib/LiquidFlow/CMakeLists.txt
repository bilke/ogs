APPEND_SOURCE_FILES(SOURCES)

add_library(LiquidFlow ${SOURCES})
install(TARGETS LiquidFlow LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(LiquidFlow PUBLIC ProcessLib)

include(Tests.cmake)
