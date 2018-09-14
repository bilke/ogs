add_library(ProcessLibBoundaryConditionPython
    PythonBoundaryCondition.cpp
    PythonBoundaryCondition.h
    PythonBoundaryConditionLocalAssembler.h
    PythonBoundaryConditionPythonSideInterface.h)
install(TARGETS ProcessLibBoundaryConditionPython
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_compile_definitions(ProcessLibBoundaryConditionPython
    PUBLIC OGS_USE_PYTHON)

target_link_libraries(ProcessLibBoundaryConditionPython
    PUBLIC BaseLib MathLib MeshLib NumLib logog
    PRIVATE pybind11::pybind11)

# For the embedded Python module
add_library(ProcessLibBoundaryConditionPythonModule
    PythonBoundaryConditionModule.cpp
    PythonBoundaryConditionModule.h)
install(TARGETS ProcessLibBoundaryConditionPythonModule
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_link_libraries(ProcessLibBoundaryConditionPythonModule
    PUBLIC
    ProcessLibBoundaryConditionPython
    pybind11::pybind11)
