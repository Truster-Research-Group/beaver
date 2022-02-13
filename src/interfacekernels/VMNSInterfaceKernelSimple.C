//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "VMNSInterfaceKernelSimple.h"

registerMooseObject("TensorMechanicsApp", VMNSInterfaceKernelSimple);

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
        getMaterialPropertyByName<RealVectorValue>(_base_name + "interface_displacement_jump"))
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
