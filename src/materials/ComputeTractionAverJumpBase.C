//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeTractionAverJumpBase.h"

InputParameters
ComputeTractionAverJumpBase::validParams()
{
  InputParameters params = InterfaceMaterial::validParams();
  params.addClassDescription("Base class used to compute the traction "
                             "average and jump across an interface.");
  params.suppressParameter<bool>("use_displaced_mesh");
  params.addParam<std::string>("base_name", "Material property base name");
  params.addParam<std::string>("base_name_e", "Base name of traction on element side");
  params.addParam<std::string>("base_name_n", "Base name of traction on neighbor side");

  return params;
}

ComputeTractionAverJumpBase::ComputeTractionAverJumpBase(const InputParameters & parameters)
  : InterfaceMaterial(parameters),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _base_name_e(isParamValid("base_name_e") ? getParam<std::string>("base_name_e") + "_" : ""),
    _base_name_n(isParamValid("base_name_n") ? getParam<std::string>("base_name_n") + "_" : ""),
    _normals(_assembly.normals()),
    _traction_aver_global(
        declarePropertyByName<RealVectorValue>(_base_name + "traction_aver_global")),
    _traction_jump_global(
        declarePropertyByName<RealVectorValue>(_base_name + "traction_jump_global")),
    _interface_traction_aver(
        declarePropertyByName<RealVectorValue>(_base_name + "interface_traction_aver")),
    _dpk1dF_e(declareProperty<RankFourTensor>(_base_name + "dpk1dF_e")),
    _dpk1dF_n(declareProperty<RankFourTensor>(_base_name + "dpk1dF_n"))
{
}

void
ComputeTractionAverJumpBase::initQpStatefulProperties()
{
  // required to promote _interface_traction_aver to stateful in case someone needs it
  _interface_traction_aver[_qp] = 0;
}

void
ComputeTractionAverJumpBase::computeQpProperties()
{
  computeQpTracAverJump();
  obtainQpModuliTensors();
}
