/**
 * \file
 *
 * \copyright
 * Copyright (c) 2012-2017, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include <tclap/CmdLine.h>
#include <dune/grid/uggrid.hh>

// BaseLib
#include "BaseLib/BuildInfo.h"
int main(int argc, char *argv[])
{
    // Parse CLI arguments.
    TCLAP::CmdLine cmd("OpenGeoSys-6 software.\n"
            "Copyright (c) 2012-2017, OpenGeoSys Community "
            "(http://www.opengeosys.org) "
            "Distributed under a Modified BSD License. "
            "See accompanying file LICENSE.txt or "
            "http://www.opengeosys.org/project/license\n"
            "version: " + BaseLib::BuildInfo::git_describe,
        ' ',
        BaseLib::BuildInfo::git_describe);

    TCLAP::UnlabeledValueArg<std::string> project_arg(
        "project-file",
        "Path to the ogs6 project file.",
        true,
        "",
        "PROJECT FILE");
    cmd.add(project_arg);

    TCLAP::ValueArg<std::string> outdir_arg(
        "o", "output-directory",
        "the output directory to write to",
        false,
        "",
        "output directory");
    cmd.add(outdir_arg);


    Dune::UGGrid<3> grid;
}
