include("${CTEST_SCRIPT_DIRECTORY}/CDashSetup.cmake")

ctest_start(Experimental)
ctest_configure(OPTIONS -DOGS_USE_CONAN=ON)
ctest_build()
ctest_test(
    EXCLUDE ".*LARGE.*|.*1e4.*"
    PARALLEL_LEVEL 3
)
ctest_submit()
