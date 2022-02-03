//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "VMNTInterfaceKernelSimple.h"

registerMooseObject("BeaverApp", VMNTInterfaceKernelSimple);

InputParameters
VMNTInterfaceKernelSimple::validParams()
{
  InputParameters params = VMNTInterfaceKernelBase::validParams();

  params.addClassDescription(
      "VMNT Interface kernel to use with ComputeHeatFluxAverJumpSimple. "
      "Simple average for the heat flux.");
  params.addParam<Real>("pen_scale", 1.0, "Increase or decrease the penalty");

  return params;
}

VMNTInterfaceKernelSimple::VMNTInterfaceKernelSimple(const InputParameters & parameters)
  : VMNTInterfaceKernelBase(parameters),
    _pen_scale(getParam<Real>("pen_scale")),
    _temp_jump_global(
        getMaterialPropertyByName<Real>(_base_name + "temp_jump_global")),
    _interface_temp_jump(
        getMaterialPropertyByName<Real>(_base_name + "interface_temp_jump")),
    _heatflux_aver_global(
        getMaterialPropertyByName<Real>(_base_name + "heatflux_aver_global")),
    _heatflux_jump_global(
        getMaterialPropertyByName<Real>(_base_name + "heatflux_jump_global")),
    _interface_heatflux_aver(
        getMaterialPropertyByName<Real>(_base_name + "interface_heatflux_aver"))
{
}


Real
VMNTInterfaceKernelSimple::computeResiStab(Moose::DGResidualType type) const
{

  Real r = _pen_scale * pencoef * _temp_jump_global[_qp];

  switch (type)
  {
    // comment
    case Moose::Element:
      r *= -_test[_i][_qp];
      break;
    case Moose::Neighbor:
      r *= _test_neighbor[_i][_qp];
      break;
  }
  return r;
}

Real
VMNTInterfaceKernelSimple::computeResiCons(Moose::DGResidualType type) const
{

  return 0.0;
}

Real
VMNTInterfaceKernelSimple::computeResiSymm(Moose::DGResidualType type) const
{

  return 0.0;
}

Real
VMNTInterfaceKernelSimple::computeResiFlux(Moose::DGResidualType type) const
{

  return 0.0;
}

Real
VMNTInterfaceKernelSimple::computeJacoStab(
    const Moose::DGJacobianType type) const
{
  Real jac = _pen_scale * pencoef;
  switch (type)
  {
    case Moose::ElementElement: // Residual_sign -1  ddeltaU_ddisp sign -1;
      jac *= _test[_i][_qp] * _phi[_j][_qp];
      break;
    case Moose::ElementNeighbor: // Residual_sign -1  ddeltaU_ddisp sign 1;
      jac *= -_test[_i][_qp] * _phi_neighbor[_j][_qp];
      break;
    case Moose::NeighborElement: // Residual_sign 1  ddeltaU_ddisp sign -1;
      jac *= -_test_neighbor[_i][_qp] * _phi[_j][_qp];
      break;
    case Moose::NeighborNeighbor: // Residual_sign 1  ddeltaU_ddisp sign 1;
      jac *= _test_neighbor[_i][_qp] * _phi_neighbor[_j][_qp];
      break;
  }
  return jac;
}

Real
VMNTInterfaceKernelSimple::computeJacoCons(
    const Moose::DGJacobianType type) const
{
  return 0.0;
}

Real
VMNTInterfaceKernelSimple::computeJacoSymm(
    const Moose::DGJacobianType type) const
{
  return 0.0;
}

Real
VMNTInterfaceKernelSimple::computeJacoFlux(
    const Moose::DGJacobianType type) const
{
  return 0.0;
}

Real
VMNTInterfaceKernelSimple::computeJacoDebo(
    const Moose::DGJacobianType type) const
{
  return 0.0;
}
