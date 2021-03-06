GET_SOURCE_FILES(SOURCES)
APPEND_SOURCE_FILES(SOURCES Gmsh)
APPEND_SOURCE_FILES(SOURCES Legacy)

if(NOT Shapelib_FOUND)
    list(REMOVE_ITEM SOURCES SHPInterface.h SHPInterface.cpp)
endif()

# GO2OGS
GET_SOURCE_FILES(SOURCES_GO2OGS GocadIO)
set(SOURCES ${SOURCES} ${SOURCES_GO2OGS})

if(OGS_BUILD_GUI)
    APPEND_SOURCE_FILES(SOURCES XmlIO/Qt)
    APPEND_SOURCE_FILES(SOURCES FEFLOW)
endif()

if(OGS_BUILD_SWMM)
    GET_SOURCE_FILES(SOURCES_SWMM SWMM)
    SET(SOURCES ${SOURCES} ${SOURCES_SWMM})
endif()

include(${PROJECT_SOURCE_DIR}/scripts/cmake/OGSEnabledElements.cmake)

# Create the library
add_library(ApplicationsFileIO ${SOURCES})
if(BUILD_SHARED_LIBS)
    install(TARGETS ApplicationsFileIO LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
target_link_libraries(ApplicationsFileIO
    PUBLIC BaseLib DataHolderLib GeoLib MathLib logog
    PRIVATE MeshLib
)

if(OGS_BUILD_GUI)
    # Needed for the XmlPrjInterface, which links the DE/Base/OGSError.h.
    target_link_libraries(ApplicationsFileIO PUBLIC QtBase)
endif()

if(Shapelib_FOUND)
    target_link_libraries(ApplicationsFileIO PRIVATE ${Shapelib_LIBRARIES})
endif()

if(OGS_BUILD_SWMM)
    target_link_libraries(ApplicationsFileIO PRIVATE SwmmInterface)
endif()

if(OGS_USE_PCH)
    cotire(ApplicationsFileIO)
endif()


configure_file(XmlIO/OpenGeoSysCND.xsd ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/OpenGeoSysCND.xsd COPYONLY)
configure_file(XmlIO/OpenGeoSysNum.xsd ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/OpenGeoSysNum.xsd COPYONLY)
configure_file(XmlIO/OpenGeoSysProject.xsd ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/OpenGeoSysProject.xsd COPYONLY)
install(FILES
    XmlIO/OpenGeoSysCND.xsd
    XmlIO/OpenGeoSysNum.xsd
    XmlIO/OpenGeoSysProject.xsd
    DESTINATION bin COMPONENT ogs_cli
)
