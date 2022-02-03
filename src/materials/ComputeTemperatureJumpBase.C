//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeTemperatureJumpBase.h"

InputParameters
ComputeTemperatureJumpBase::validParams()
{
  InputParameters params = InterfaceMaterial::validParams();
  params.addClassDescription("Base class used to compute the temperature jump across an interface.");
  params.addRequiredCoupledVar("temp",
                               "The temperature appropriate for the simulation");
  params.suppressParameter<bool>("use_displaced_mesh");
  params.addParam<std::string>("base_name", "Material property base name");

  return params;
}

ComputeTemperatureJumpBase::ComputeTemperatureJumpBase(const InputParameters & parameters)
  : InterfaceMaterial(parameters),
    _base_name(isParamValid("base_name") && !getParam<std::string>("base_name").empty()
                   ? getParam<std::string>("base_name") + "_"
                   : ""),
    _normals(_assembly.normals()),
    _temp(coupledValue("temp")),
    _temp_neighbor(coupledNeighborValue("temp")),
    _temp_jump_global(
        declarePropertyByName<Real>(_base_name + "temp_jump_global")),
    _interface_temp_jump(
        declarePropertyByName<Real>(_base_name + "interface_temp_jump"))
{
}

void
ComputeTemperatureJumpBase::initQpStatefulProperties()
{
  // required to promote _interface_temp_jump to stateful in case someone needs it
  _interface_temp_jump[_qp] = 0;
}

void
ComputeTemperatureJumpBase::computeQpProperties()
{

  // computing the temperature jump
  _temp_jump_global[_qp] = _temp_neighbor[_qp] - _temp[_qp];

  computeLocalTemperatureJump();
}
