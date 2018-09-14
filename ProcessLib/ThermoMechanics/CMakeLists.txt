APPEND_SOURCE_FILES(SOURCES)

add_library(ThermoMechanics ${SOURCES})
install(TARGETS ThermoMechanics LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(ThermoMechanics PUBLIC ProcessLib)

include(Tests.cmake)
