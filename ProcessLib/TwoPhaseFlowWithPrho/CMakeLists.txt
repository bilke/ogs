APPEND_SOURCE_FILES(SOURCES)

add_library(TwoPhaseFlowWithPrho ${SOURCES})
install(TARGETS TwoPhaseFlowWithPrho LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
target_link_libraries(TwoPhaseFlowWithPrho PUBLIC ProcessLib)

include(Tests.cmake)
