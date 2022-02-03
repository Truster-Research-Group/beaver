//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeHeatFluxAverJumpLocal.h"

InputParameters
ComputeHeatFluxAverJumpLocal::validParams()
{
  InputParameters params = ComputeHeatFluxAverJumpBase::validParams();
  params.addClassDescription("Compute heat flux avearge and jump using "
                             "material vectors from elem and neig sides.");

  return params;
}

ComputeHeatFluxAverJumpLocal::ComputeHeatFluxAverJumpLocal(
    const InputParameters & parameters)
  : ComputeHeatFluxAverJumpBase(parameters),
    _heat_flux_e(getMaterialPropertyByName<RealVectorValue>(_base_name_e + "heat_flux")),
    _heat_flux_n(getNeighborMaterialPropertyByName<RealVectorValue>(_base_name_n + "heat_flux"))
    
{
}

void
ComputeHeatFluxAverJumpLocal::computeQpFluxAverJump()
{
  // First get the global version of average and jump
  computeGlobalFluxAverJump();
  // Then compute the local version of the average
  computeLocalHeatFluxAver();
}
