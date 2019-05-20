/**
 * \file   MeshSurfaceExtraction.h
 * \author Karsten Rink
 * \date   2013-04-04
 * \brief  Definition of the MeshSurfaceExtraction class
 *
 * \copyright
 * Copyright (c) 2012-2019, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include <cstddef>
#include <vector>

#include "MathLib/Vector3.h"
#include "MeshLib/Mesh.h"
#include "MeshLib/Properties.h"

namespace MeshLib
{
// forward declarations
class Mesh;
class Element;
class Node;

/**
 * \brief A set of tools concerned with extracting nodes and elements from a
 * mesh surface
 */
class MeshSurfaceExtraction
{
public:
    /// Returns a vector of the areas assigned to each node on a surface mesh.
    static std::vector<double> getSurfaceAreaForNodes(
        const MeshLib::Mesh& mesh);

    /// Returns the surface nodes of a mesh.
    static std::vector<MeshLib::Node*> getSurfaceNodes(
        const MeshLib::Mesh& mesh, const MathLib::Vector3& dir, double angle);

    /**
     * Returns the 2d-element mesh representing the surface of the given mesh.
     * \param subsfc_mesh The original mesh
     * \param dir The direction in which face normals have to
     * point to be considered surface elements
     * \param angle The angle of the allowed deviation from the
     * given direction (0 <= angle <= 90 degrees)
     * \param subsfc_node_id_prop_name The name of the \c PropertyVector in
     * the surface mesh the subsurface mesh node ids are stored to. If the
     * string is empty, there won't be such a \c PropertyVector.
     * \param subsfc_element_id_prop_name The name of the PropertyVector in
     * the surface mesh that stores the subsurface element ids. If the string
     * is empty, there won't be such a \c PropertyVector.
     * \param face_id_prop_name The name of the \c PropertyVector in the surface
     * mesh that stores the face number of the subsurface element that belongs
     * to the boundary. If the string is empty, there won't be such a \c
     * PropertyVector.
     * \return A 2D mesh representing the surface in direction dir
     */
    static MeshLib::Mesh* getMeshSurface(
        const MeshLib::Mesh& subsfc_mesh,
        const MathLib::Vector3& dir,
        double angle,
        std::string const& subsfc_node_id_prop_name = "",
        std::string const& subsfc_element_id_prop_name = "",
        std::string const& face_id_prop_name = "");

    /**
     * Returns the boundary of mesh, i.e. lines for 2D meshes and surfaces for
     * 3D meshes. Note, that this method also returns inner boundaries and might
     * give unexpected results when the mesh geometry is not strict (e.g. more
     * than two triangles sharing an edge). \param mesh The original mesh of
     * dimension d \return     A mesh of dimension (d-1) representing the
     * boundary of the mesh.
     */
    static MeshLib::Mesh* getMeshBoundary(const MeshLib::Mesh& mesh);

private:
    /// Functionality needed for getSurfaceNodes() and getMeshSurface()
    static void get2DSurfaceElements(
        const std::vector<MeshLib::Element*>& all_elements,
        std::vector<MeshLib::Element*>& sfc_elements,
        std::vector<std::size_t>& element_to_bulk_element_id_map,
        std::vector<std::size_t>& element_to_bulk_face_id_map,
        const MathLib::Vector3& dir,
        double angle,
        unsigned mesh_dimension);

    /// Functionality needed for getSurfaceNodes() and getMeshSurface()
    static void get2DSurfaceNodes(
        std::vector<MeshLib::Node*>& sfc_nodes,
        std::size_t n_all_nodes,
        const std::vector<MeshLib::Element*>& sfc_elements,
        std::vector<std::size_t>& node_id_map);

    /// Creates the element vector for the 2d surface mesh
    static std::vector<MeshLib::Element*> createSfcElementVector(
        std::vector<MeshLib::Element*> const& sfc_elems,
        std::vector<MeshLib::Node*> const& sfc_nodes,
        std::vector<std::size_t> const& node_id_map);

    template <typename T>
    static bool processPropertyVector(std::string const& name,
                                      MeshLib::MeshItemType const type,
                                      MeshLib::Properties const& properties,
                                      std::size_t const /*vec_size*/,
                                      std::vector<std::size_t> const& id_map,
                                      MeshLib::Mesh& sfc_mesh)
    {
        if (!properties.existsPropertyVector<T>(name, type, 1))
        {
            return false;
        }
        auto sfc_prop = getOrCreateMeshProperty<T>(sfc_mesh, name, type, 1);
        sfc_prop->clear();
        sfc_prop->reserve(id_map.size());

        auto const& org_vec = *properties.getPropertyVector<T>(name, type, 1);
        std::transform(
            begin(id_map), end(id_map), std::back_inserter(*sfc_prop),
            [&org_vec](std::size_t const bulk_id) { return org_vec[bulk_id]; });

        return true;
    }

    /// Copies relevant parts of scalar arrays to the surface mesh
    static bool createSfcMeshProperties(
        MeshLib::Mesh& sfc_mesh,
        MeshLib::Properties const& properties,
        std::vector<std::size_t> const& node_ids_map,
        std::vector<std::size_t> const& element_ids_map);
};

}  // end namespace MeshLib
