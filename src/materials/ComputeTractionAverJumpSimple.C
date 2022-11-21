//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeTractionAverJumpSimple.h"

registerMooseObject("BeaverApp", ComputeTractionAverJumpSimple);

InputParameters
ComputeTractionAverJumpSimple::validParams()
{
  InputParameters params = ComputeTractionAverJumpLocal::validParams();
  params.addClassDescription("Use a simple average for the traction.");

  return params;
}

ComputeTractionAverJumpSimple::ComputeTractionAverJumpSimple(
    const InputParameters & parameters)
  : ComputeTractionAverJumpLocal(parameters)
{
}

void
ComputeTractionAverJumpSimple::computeGlobalTracAverJump()
{

  // computing the traction average
  _traction_aver_global[_qp] = (0.5 * _pk1_stress_n[_qp] + 0.5 * _pk1_stress_e[_qp]) * _normals[_qp];
  // computing the traction jump
  _traction_jump_global[_qp] = (_pk1_stress_n[_qp] - _pk1_stress_e[_qp]) * _normals[_qp];

}
