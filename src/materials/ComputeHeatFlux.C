//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeHeatFlux.h"

InputParameters
ComputeHeatFlux::validParams()
{
  InputParameters params = ComputeHeatFluxBase::validParams();

  params.addClassDescription("Heat flux standard update");

  return params;
}

ComputeHeatFlux::ComputeHeatFlux(const InputParameters & parameters)
  : ComputeHeatFluxBase(parameters),
    _grad_t(getMaterialPropertyByName<RealVectorValue>(_base_name + "grad_t"))
{
}

void
ComputeHeatFlux::computeQpFluxUpdate()
{
  computeQpHeatFlux();
}
