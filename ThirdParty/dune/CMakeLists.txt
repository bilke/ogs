
message("Going to make DUNE:
    ${CMAKE_CURRENT_SOURCE_DIR}/dune-common/bin/dunecontrol
    --builddir=${CMAKE_CURRENT_BUILD_DIR} all")
add_custom_target(Dune
    ${CMAKE_CURRENT_SOURCE_DIR}/dune-common/bin/dunecontrol --builddir=${CMAKE_CURRENT_BUILD_DIR} all
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
