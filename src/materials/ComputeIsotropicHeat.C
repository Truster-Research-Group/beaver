//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeIsotropicHeat.h"

registerMooseObject("BeaverApp", ComputeIsotropicHeat);

InputParameters
ComputeIsotropicHeat::validParams()
{
  InputParameters params = ComputeHeatFlux::validParams();

  params.addParam<MaterialPropertyName>(
      "diffusivity", "D", "The diffusivity value or material property");

  return params;
}

ComputeIsotropicHeat::ComputeIsotropicHeat(const InputParameters & parameters)
  : ComputeHeatFlux(parameters),
    _D(getMaterialProperty<Real>(getParam<MaterialPropertyName>("diffusivity")))
{
}

void
ComputeIsotropicHeat::computeQpHeatFlux()
{
  // Make the diffusivity tensor
  _diff_tensor[_qp] = _D[_qp] * RankTwoTensor::Identity();

  // The heat flux update is linear
  _heat_flux[_qp] = _diff_tensor[_qp] * _grad_t[_qp];
}
