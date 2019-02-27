/**
 * \copyright
 * Copyright (c) 2012-2019, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include <pybind11/pybind11.h>
#include <algorithm>
#include <iostream>
#include <logog/include/logog.hpp>
#include <vector>

#include "BHEInflowPythonBoundaryCondition.h"
#include "BaseLib/Error.h"
#include "ProcessLib/Utils/CreateLocalAssemblers.h"
#include "ProcessLib/Utils/ProcessUtils.h"
#include "PythonBoundaryConditionLocalAssembler.h"

namespace ProcessLib
{
template <typename BHEType>
BHEInflowPythonBoundaryCondition<BHEType>::BHEInflowPythonBoundaryCondition(
    std::pair<GlobalIndexType, GlobalIndexType>&& in_out_global_indices,
    BHEType& bhe,
    BHEInflowPythonBoundaryConditionPythonSideInterface& py_bc_object)
    : _in_out_global_indices(std::move(in_out_global_indices)),
      _bhe(bhe),
      _py_bc_object(py_bc_object)
{
    const auto g_idx_T_out = in_out_global_indices.second;

    // store the bc node ids to BHE network dataframe
    std::get<3>(_py_bc_object.dataframe_network).emplace_back(g_idx_T_out);
}

template <typename BHEType>
void BHEInflowPythonBoundaryCondition<BHEType>::getEssentialBCValues(
    const double t, GlobalVector const& /*x*/,
    NumLib::IndexValueVector<GlobalIndexType>& bc_values) const
{
    bc_values.ids.resize(1);
    bc_values.values.resize(1);
    auto const& data_exchange = _py_bc_object.dataframe_network;
    // get the number of all boundary nodes
    const std::size_t n_bc_nodes = std::get<3>(data_exchange).size();

    // get T_in bc_id
    bc_values.ids[0] = _in_out_global_indices.first;

    // get T_out bc_id
    auto const boundary_node_id = _in_out_global_indices.second;

    // return T_in from currently BHE dataframe column 2
    for (std::size_t i = 0; i < n_bc_nodes; i++)
    {

        // auto pair_flag_value = _bc_data.bc_object->getDirichletBCValue(boundary_node_id);
        auto const dataframe_node_id = std::get<3>(data_exchange);
        auto const dataframe_Tin_val = std::get<1>(data_exchange);
        if (dataframe_node_id[i] == boundary_node_id)
        {
            bc_values.values[0] = dataframe_Tin_val[i];
            break;
        }
    }

    // store the current time to network dataframe
    std::get<4>(_py_bc_object.dataframe_network) = t;
}

template <typename BHEType>
std::unique_ptr<BHEInflowPythonBoundaryCondition<BHEType>>
createBHEInflowPythonBoundaryCondition(
    std::pair<GlobalIndexType, GlobalIndexType>&& in_out_global_indices,
    BHEType& bhe,
    BHEInflowPythonBoundaryConditionPythonSideInterface& py_bc_object)

{
    DBUG("Constructing BHEInflowPythonBoundaryCondition.");

    // In case of partitioned mesh the boundary could be empty, i.e. there is no
    // boundary condition.
#ifdef USE_PETSC
    // For this special boundary condition the boundary condition is not empty
    // if the global indices are non-negative.
    if (in_out_global_indices.first < 0 && in_out_global_indices.second < 0)
    {
        return nullptr;
    }
    // If only one of the global indices (in or out) is negative the
    // implementation is not valid.
    if (in_out_global_indices.first < 0 || in_out_global_indices.second < 0)
    {
        OGS_FATAL(
            "The partition cuts the BHE into two independent parts. This "
            "behaviour is not implemented.");
    }
#endif  // USE_PETSC
    return std::make_unique<BHEInflowPythonBoundaryCondition<BHEType>>(
        std::move(in_out_global_indices), bhe, py_bc_object);
}
}  // namespace ProcessLib
