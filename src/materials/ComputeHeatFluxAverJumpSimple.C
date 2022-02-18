//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeHeatFluxAverJumpSimple.h"

registerMooseObject("BeaverApp", ComputeHeatFluxAverJumpSimple);

InputParameters
ComputeHeatFluxAverJumpSimple::validParams()
{
  InputParameters params = ComputeHeatFluxAverJumpLocal::validParams();
  params.addClassDescription("Use a simple average for the heat flux.");

  return params;
}

ComputeHeatFluxAverJumpSimple::ComputeHeatFluxAverJumpSimple(
    const InputParameters & parameters)
  : ComputeHeatFluxAverJumpLocal(parameters)
{
}

void
ComputeHeatFluxAverJumpSimple::computeGlobalFluxAverJump()
{

  // computing the heat flux average
  _heatflux_aver_global[_qp] = (0.5 * _heat_flux_n[_qp] + 0.5 * _heat_flux_e[_qp]) * _normals[_qp];
  // computing the heat flux jump
  _heatflux_jump_global[_qp] = (_heat_flux_n[_qp] - _heat_flux_e[_qp]) * _normals[_qp];

}

void
ComputeHeatFluxAverJumpSimple::computeLocalHeatFluxAver()
{
  _interface_heatflux_aver[_qp] = _heatflux_aver_global[_qp];
}

