/**
 * \copyright
 * Copyright (c) 2012-2019, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include "CreateHeatTransportBHEProcess.h"

#include <vector>

#include "ProcessLib/Output/CreateSecondaryVariables.h"
#include "ProcessLib/Utils/ProcessUtils.h"

#include "BHE/BHETypes.h"
#include "BHE/CreateBHE1U.h"
#include "BHE/CreateBHECoaxial.h"
#include "HeatTransportBHEProcess.h"
#include "HeatTransportBHEProcessData.h"

#include <pybind11/pybind11.h>
#include <iostream>
namespace ProcessLib
{
namespace HeatTransportBHE
{
std::unique_ptr<Process> createHeatTransportBHEProcess(
    MeshLib::Mesh& mesh,
    std::unique_ptr<ProcessLib::AbstractJacobianAssembler>&& jacobian_assembler,
    std::vector<ProcessVariable> const& variables,
    std::vector<std::unique_ptr<ParameterBase>> const& parameters,
    unsigned const integration_order,
    BaseLib::ConfigTree const& config,
    std::map<std::string,
             std::unique_ptr<MathLib::PiecewiseLinearInterpolation>> const&
        curves)
{
    //! \ogs_file_param{prj__processes__process__type}
    config.checkConfigParameter("type", "HEAT_TRANSPORT_BHE");

    DBUG("Create HeatTransportBHE Process.");

    // Process variable.

    //! \ogs_file_param{prj__processes__process__HEAT_TRANSPORT_BHE__process_variables}
    auto const pv_config = config.getConfigSubtree("process_variables");
    std::vector<std::vector<std::reference_wrapper<ProcessVariable>>>
        process_variables;

    // reading primary variables for each
    // BHE----------------------------------------------------------
    auto range =
        //! \ogs_file_param{prj__processes__process__HEAT_TRANSPORT_BHE__process_variables__process_variable}
        pv_config.getConfigParameterList<std::string>("process_variable");
    std::vector<std::reference_wrapper<ProcessVariable>> per_process_variables;

    for (std::string const& pv_name : range)
    {
        if (pv_name != "temperature_soil" &&
            pv_name.find("temperature_BHE") == std::string::npos)
        {
            OGS_FATAL(
                "Found a process variable name '%s'. It should be "
                "'temperature_soil' or 'temperature_BHE_X'");
        }
        auto variable = std::find_if(variables.cbegin(), variables.cend(),
                                     [&pv_name](ProcessVariable const& v) {
                                         return v.getName() == pv_name;
                                     });

        if (variable == variables.end())
        {
            OGS_FATAL(
                "Could not find process variable '%s' in the provided "
                "variables "
                "list for config tag <%s>.",
                pv_name.c_str(), "process_variable");
        }
        DBUG("Found process variable '%s' for config tag <%s>.",
             variable->getName().c_str(), "process_variable");

        per_process_variables.emplace_back(
            const_cast<ProcessVariable&>(*variable));
    }
    process_variables.push_back(std::move(per_process_variables));
    // end of reading primary variables for each
    // BHE----------------------------------------------------------

    // solid phase thermal conductivity parameter.
    auto& thermal_conductivity_solid = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HEAT_TRANSPORT_BHE__thermal_conductivity_solid}
        "thermal_conductivity_solid", parameters, 1);

    DBUG("Use '%s' as solid phase thermal conductivity parameter.",
         thermal_conductivity_solid.name.c_str());

    // solid phase thermal conductivity parameter.
    auto& thermal_conductivity_fluid = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HEAT_TRANSPORT_BHE__thermal_conductivity_fluid}
        "thermal_conductivity_fluid", parameters, 1);

    DBUG("Use '%s' as fluid phase thermal conductivity parameter.",
         thermal_conductivity_fluid.name.c_str());

    // gas phase thermal conductivity parameter.
    auto& thermal_conductivity_gas = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HEAT_TRANSPORT_BHE__thermal_conductivity_gas}
        "thermal_conductivity_gas", parameters, 1);

    DBUG("Use '%s' as gas phase thermal conductivity parameter.",
         thermal_conductivity_gas.name.c_str());

    // solid phase heat capacity parameter.
    auto& heat_capacity_solid = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HEAT_TRANSPORT_BHE__heat_capacity_solid}
        "heat_capacity_solid", parameters, 1);

    DBUG("Use '%s' as solid phase heat capacity parameter.",
         heat_capacity_solid.name.c_str());

    // fluid phase heat capacity parameter.
    auto& heat_capacity_fluid = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HEAT_TRANSPORT_BHE__heat_capacity_fluid}
        "heat_capacity_fluid", parameters, 1);

    DBUG("Use '%s' as fluid phase heat capacity parameter.",
         heat_capacity_fluid.name.c_str());

    // gas phase heat capacity parameter.
    auto& heat_capacity_gas = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HEAT_TRANSPORT_BHE__heat_capacity_gas}
        "heat_capacity_gas", parameters, 1);

    DBUG("Use '%s' as gas phase heat capacity parameter.",
         heat_capacity_gas.name.c_str());

    // solid phase density parameter.
    auto& density_solid = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HEAT_TRANSPORT_BHE__density_solid}
        "density_solid", parameters, 1);

    DBUG("Use '%s' as solid phase density parameter.",
         density_solid.name.c_str());

    // fluid phase density parameter.
    auto& density_fluid = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HEAT_TRANSPORT_BHE__density_fluid}
        "density_fluid", parameters, 1);

    DBUG("Use '%s' as fluid phase density parameter.",
         density_fluid.name.c_str());

    // gas phase density parameter.
    auto& density_gas = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HEAT_TRANSPORT_BHE__density_gas}
        "density_gas", parameters, 1);

    DBUG("Use '%s' as gas phase density parameter.", density_gas.name.c_str());

    // reading BHE parameters --------------------------------------------------
    std::vector<BHE::BHETypes> bhes;

    // bhe array network parameter.
    bool if_bhe_network_exist_python_bc = false;

    auto const& bhe_configs =
        //! \ogs_file_param{prj__processes__process__HEAT_TRANSPORT_BHE__borehole_heat_exchangers}
        config.getConfigSubtree("borehole_heat_exchangers");

    for (
        auto const& bhe_config :
        //! \ogs_file_param{prj__processes__process__HEAT_TRANSPORT_BHE__borehole_heat_exchangers__borehole_heat_exchanger}
        bhe_configs.getConfigSubtreeList("borehole_heat_exchanger"))
    {
        // read in the parameters
        const std::string bhe_type =
            //! \ogs_file_param{prj__processes__process__HEAT_TRANSPORT_BHE__borehole_heat_exchangers__borehole_heat_exchanger__type}
            bhe_config.getConfigParameter<std::string>("type");

        if (bhe_type == "1U")
        {
            bhes.push_back(BHE::createBHE1U(bhe_config, curves));
            continue;
        }

        if (bhe_type == "CXA")
        {
            bhes.push_back(BHE::createBHECoaxial<BHE::BHE_CXA>(bhe_config, curves));
            continue;
        }

        if (bhe_type == "CXC")
        {
            bhes.push_back(BHE::createBHECoaxial<BHE::BHE_CXC>(bhe_config, curves));
            continue;
        }
        OGS_FATAL("Unknown BHE type '%s'.", bhe_type.c_str());
    }
    // end of reading BHE parameters -------------------------------------------

    // find if bhe uses python boundary condition
    auto const isUsingPythonBC =
        apply_visitor([](auto const& bhe) { return bhe.ifUsePythonBC; },
                      bhes[0]);
    if (isUsingPythonBC == true)
    {
        if_bhe_network_exist_python_bc = true;
    }
    //! Python object computing BC values.
    BHEInflowPythonBoundaryConditionPythonSideInterface* bc = nullptr;
    // create a pythonBoundaryCondition object
    if (if_bhe_network_exist_python_bc == true)
    {
        // Evaluate Python code in scope of main module
        pybind11::object scope =
            pybind11::module::import("__main__").attr("__dict__");

        if (!scope.contains("bc_bhe"))
            OGS_FATAL(
                "Function 'bc_bhe' is not defined in the python script file, "
                "or there "
                "was no python script file specified.");

        bc = scope["bc_bhe"]
                 .cast<BHEInflowPythonBoundaryConditionPythonSideInterface*>();

        if (bc == nullptr)
            OGS_FATAL(
                "Not able to access the correct bc pointer from python script "
                "file specified.");

        // create BHE network dataframe from Python
        bc->dataframe_network = bc->initializeDataContainer();
        // clear ogs bc_node_id memory in dataframe
        std::get<3>(bc->dataframe_network).clear();  // ogs_bc_node_id

        // here calls the tespyHydroSolver to get the pipe flow velocity in bhe
        // network, and replace the value in flow velocity Matrix _u
        auto const tespy_flow_rate = std::get<1>(bc->tespyHydroSolver());
        const std::size_t n_bhe = tespy_flow_rate.size();
        if (bhes.size() != n_bhe)
            OGS_FATAL(
                "The number of BHEs defined in OGS and TESPy are not the "
                "same!");

        for (std::size_t idx_bhe = 0; idx_bhe < n_bhe; idx_bhe++)
        {
            // the flow_rate in OGS should be updated from the flow_rate
            // computed by TESPy.
            auto update_flow_rate = [&](auto& bhe) {
                bhe.updateHeatTransferCoefficients(tespy_flow_rate[idx_bhe]);
            };
            apply_visitor(update_flow_rate, bhes[idx_bhe]);
        }
    }

    HeatTransportBHEProcessData process_data(thermal_conductivity_solid,
                                             thermal_conductivity_fluid,
                                             thermal_conductivity_gas,
                                             heat_capacity_solid,
                                             heat_capacity_fluid,
                                             heat_capacity_gas,
                                             density_solid,
                                             density_fluid,
                                             density_gas,
                                             std::move(bhes),
                                             if_bhe_network_exist_python_bc,
                                             bc);

    SecondaryVariableCollection secondary_variables;

    NumLib::NamedFunctionCaller named_function_caller(
        {"HeatTransportBHE_Temperature"});

    ProcessLib::createSecondaryVariables(config, secondary_variables,
                                         named_function_caller);

    return std::make_unique<HeatTransportBHEProcess>(
        mesh, std::move(jacobian_assembler), parameters, integration_order,
        std::move(process_variables), std::move(process_data),
        std::move(secondary_variables), std::move(named_function_caller));
}
}  // namespace HeatTransportBHE
}  // namespace ProcessLib
