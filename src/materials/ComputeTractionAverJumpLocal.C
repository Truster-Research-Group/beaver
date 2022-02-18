//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeTractionAverJumpLocal.h"

InputParameters
ComputeTractionAverJumpLocal::validParams()
{
  InputParameters params = ComputeTractionAverJumpBase::validParams();
  params.addClassDescription("Compute traction avearge and jump using "
                             "material tensors from elem and neig sides.");

  return params;
}

ComputeTractionAverJumpLocal::ComputeTractionAverJumpLocal(
    const InputParameters & parameters)
  : ComputeTractionAverJumpBase(parameters),
    _pk1_stress_e(getMaterialPropertyByName<RankTwoTensor>(_base_name_e + "pk1_stress")),
    _pk1_stress_n(getNeighborMaterialPropertyByName<RankTwoTensor>(_base_name_n + "pk1_stress")),
    _pk1_jacobian_e(getMaterialPropertyByName<RankFourTensor>(_base_name_e + "pk1_jacobian")),
    _pk1_jacobian_n(getNeighborMaterialPropertyByName<RankFourTensor>(_base_name_n + "pk1_jacobian")),
    _czm_total_rotation(getMaterialPropertyByName<RankTwoTensor>(_base_name + "czm_total_rotation"))
{
}

void
ComputeTractionAverJumpLocal::computeQpTracAverJump()
{
  // First get the global version of average and jump
  computeGlobalTracAverJump();
  // Then compute the local version of the average
  computeLocalTractionAver();
}

void
ComputeTractionAverJumpLocal::obtainQpModuliTensors()
{
  // Copy the moduli tensor from each Material on the interface
  // into the two InterfaceMaterial tensor holders
  _dpk1dF_e[_qp] = _pk1_jacobian_e[_qp];
  _dpk1dF_n[_qp] = _pk1_jacobian_n[_qp];
}

void
ComputeTractionAverJumpLocal::computeLocalTractionAver()
{
  _interface_traction_aver[_qp] =
      _czm_total_rotation[_qp].transpose() * _traction_aver_global[_qp];
}
