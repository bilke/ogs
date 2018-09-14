APPEND_SOURCE_FILES(SOURCES)

add_library(ThermalTwoPhaseFlowWithPP ${SOURCES})
install(TARGETS ThermalTwoPhaseFlowWithPP LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(ThermalTwoPhaseFlowWithPP PUBLIC ProcessLib)

include(Tests.cmake)
