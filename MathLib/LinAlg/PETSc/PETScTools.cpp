/*!
   \file  PETScTools.cpp
   \brief Definition of a function related to PETSc solver interface to assign
         the Dirichlet boundary conditions.

   \author Wenqing Wang
   \version
   \date Nov 2011 - Sep 2013

   \copyright
    Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
               Distributed under a Modified BSD License.
               See accompanying file LICENSE.txt or
               http://www.opengeosys.org/project/license
*/

#include "PETScTools.h"
#include <logog/include/logog.hpp>
#include "PETScVector.h"

namespace MathLib
{
void applyKnownSolution(PETScMatrix& A, PETScVector& b, PETScVector& x,
                        const std::vector<PetscInt>& vec_knownX_id,
                        const std::vector<PetscScalar>& vec_knownX_x)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    INFO("[%d] applying %d dirichlet conditions", rank, vec_knownX_id.size());
    A.finalizeAssembly();
    x.finalizeAssembly();
    if (vec_knownX_id.size() > 0)
    {
        x.set(vec_knownX_id, vec_knownX_x);
    }
    x.finalizeAssembly();

    const PetscScalar one = 1.0;
    const PetscInt nrows = static_cast<PetscInt>(vec_knownX_id.size());

    PETSc_Mat & mat(A.getRawMatrix());

    // Each process will only zero its own rows.
    // This avoids all reductions in the zero row routines
    // and thus improves performance for very large process counts.
    // See PETSc doc about MAT_NO_OFF_PROC_ZERO_ROWS.
    // MatSetOption(mat, MAT_NO_OFF_PROC_ZERO_ROWS, PETSC_TRUE);

    // Keep the non-zero pattern for the assignment operator.
    MatSetOption(mat, MAT_KEEP_NONZERO_PATTERN, PETSC_TRUE);

    Vec& solution(x.getRawVector());
    Vec& rhs(b.getRawVector());
    if (nrows > 0)
    {
        MatZeroRowsColumns(mat, nrows, vec_knownX_id.data(), one, solution, rhs);
    }
    else
    {
        MatZeroRowsColumns(mat, 0, PETSC_NULL, one, solution, rhs);
    }

    A.finalizeAssembly();

    /*
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        INFO("rank %d has %d dirichlet conditions", rank, vec_knownX_id.size());
        for (std::size_t i(0); i < vec_knownX_id.size(); ++i)
            INFO("rank %d: dirichlet condition %e at dof %d", rank,
       vec_knownX_x[i], vec_knownX_id[i]);

        A.finalizeAssembly();

        // update b
        b.finalizeAssembly();
        PETSc_Mat const& mat(A.getRawMatrix());
        PetscInt ncols(0);
        for (std::size_t n(0); n < vec_knownX_id.size(); ++n)
        {
            PetscInt const* cols;
            PetscScalar const* mat_vals;
            MatGetRow(mat, vec_knownX_id[n], &ncols, &cols, &mat_vals);
            INFO("%d column entries in row %d.", ncols, vec_knownX_id[n]);
            b.setLocalAccessibleVector();
            for (PetscInt i(0); i < ncols; ++i)
            {
                INFO("column %d value %e.", cols[i], mat_vals[i]);
                // b.add(cols[i], - mat_vals[i] * vec_knownX_x[n]);
                auto temp = b[cols[i]];
                b.set(cols[i], temp - mat_vals[i] * vec_knownX_x[n]);
                // INFO("set 'dirichlet value' at pos %d to %e.", cols[i],
       b[cols[i]]);
            }
            MatRestoreRow(mat, vec_knownX_id[n], &ncols, &cols, &mat_vals);
        }
        A.setRowsColumnsZero(vec_knownX_id);
        A.finalizeAssembly();

        x.finalizeAssembly();
        b.finalizeAssembly();
        if (vec_knownX_id.size() > 0)
        {
            x.set(vec_knownX_id, vec_knownX_x);
            b.set(vec_knownX_id, vec_knownX_x);
        }
    */

        x.finalizeAssembly();
        b.finalizeAssembly();
}

}  // end of namespace MathLib
