if(NOT CPPCHECK_TOOL_PATH)
    return()
endif()

add_custom_target(cppcheck
    COMMAND ${CPPCHECK_TOOL_PATH}
        # --force
        --enable=all
        # --inconclusive
        -i ${PROJECT_BINARY_DIR}/CMakeFiles
        -i ${PROJECT_SOURCE_DIR}/ThirdParty
        -i ${PROJECT_SOURCE_DIR}/Applications/DataExplorer
        -i ${PROJECT_SOURCE_DIR}/Tests
        ${PROJECT_SOURCE_DIR}
)
