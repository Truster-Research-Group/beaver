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
        getMaterialPropertyByName<Real>(_base_name + "interface_heatflux_aver")),
    _dQdT_e(getMaterialPropertyByName<RankTwoTensor>(_base_name + "dQdT_e")),
    _dQdT_n(getMaterialPropertyByName<RankTwoTensor>(_base_name + "dQdT_n"))
{
}


Real
VMNTInterfaceKernelSimple::computeResiStab(Moose::DGResidualType type) const
{

  const unsigned int elem_b_order = _var.order();
  const double h_elem =
      _current_elem_volume / _current_side_volume * 1. / Utility::pow<2>(elem_b_order);

  Real r = (_pen_scale * pencoef / h_elem) * _temp_jump_global[_qp];

  switch (type)
  {
    // comment
    case Moose::Element:
      r *= - _test[_i][_qp];
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

  Real r = _heatflux_aver_global[_qp];

  switch (type)
  {
    // comment
    case Moose::Element:
      r *= - _test[_i][_qp];
      break;
    case Moose::Neighbor:
      r *= _test_neighbor[_i][_qp];
      break;
  }
  return r;
}

Real
VMNTInterfaceKernelSimple::computeResiSymm(Moose::DGResidualType type) const
{

  Real r = - _temp_jump_global[_qp];

  switch (type)
  {
    case Moose::Element:
      r *= _nis_flag * 0.5 * (_dQdT_e[_qp] * _grad_test[_i][_qp]) *
           _normals[_qp];
      break;

    case Moose::Neighbor:
      r *= _nis_flag * 0.5 * (_dQdT_n[_qp] * _grad_test_neighbor[_i][_qp]) *
           _normals[_qp];
      break;
  }

  return r;
}

Real
VMNTInterfaceKernelSimple::computeResiFlux(Moose::DGResidualType type) const
{

  const unsigned int elem_b_order = _var.order();
  const double h_elem =
      _current_elem_volume / _current_side_volume * 1. / Utility::pow<2>(elem_b_order);

  Real r = - (_pen_scale * h_elem / pencoef) * _heatflux_jump_global[_qp];

  switch (type)
  {
    case Moose::Element:
      r *= - (_dQdT_e[_qp] * _grad_test[_i][_qp]) *
           _normals[_qp];
      break;

    case Moose::Neighbor:
      r *= (_dQdT_n[_qp] * _grad_test_neighbor[_i][_qp]) *
           _normals[_qp];
      break;
  }

  return r;
}

Real
VMNTInterfaceKernelSimple::computeJacoStab(
    const Moose::DGJacobianType type) const
{
  const unsigned int elem_b_order = _var.order();
  const double h_elem =
      _current_elem_volume / _current_side_volume * 1. / Utility::pow<2>(elem_b_order);

  Real jac = (_pen_scale * pencoef / h_elem);

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
  Real jac = 0.0;

  switch (type)
  {
    case Moose::ElementElement:
      jac -= 0.5 * (_dQdT_e[_qp] * _grad_phi[_j][_qp]) * _normals[_qp] * 
           _test[_i][_qp];
      break;

    case Moose::ElementNeighbor:
      jac -= 0.5 * (_dQdT_n[_qp] * _grad_phi_neighbor[_j][_qp]) * _normals[_qp] * 
           _test[_i][_qp];
      break;

    case Moose::NeighborElement:
      jac += 0.5 * (_dQdT_e[_qp] * _grad_phi[_j][_qp]) * _normals[_qp] * 
           _test_neighbor[_i][_qp];
      break;

    case Moose::NeighborNeighbor:
      jac += 0.5 * (_dQdT_n[_qp] * _grad_phi_neighbor[_j][_qp]) * _normals[_qp] *
           _test_neighbor[_i][_qp];
      break;
  }

  return jac;
}

Real
VMNTInterfaceKernelSimple::computeJacoSymm(
    const Moose::DGJacobianType type) const
{
  Real jac = 0;

  switch (type)
  {
    case Moose::ElementElement:
      jac += _nis_flag * 0.5 * _phi[_j][_qp] *
            (_dQdT_e[_qp] * _grad_test[_i][_qp]) * _normals[_qp];
      break;

    case Moose::ElementNeighbor:
      jac += _nis_flag * 0.5 * -_phi_neighbor[_j][_qp] *
            (_dQdT_e[_qp] * _grad_test[_i][_qp]) * _normals[_qp];
      break;

    case Moose::NeighborElement:
      jac += _nis_flag * 0.5 * _phi[_j][_qp] *
            (_dQdT_n[_qp] * _grad_test_neighbor[_i][_qp]) * _normals[_qp];
      break;

    case Moose::NeighborNeighbor:
      jac += _nis_flag * 0.5 * -_phi_neighbor[_j][_qp] *
            (_dQdT_n[_qp] * _grad_test_neighbor[_i][_qp]) * _normals[_qp];
      break;
  }

  return jac;
}

Real
VMNTInterfaceKernelSimple::computeJacoFlux(
    const Moose::DGJacobianType type) const
{
  const unsigned int elem_b_order = _var.order();
  const double h_elem =
      _current_elem_volume / _current_side_volume * 1. / Utility::pow<2>(elem_b_order);

  Real jac = - (_pen_scale * h_elem / pencoef);

  switch (type)
  {
    case Moose::ElementElement:
      jac *= ((_dQdT_e[_qp] * _grad_phi[_j][_qp]) * _normals[_qp]) *
            ((_dQdT_e[_qp] * _grad_test[_i][_qp]) * _normals[_qp]);
      break;

    case Moose::ElementNeighbor:
      jac *= - ((_dQdT_n[_qp] * _grad_phi_neighbor[_j][_qp]) * _normals[_qp]) *
            ((_dQdT_e[_qp] * _grad_test[_i][_qp]) * _normals[_qp]);
      break;

    case Moose::NeighborElement:
      jac *= - ((_dQdT_e[_qp] * _grad_phi[_j][_qp]) * _normals[_qp]) *
            ((_dQdT_n[_qp] * _grad_test_neighbor[_i][_qp]) * _normals[_qp]);
      break;

    case Moose::NeighborNeighbor:
      jac *= ((_dQdT_n[_qp] * _grad_phi_neighbor[_j][_qp]) * _normals[_qp]) *
            ((_dQdT_n[_qp] * _grad_test_neighbor[_i][_qp]) * _normals[_qp]);
      break;
  }

  return jac;
}

Real
VMNTInterfaceKernelSimple::computeJacoDebo(
    const Moose::DGJacobianType type) const
{
  return 0.0;
}
