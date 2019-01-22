# Check requirements / supported configurations
if(MSVC AND NOT HAVE_64_BIT AND NOT OGS_32_BIT)
    message(FATAL_ERROR "Building OGS on Windows with 32-bit is not supported! \
Either use the correct generator, e.g. 'Visual Studio 14 2015 Win64' or define \
'-DOGS_32_BIT=ON' if you know what you are doing.")
endif()

# Set build directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
if(OGS_USE_CONAN AND MSVC)
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
endif()

set(Data_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Tests/Data CACHE INTERNAL "")
set(Data_BINARY_DIR ${PROJECT_BINARY_DIR}/Tests/Data CACHE INTERNAL "")

# Enable Visual Studio project folder grouping
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MACOSX_RPATH 1)

# Get version info from Git, implementation based on
# https://github.com/tomtom-international/cpp-dependencies
execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --long --dirty --always
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE DESCRIBE_RESULT
    OUTPUT_VARIABLE DESCRIBE_STDOUT
)
if(DESCRIBE_RESULT EQUAL 0)
    string(STRIP "${DESCRIBE_STDOUT}" DESCRIBE_STDOUT)
    message(STATUS "Git reported this project's version as '${DESCRIBE_STDOUT}'")
    if(DESCRIBE_STDOUT MATCHES "^(.*)-(dirty)$")
      set(DESCRIBE_DIRTY "${CMAKE_MATCH_2}")
      set(DESCRIBE_STDOUT "${CMAKE_MATCH_1}")
    endif()
    if(DESCRIBE_STDOUT MATCHES "^([0-9a-f]+)$")
      set(DESCRIBE_COMMIT_NAME "${CMAKE_MATCH_1}")
      set(DESCRIBE_STDOUT "")
    elseif(DESCRIBE_STDOUT MATCHES "^(.*)-g([0-9a-f]+)$")
      set(DESCRIBE_COMMIT_NAME "${CMAKE_MATCH_2}")
      set(DESCRIBE_STDOUT "${CMAKE_MATCH_1}")
    endif()
    if(DESCRIBE_STDOUT MATCHES "^(.*)-([0-9]+)$")
      set(DESCRIBE_COMMIT_COUNT "${CMAKE_MATCH_2}")
      set(DESCRIBE_TAG "${CMAKE_MATCH_1}")
      set(DESCRIBE_STDOUT "")
    endif()
    if("${DESCRIBE_TAG}.0.0" MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+).*$")
      set(CPACK_PACKAGE_VERSION_MAJOR "${CMAKE_MATCH_1}")
      set(CPACK_PACKAGE_VERSION_MINOR "${CMAKE_MATCH_2}")
      set(CPACK_PACKAGE_VERSION_PATCH "${CMAKE_MATCH_3}")
    endif()

    set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
    if(DESCRIBE_COMMIT_COUNT GREATER 0)
      set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}-${DESCRIBE_COMMIT_COUNT}")
    endif()

    set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}-${DESCRIBE_COMMIT_NAME}")

    if(DESCRIBE_DIRTY)
      string(TIMESTAMP DESCRIBE_DIRTY_TIMESTAMP "%Y%m%d%H%M%S" UTC)
      set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}.dirty.${DESCRIBE_DIRTY_TIMESTAMP}")
    endif()
    set(OGS_VERSION ${CPACK_PACKAGE_VERSION})
    message(STATUS "OGS VERSION: ${CPACK_PACKAGE_VERSION}")
else()
    message(WARNING "Git repository contains no tags! Please run: git fetch --tags")
endif()
