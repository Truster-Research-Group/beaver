//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "HeatDiffusion.h"

registerMooseObject("BeaverApp", HeatDiffusion);

InputParameters
HeatDiffusion::validParams()
{
  InputParameters params = HeatDiffusionBase::validParams();

  params.addClassDescription("Linear heat diffusion kernel");

  // This kernel requires use_displaced_mesh to be off
  params.suppressParameter<bool>("use_displaced_mesh");

  return params;
}

HeatDiffusion::HeatDiffusion(const InputParameters & parameters)
  : HeatDiffusionBase(parameters),
    _Qvec(getMaterialPropertyByName<RealVectorValue>(_base_name + "heat_flux")),
    _dQdTs(getMaterialPropertyByName<Real>(_base_name + "diff_scalar"))
  //  _dQdT(getMaterialPropertyByName<RankTwoTensor>(_base_name + "diff_tensor"))
{
}

Real
HeatDiffusion::computeQpResidual()
{
  return _Qvec[_qp] * _grad_test[_i][_qp];
}

Real
HeatDiffusion::computeQpJacobian()
{
  return _dQdTs[_qp] * _grad_phi[_j][_qp] * _grad_test[_i][_qp];
  // return (_dQdT[_qp] * _grad_phi[_j][_qp]) * _grad_test[_i][_qp];
}
