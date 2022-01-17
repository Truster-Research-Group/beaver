//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeTempGradBase.h"
#include "MooseMesh.h"
#include "Assembly.h"

InputParameters
ComputeTempGradBase::validParams()
{
  InputParameters params = Material::validParams();
  params.addRequiredCoupledVar(
      "temp",
      "The temperature appropriate for the simulation");
  params.addParam<std::string>("base_name",
                               "Optional parameter that allows the user to define "
                               "multiple thermal material systems on the same "
                               "block, i.e. for multiple phases");
  return params;
}

ComputeTempGradBase::ComputeTempGradBase(const InputParameters & parameters)
  : DerivativeMaterialInterface<Material>(parameters),
    _temp(coupledValue("temp")),
    _grad_temp(coupledGradient("temp")),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _grad_T(declareProperty<RealVectorValue>(_base_name + "grad_T")),
    _hold_T(declareProperty<RealVectorValue>(_base_name + "hold_T"))
{
}

void
ComputeTempGradBase::initQpStatefulProperties()
{
  _grad_T[_qp].zero();
  _hold_T[_qp].zero();
}
