/**
 * \copyright
 * Copyright (c) 2012-2019, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include "BHEInflowPythonBoundaryConditionPythonSideInterface.h"
#include "NumLib/IndexValueVector.h"
#include "ProcessLib/BoundaryCondition/BoundaryCondition.h"
#include "ProcessLib/BoundaryCondition/GenericNaturalBoundaryConditionLocalAssembler.h"
#include "ProcessLib/HeatTransportBHE/BHE/BHETypes.h"

namespace ProcessLib
{
//! A boundary condition whose values are computed by a Python script.
template <typename BHEType>
class BHEInflowPythonBoundaryCondition final : public BoundaryCondition
{
public:
    BHEInflowPythonBoundaryCondition(
        std::pair<GlobalIndexType, GlobalIndexType>&& in_out_global_indices,
        BHEType& bhe,
        BHEInflowPythonBoundaryConditionPythonSideInterface& py_bc_object);


    void getEssentialBCValues(
        const double t, const GlobalVector& x,
        NumLib::IndexValueVector<GlobalIndexType>& bc_values) const override;

private:
    std::pair<GlobalIndexType, GlobalIndexType> const _in_out_global_indices;
    BHEType& _bhe;
    BHEInflowPythonBoundaryConditionPythonSideInterface& _py_bc_object;
};

//! Creates a new PythonBoundaryCondition object.
template <typename BHEType>
std::unique_ptr<BHEInflowPythonBoundaryCondition<BHEType>>
createBHEInflowPythonBoundaryCondition(
    std::pair<GlobalIndexType, GlobalIndexType>&& in_out_global_indices,
    BHEType& bhe,
    BHEInflowPythonBoundaryConditionPythonSideInterface& py_bc_object);
}  // namespace ProcessLib
