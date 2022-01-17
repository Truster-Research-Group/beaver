//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "HeatDiffusionBase.h"

InputParameters
HeatDiffusionBase::validParams()
{
  InputParameters params = Kernel::validParams();

  params.addParam<std::string>("base_name", "Material property base name");

  return params;
}

HeatDiffusionBase::HeatDiffusionBase(const InputParameters & parameters)
  : DerivativeMaterialInterface<Kernel>(parameters),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : "")
{
}

void
HeatDiffusionBase::computeResidual()
{
  precalculateResidual();
  Kernel::computeResidual();
}

void
HeatDiffusionBase::computeJacobian()
{
  precalculateJacobian();
  Kernel::computeJacobian();
}

void
HeatDiffusionBase::precalculateResidual()
{
  // i.e. do nothing by default
}

void
HeatDiffusionBase::precalculateJacobian()
{
  // i.e. do nothing by default
}
