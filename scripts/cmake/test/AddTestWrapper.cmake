# IMPORTANT: multiple arguments in one variables have to be in list notation (;)
# and have to be quoted when passed "-DEXECUTABLE_ARGS=${AddTest_EXECUTABLE_ARGS}"
foreach(FILE ${FILES_TO_DELETE})
    file(REMOVE ${BINARY_PATH}/${FILE})
endforeach()

# convert list to space delimited string
set(CMD "${WRAPPER_COMMAND}")
foreach(A ${WRAPPER_ARGS})
    set(CMD "${CMD} ${A}")
endforeach()

set(CMD "${CMD} ${EXECUTABLE}")
foreach(A ${EXECUTABLE_ARGS})
    set(CMD "${CMD} ${A}")
endforeach()
string(STRIP "${CMD}" CMD)

# Create Python virtual environment and install packages
if(EXISTS ${SOURCE_PATH}/requirements.txt AND NOT EXISTS ${BINARY_PATH}/.venv)
    message(STATUS "Generating Python virtual environment...")
    execute_process(
        COMMAND virtualenv .venv
        WORKING_DIRECTORY ${BINARY_PATH}
    )
    set(PIP_EXE .venv/bin/pip)
    if(WIN32)
        set(PIP_EXE .venv/Scripts/pip.exe)
    endif()
    execute_process(
        COMMAND ${PIP_EXE} install -r ${SOURCE_PATH}/requirements.txt
        WORKING_DIRECTORY ${BINARY_PATH}
    )
endif()

message(STATUS "running command generating test results:\ncd ${SOURCE_PATH} && ${CMD} >${STDOUT_FILE_PATH}")
execute_process(
    COMMAND ${WRAPPER_COMMAND} ${WRAPPER_ARGS} ${EXECUTABLE} ${EXECUTABLE_ARGS}
    WORKING_DIRECTORY ${SOURCE_PATH}
    RESULT_VARIABLE EXIT_CODE
    # OUTPUT_FILE ${STDOUT_FILE_PATH} # must be used exclusively
    OUTPUT_VARIABLE OUTPUT
    ERROR_VARIABLE OUTPUT
)

if(NOT EXIT_CODE STREQUAL "0")
    message(FATAL_ERROR "Test wrapper exited with code: ${EXIT_CODE}\n${OUTPUT}")
endif()
