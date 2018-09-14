APPEND_SOURCE_FILES(SOURCES)

add_library(TwoPhaseFlowWithPP ${SOURCES})
install(TARGETS TwoPhaseFlowWithPP LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(TwoPhaseFlowWithPP PUBLIC ProcessLib)

include(Tests.cmake)
