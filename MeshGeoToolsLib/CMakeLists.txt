# Source files
GET_SOURCE_FILES(SOURCES)

# Create the library
add_library(MeshGeoToolsLib ${SOURCES})
install(TARGETS MeshGeoToolsLib LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_link_libraries(MeshGeoToolsLib
    PUBLIC GeoLib MathLib
    PRIVATE BaseLib MeshLib logog
)

if(OGS_USE_PCH)
    cotire(MeshGeoToolsLib)
endif()
