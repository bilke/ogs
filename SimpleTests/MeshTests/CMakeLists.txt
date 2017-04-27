if(OGS_USE_DUNE)
    add_executable(DuneGrid DuneGrid.cpp)

    target_link_libraries(DuneGrid
        dunegrid
        BaseLib
    )
endif()

add_executable(MeshRead MeshRead.cpp)

target_link_libraries(MeshRead
    MeshLib
    MathLib
    BaseLib
    GeoLib
    ${BOOST_LIBRARIES}
)

add_executable(MeshSearchTest MeshSearchTest.cpp)

target_link_libraries(MeshSearchTest
    MeshLib
    MathLib
    BaseLib
    GeoLib
    ${BOOST_LIBRARIES}
)
