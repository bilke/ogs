APPEND_SOURCE_FILES(SOURCES)

add_library(ThermoMechanicalPhaseField ${SOURCES})
install(TARGETS ThermoMechanicalPhaseField LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(ThermoMechanicalPhaseField PUBLIC ProcessLib)

include(Tests.cmake)
