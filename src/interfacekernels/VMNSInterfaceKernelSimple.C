//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "VMNSInterfaceKernelSimple.h"

registerMooseObject("BeaverApp", VMNSInterfaceKernelSimple);

InputParameters
VMNSInterfaceKernelSimple::validParams()
{
  InputParameters params = VMNSInterfaceKernelBase::validParams();

  params.addClassDescription(
      "VMNS Interface kernel to use with ComputeTractionAverJumpSimple. "
      "Simple average for the traction vector.");
  params.addParam<Real>("pen_scale", 1.0, "Increase or decrease the penalty");

  return params;
}

VMNSInterfaceKernelSimple::VMNSInterfaceKernelSimple(const InputParameters & parameters)
  : VMNSInterfaceKernelBase(parameters),
    _pen_scale(getParam<Real>("pen_scale")),
    _displacement_jump_global(
        getMaterialPropertyByName<RealVectorValue>(_base_name + "displacement_jump_global")),
    _interface_displacement_jump(
        getMaterialPropertyByName<RealVectorValue>(_base_name + "interface_displacement_jump")),
    _traction_aver_global(
        getMaterialPropertyByName<RealVectorValue>(_base_name + "traction_aver_global")),
    _traction_jump_global(
        getMaterialPropertyByName<RealVectorValue>(_base_name + "traction_jump_global")),
    _interface_traction_aver(
        getMaterialPropertyByName<RealVectorValue>(_base_name + "interface_traction_aver")),
    _dpk1dF_e(getMaterialPropertyByName<RankFourTensor>(_base_name + "dpk1dF_e")),
    _dpk1dF_n(getMaterialPropertyByName<RankFourTensor>(_base_name + "dpk1dF_n"))
{
}

Real
VMNSInterfaceKernelSimple::computeResiStab(const Moose::DGResidualType type) const
{

  const unsigned int elem_b_order = _var.order();
  const double h_elem =
      _current_elem_volume / _current_side_volume * 1. / Utility::pow<2>(elem_b_order);

  // Make the displacement stability penalty tensor
  const RankTwoTensor I = (pencoef  / h_elem) * RankTwoTensor::Identity();
  const RealVectorValue pen_disp = _pen_scale * I * _displacement_jump_global[_qp];
  Real r = pen_disp(_component);

  switch (type)
  {
    // [test_secondary-test_primary]*T where T represents the traction.
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
VMNSInterfaceKernelSimple::computeResiCons(const Moose::DGResidualType type) const
{

  Real r = _traction_aver_global[_qp](_component);

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
VMNSInterfaceKernelSimple::computeResiSymm(const Moose::DGResidualType type) const
{
  Real r = 0.0;

  return r;
}

Real
VMNSInterfaceKernelSimple::computeResiFlux(const Moose::DGResidualType type) const
{
  Real r = 0.0;

  return r;
}

Real
VMNSInterfaceKernelSimple::computeJacoStab(
    const unsigned int & component_j, const Moose::DGJacobianType & type) const
{
  const unsigned int elem_b_order = _var.order();
  const double h_elem =
      _current_elem_volume / _current_side_volume * 1. / Utility::pow<2>(elem_b_order);
  
  // Make the displacement stability penalty tensor
  const RankTwoTensor I = (pencoef  / h_elem) * RankTwoTensor::Identity();
  Real jac = _pen_scale * I(_component, component_j);
  
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
VMNSInterfaceKernelSimple::computeJacoCons(
    const unsigned int & component_j, const Moose::DGJacobianType & type) const
{

  Real jac = 0.0;
  Real r = 0.0;
  RealVectorValue d_traction;

  switch (type)
  {
    case Moose::ElementElement:
    
      d_traction = 0.5 * dTract_e( trialGrad_e(component_j) );
      r = d_traction(_component);
      jac -= r * _test[_i][_qp];
      break;

    case Moose::ElementNeighbor:
      d_traction = 0.5 * dTract_n( trialGrad_n(component_j) );
      r = d_traction(_component);
      jac -= r * _test[_i][_qp];
      break;

    case Moose::NeighborElement:
      d_traction = 0.5 * dTract_e( trialGrad_e(component_j) );
      r = d_traction(_component);
      jac += r * _test_neighbor[_i][_qp];
      break;

    case Moose::NeighborNeighbor:
      d_traction = 0.5 * dTract_n( trialGrad_n(component_j) );
      r = d_traction(_component);
      jac += r * _test_neighbor[_i][_qp];
      break;
  }
  
  return jac;
}

Real
VMNSInterfaceKernelSimple::computeJacoSymm(
    const unsigned int & component_j, const Moose::DGJacobianType & type) const
{
  Real jac = 0.0;
  
  return jac;
}

Real
VMNSInterfaceKernelSimple::computeJacoFlux(
    const unsigned int & component_j, const Moose::DGJacobianType & type) const
{
  Real jac = 0.0;
  
  return jac;
}

Real
VMNSInterfaceKernelSimple::computeJacoDebo(
    const unsigned int & component_j, const Moose::DGJacobianType & type) const
{
  Real jac = 0.0;
  
  return jac;
}

/// Lots of helpers to make the above equations simpler
RankTwoTensor
VMNSInterfaceKernelSimple::testGrad_e(const unsigned int i) const
{
  return gradOp(i, _grad_test[_i][_qp]);
}

RankTwoTensor
VMNSInterfaceKernelSimple::testGrad_n(const unsigned int i) const
{
  return gradOp(i, _grad_test_neighbor[_i][_qp]);
}

RankTwoTensor
VMNSInterfaceKernelSimple::trialGrad_e(const unsigned int k) const
{
  return gradOp(k, _grad_phi[_j][_qp]);
}

RankTwoTensor
VMNSInterfaceKernelSimple::trialGrad_n(const unsigned int k) const
{
  return gradOp(k, _grad_phi_neighbor[_j][_qp]);
}

RealVectorValue
VMNSInterfaceKernelSimple::dTract_e(const RankTwoTensor & grad_shape) const
{
  return (_dpk1dF_e[_qp] * grad_shape) * _normals[_qp];
}

RealVectorValue
VMNSInterfaceKernelSimple::dTract_n(const RankTwoTensor & grad_shape) const
{
  return (_dpk1dF_n[_qp] * grad_shape) * _normals[_qp];
}