######################
### Find tools     ###
######################

find_package(Doxygen OPTIONAL_COMPONENTS dot)

# Find gnu profiler gprof
find_program(GPROF_PATH gprof DOC "GNU profiler gprof" QUIET)

find_program(CPPCHECK_TOOL_PATH cppcheck)

find_package(Python COMPONENTS Interpreter Development)

# Find bash itself ...
find_program(BASH_TOOL_PATH bash
    HINTS ${GITHUB_BIN_DIR} DOC "The bash executable")

# Dumpbin is a windows dependency analaysis tool required for packaging.
# Variable has to be named gp_cmd to override the outdated find routines
# of the GetPrerequisites CMake-module.
if(WIN32)
    include(MSVCPaths)
    find_program(gp_cmd dumpbin DOC "Windows dependency analysis tool"
        PATHS ${MSVC_INSTALL_PATHS} PATH_SUFFIXES VC/bin)
    if(gp_cmd)
        get_filename_component(dir ${gp_cmd} PATH)
        set(ENV{PATH} "${dir}/../../../Common7/IDE;$ENV{PATH}")
    endif()
endif()

find_program(CURL_TOOL_PATH curl DOC "The curl-tool")

find_program(S3CMD_TOOL_PATH s3cmd DOC "S3cmd tool for uploading to Amazon S3")

find_program(CCACHE_TOOL_PATH ccache)

# Tools for web
find_program(VTKJS_CONVERTER vtkDataConverter
    PATHS ${PROJECT_SOURCE_DIR}/web/node_modules/.bin)
find_program(HUGO hugo)
find_program(NPM npm)
find_program(YARN yarn)
find_program(PIP pip)
find_program(PANDOC_CITEPROC pandoc-citeproc)

find_program(MODULE_CMD modulecmd
    PATHS /usr/local/modules/3.2.10-1/Modules/3.2.10/bin)

######################
### Find libraries ###
######################
set(VTK_COMPONENTS vtkIOXML)
if(OGS_BUILD_GUI)
    set(VTK_COMPONENTS ${VTK_COMPONENTS}
        vtkIOImage vtkIOLegacy vtkIOExport vtkIOExportPDF
        vtkIOExportOpenGL2 vtkInteractionStyle vtkInteractionWidgets
        vtkGUISupportQt vtkRenderingOpenGL2 vtkRenderingContextOpenGL2
        vtkFiltersTexture vtkRenderingCore
    )
endif()
if(OGS_USE_MPI)
    set(VTK_COMPONENTS ${VTK_COMPONENTS} vtkIOParallelXML vtkParallelMPI)
endif()

if(OGS_USE_CONAN)
    find_package(boost REQUIRED CONFIG)
    set_target_properties(boost::boost PROPERTIES IMPORTED_GLOBAL TRUE)
    add_library(Boost::boost ALIAS boost::boost)
    find_package(eigen REQUIRED CONFIG)
    find_package(vtk REQUIRED CONFIG)
    set(VTK_LIBRARIES vtk::vtk CACHE INTERNAL "")
else()
    find_package(Boost 1.62.0 REQUIRED)
    find_package(VTK 8.1.2 REQUIRED COMPONENTS ${VTK_COMPONENTS})
    include(${VTK_USE_FILE})
    find_package(Eigen3 3.3.4 REQUIRED)
    include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})
endif()

## pthread, is a requirement of logog ##
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if(CMAKE_USE_PTHREADS_INIT)
    set(HAVE_PTHREADS TRUE)
    add_definitions(-DHAVE_PTHREADS)
endif()

# Do not search for libs if this option is set
if(OGS_NO_EXTERNAL_LIBS)
    return()
endif() # OGS_NO_EXTERNAL_LIBS

find_package(OpenMP)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_package(Metis QUIET)

