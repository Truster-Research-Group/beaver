//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeHeatFluxAverJumpBase.h"

InputParameters
ComputeHeatFluxAverJumpBase::validParams()
{
  InputParameters params = InterfaceMaterial::validParams();
  params.addClassDescription("Base class used to compute the heat flux "
                             "average and jump across an interface.");
  params.suppressParameter<bool>("use_displaced_mesh");
  params.addParam<std::string>("base_name", "Material property base name");
  params.addParam<std::string>("base_name_e", "Base name of heat flux on element side");
  params.addParam<std::string>("base_name_n", "Base name of heat flux on neighbor side");

  return params;
}

ComputeHeatFluxAverJumpBase::ComputeHeatFluxAverJumpBase(const InputParameters & parameters)
  : InterfaceMaterial(parameters),
    _base_name(isParamValid("base_name") && !getParam<std::string>("base_name").empty()
                   ? getParam<std::string>("base_name") + "_"
                   : ""),
    _base_name_e(isParamValid("base_name_e") && !getParam<std::string>("base_name_e").empty()
                   ? getParam<std::string>("base_name_e") + "_"
                   : ""),
    _base_name_n(isParamValid("base_name_n") && !getParam<std::string>("base_name_n").empty()
                   ? getParam<std::string>("base_name_n") + "_"
                   : ""),
    _normals(_assembly.normals()),
    _heatflux_aver_global(
        declarePropertyByName<Real>(_base_name + "heatflux_aver_global")),
    _heatflux_jump_global(
        declarePropertyByName<Real>(_base_name + "heatflux_jump_global")),
    _interface_heatflux_aver(
        declarePropertyByName<Real>(_base_name + "interface_heatflux_aver"))
{
}

void
ComputeHeatFluxAverJumpBase::initQpStatefulProperties()
{
  // required to promote _interface_heatflux_aver to stateful in case someone needs it
  _interface_heatflux_aver[_qp] = 0;
}

void
ComputeHeatFluxAverJumpBase::computeQpProperties()
{
  computeQpFluxAverJump();
}
