APPEND_SOURCE_FILES(SOURCES)

add_library(HydroMechanics ${SOURCES})
install(TARGETS HydroMechanics LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(HydroMechanics PUBLIC ProcessLib)

include(Tests.cmake)