## Qt5 library ##
if(OGS_BUILD_GUI)
    set(QT_MODULES Gui Widgets Xml XmlPatterns)
    if(OGS_USE_CONAN AND UNIX AND NOT APPLE)
        set(QT_MODULES ${QT_MODULES} X11Extras)
    endif()
    if(OGS_USE_CONAN)
        find_package(qt REQUIRED CONFIG)
        set_target_properties(qt::qt PROPERTIES IMPORTED_GLOBAL TRUE)
        add_library(Qt5::Core ALIAS qt::qt)
        add_library(Qt5::Gui ALIAS qt::qt)
        add_library(Qt5::Widgets ALIAS qt::qt)
        add_library(Qt5::Xml ALIAS qt::qt)
        add_library(Qt5::Network ALIAS qt::qt)
        add_library(Qt5::XmlPatterns ALIAS qt::qt)
        # https://gitlab.kitware.com/cmake/cmake/issues/19167#note_558442
        set(Qt5Core_VERSION_MAJOR 5 CACHE INTERNAL "")
        set(Qt5Core_VERSION_MINOR 12 CACHE INTERNAL "")
        add_executable(Qt5::moc IMPORTED)
        include(${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)
        file(TO_NATIVE_PATH "${CONAN_BIN_DIRS_QT}/moc" MOC_PATH)
        #set_target_properties(Qt5::moc PROPERTIES IMPORTED_LOCATION ${MOC_PATH})
        set_target_properties(Qt5::moc PROPERTIES IMPORTED_LOCATION ${CONAN_BIN_DIRS_QT}/moc)
    else()
        find_package(Qt5 5.2 REQUIRED ${QT_MODULES})
    endif()
    cmake_policy(SET CMP0020 NEW)
endif()

if(OGS_USE_NETCDF)
    if(OGS_USE_CONAN)
        find_package(netcdf REQUIRED CONFIG)
        set(NetCDF_LIBRARIES netcdf::netcdf CACHE INTERNAL "")
    else()
        set(NETCDF_ROOT ${CONAN_NETCDF-C_ROOT})
        set(NETCDF_CXX_ROOT ${CONAN_NETCDF-CXX_ROOT})
        find_package(NetCDF REQUIRED)
    endif()
endif()

# lapack
find_package(LAPACK QUIET)

## geotiff ##
if(OGS_USE_CONAN)
    find_package(geotiff CONFIG)
    set(GeoTiff_LIBRARIES geotiff::geotiff CACHE INTERNAL "")
else()
    find_package(LibGeoTiff)
endif()
if(GEOTIFF_FOUND)
    add_definitions(-DGEOTIFF_FOUND)
endif() # GEOTIFF_FOUND

## lis ##
if(OGS_USE_LIS)
    find_package( LIS REQUIRED )
endif()

if(OGS_USE_MKL)
    find_package( MKL REQUIRED )
endif()

if(OGS_USE_PETSC)
    message(STATUS "Configuring for PETSc")

    option(FORCE_PETSC_EXECUTABLE_RUNS
        "Force CMake to accept a given PETSc configuration" ON)

    # Force CMake to accept a given PETSc configuration in case the failure of
    # MPI tests. This may cause the compilation broken.
    if(FORCE_PETSC_EXECUTABLE_RUNS)
        set(PETSC_EXECUTABLE_RUNS YES)
    endif()

    find_package(PETSc REQUIRED)

    include_directories(SYSTEM ${PETSC_INCLUDES})

    add_definitions(-DPETSC_VERSION_NUMBER=PETSC_VERSION_MAJOR*1000+PETSC_VERSION_MINOR*10)

endif()

find_package(OpenSSL)

## Check MPI package
if(OGS_USE_MPI)
    find_package(MPI REQUIRED)
endif()

if(OGS_USE_CONAN)
    find_package(shapelib REQUIRED CONFIG)
    set(Shapelib_LIBRARIES shapelib::shapelib CACHE INTERNAL "")
    set(Shapelib_FOUND TRUE CACHE INTERNAL "")
else()
    find_package(Shapelib)
endif()
if(OGS_BUILD_GUI AND NOT Shapelib_FOUND)
    message(FATAL_ERROR "Shapelib not found but it is required for OGS_BUILD_GUI!")
endif()

## Sundials cvode ode-solver library
if(OGS_USE_CVODE)
    find_package(CVODE REQUIRED)
    add_definitions(-DCVODE_FOUND)
endif()

if(OGS_USE_MFRONT)
    find_package(MGIS REQUIRED)
endif()
