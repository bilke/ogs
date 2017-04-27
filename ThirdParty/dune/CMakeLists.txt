message(STATUS "dune-target created. Build it first!")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/dunecontrol.cfg.in
    ${CMAKE_CURRENT_BINARY_DIR}/dunecontrol.cfg
)

add_custom_target(dune
    ./dune-common/bin/dunecontrol
    --opts=${CMAKE_CURRENT_BINARY_DIR}/dunecontrol.cfg
    --builddir=${CMAKE_CURRENT_BINARY_DIR} all
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
