//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeTemperatureJumpLocal.h"

registerMooseObject("BeaverApp", ComputeTemperatureJumpLocal);

InputParameters
ComputeTemperatureJumpLocal::validParams()
{
  InputParameters params = ComputeTemperatureJumpBase::validParams();
  params.addClassDescription("Compute the total temperature jump across an interface"
                             " in localcoordinates");

  return params;
}

ComputeTemperatureJumpLocal::ComputeTemperatureJumpLocal(
    const InputParameters & parameters)
  : ComputeTemperatureJumpBase(parameters)
{
}

void
ComputeTemperatureJumpLocal::computeLocalTemperatureJump()
{
  _interface_temp_jump[_qp] = _temp_jump_global[_qp];
}
