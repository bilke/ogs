if(NOT CPPDEPENDENCIES_TOOL_PATH)
    return()
endif()

add_custom_target(cppdeps
    COMMAND ${CPPDEPENDENCIES_TOOL_PATH}
        --stats
        --graph dependencies.dot
        --dir ${PROJECT_SOURCE_DIR}
)

if(NOT DOT_TOOL_PATH)
    return()
endif()

add_custom_command(TARGET cppdeps POST_BUILD
    COMMAND ${DOT_TOOL_PATH} -Tpng dependencies.dot > dependencies.png
)
