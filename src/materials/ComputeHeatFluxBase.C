//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeHeatFluxBase.h"

InputParameters
ComputeHeatFluxBase::validParams()
{
  InputParameters params = Material::validParams();

  params.addParam<std::string>("base_name", "Material property base name");

  return params;
}

ComputeHeatFluxBase::ComputeHeatFluxBase(const InputParameters & parameters)
  : Material(parameters),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _heat_flux(declareProperty<RealVectorValue>(_base_name + "heat_flux")),
    _diff_scalar(declareProperty<Real>(_base_name + "diff_scalar"))
    // _diff_tensor(declareProperty<RankTwoTensor>(_base_name + "diff_tensor"))
{
}

void
ComputeHeatFluxBase::initQpStatefulProperties()
{
  // Actually no need to zero out the stresses as they aren't stateful (yet)
}

void
ComputeHeatFluxBase::computeQpProperties()
{
  computeQpFluxUpdate();
}
