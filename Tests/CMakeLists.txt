include(${PROJECT_SOURCE_DIR}/scripts/cmake/OGSEnabledElements.cmake)

# VS2012 doesn't support correctly the tuples yet
# See http://code.google.com/p/googletest/issues/detail?id=412
if(MSVC)
    add_definitions(/D_VARIADIC_MAX=10)
endif()

APPEND_SOURCE_FILES(TEST_SOURCES)
APPEND_SOURCE_FILES(TEST_SOURCES BaseLib)
APPEND_SOURCE_FILES(TEST_SOURCES FileIO)
APPEND_SOURCE_FILES(TEST_SOURCES GeoLib)
APPEND_SOURCE_FILES(TEST_SOURCES GeoLib/IO)
APPEND_SOURCE_FILES(TEST_SOURCES MaterialLib)
APPEND_SOURCE_FILES(TEST_SOURCES MathLib)
APPEND_SOURCE_FILES(TEST_SOURCES MeshLib)
APPEND_SOURCE_FILES(TEST_SOURCES MeshGeoToolsLib)
APPEND_SOURCE_FILES(TEST_SOURCES NumLib)
APPEND_SOURCE_FILES(TEST_SOURCES ParameterLib)
APPEND_SOURCE_FILES(TEST_SOURCES ProcessLib)

if(OGS_BUILD_GUI)
    APPEND_SOURCE_FILES(TEST_SOURCES FileIO_Qt)
endif()

if(OGS_BUILD_SWMM)
    APPEND_SOURCE_FILES(TEST_SOURCES FileIO_SWMM)
endif()

if(OGS_BUILD_PROCESS_LIE)
    APPEND_SOURCE_FILES(TEST_SOURCES ProcessLib/LIE)
endif()

if(OGS_USE_PETSC)
    list(REMOVE_ITEM TEST_SOURCES NumLib/TestSerialLinearSolver.cpp)
endif()

add_executable(testrunner ${TEST_SOURCES})
set_target_properties(testrunner PROPERTIES FOLDER Testing)

target_link_libraries(testrunner
    ApplicationsFileIO
    gtest
    MeshGeoToolsLib
    MaterialLib
    MeshLib
    NumLib
    ParameterLib
    ProcessLib
    Threads::Threads
    ${VTK_LIBRARIES}
)

if(OGS_BUILD_PROCESS_LIE)
    target_link_libraries(testrunner LIE)
endif()

if(OGS_USE_PETSC)
    target_link_libraries(testrunner ${PETSC_LIBRARIES})
endif()

if(OGS_USE_MPI)
    target_link_libraries(testrunner MPI::MPI_CXX)
endif()

if(OGS_BUILD_SWMM)
    target_link_libraries(testrunner SwmmInterface)
endif()

if(OGS_INSITU)
    target_link_libraries(testrunner InSituLib)
endif()

if(OGS_BUILD_GUI)
    target_link_libraries(testrunner Qt5::Core Qt5::Gui Qt5::Xml Qt5::Network VtkVis QtDataView)
    if(GEOTIFF_FOUND)
        target_link_libraries(testrunner ${GEOTIFF_LIBRARIES})
    endif()
endif()

if(OGS_USE_PCH)
    cotire(testrunner)
endif()

# Add make-target tests which runs the testrunner
if(IS_CI AND NOT OGS_COVERAGE)
    set(TESTRUNNER_ADDITIONAL_ARGUMENTS ${TESTRUNNER_ADDITIONAL_ARGUMENTS}
        --gtest_shuffle --gtest_repeat=3)
endif()
set(TESTRUNNER_ADDITIONAL_ARGUMENTS ${TESTRUNNER_ADDITIONAL_ARGUMENTS}
    -l warn
    --gtest_output=xml:./testrunner.xml)

add_custom_target(tests-cleanup ${CMAKE_COMMAND} -E remove -f testrunner.xml)
set_target_properties(tests-cleanup PROPERTIES FOLDER Testing)

if(OGS_USE_PETSC)
    if("${HOSTNAME}" MATCHES "frontend.*")
        set(MPIRUN_ARGS --mca btl_openib_allow_ib 1)
    endif()
    set(TEST_FILTER_MPI --gtest_filter=-MPITest*)
    add_custom_target(tests
        mpirun ${MPIRUN_ARGS} -np 1 $<TARGET_FILE:testrunner> ${TESTRUNNER_ADDITIONAL_ARGUMENTS} ${TEST_FILTER_MPI}
        DEPENDS testrunner tests-cleanup
    )
    add_custom_target(tests_mpi
        mpirun ${MPIRUN_ARGS} -np 3 $<TARGET_FILE:testrunner> --gtest_filter=MPITest*
        DEPENDS testrunner
    )
    set_target_properties(tests_mpi PROPERTIES FOLDER Testing)
else()
    add_custom_target(tests
        $<TARGET_FILE:testrunner> ${TESTRUNNER_ADDITIONAL_ARGUMENTS}
        DEPENDS testrunner tests-cleanup
    )
endif()

set_target_properties(tests PROPERTIES FOLDER Testing)

# Creates one ctest entry for every googletest
#ADD_GOOGLE_TESTS ( ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/testrunner ${TEST_SOURCES})
