//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "VMNSInterfaceKernelBase.h"

/// DG cohesive zone model kernel for the small strain formulation.
/// This kernel assummes the traction sepration law only depends from the
/// displacement jump. One kernel is required for each displacement component
class VMNSInterfaceKernelSimple : public VMNSInterfaceKernelBase
{
public:
  static InputParameters validParams();
  VMNSInterfaceKernelSimple(const InputParameters & parameters);

protected:

  /// Stability/penalty term for residual
  Real computeResiStab(const Moose::DGResidualType type) const override;
  /// Consistency term for residual
  Real computeResiCons(const Moose::DGResidualType type) const override;
  /// Symmetrizing/adjoint term for residual
  Real computeResiSymm(const Moose::DGResidualType type) const override;
  /// Flux/penalty term for residual
  Real computeResiFlux(const Moose::DGResidualType type) const override;
  
  /// Stability/penalty term for Jacobian
  Real computeJacoStab(const unsigned int & component_j,
                       const Moose::DGJacobianType & type) const override;
  /// Consistency term for Jacobian
  Real computeJacoCons(const unsigned int & component_j,
                       const Moose::DGJacobianType & type) const override;
  /// Symmetrizing/adjoint term for Jacobian
  Real computeJacoSymm(const unsigned int & component_j,
                       const Moose::DGJacobianType & type) const override;
  /// Flux/penalty term for Jacobian
  Real computeJacoFlux(const unsigned int & component_j,
                       const Moose::DGJacobianType & type) const override;
  /// Damage term for Jacobian
  Real computeJacoDebo(const unsigned int & component_j,
                       const Moose::DGJacobianType & type) const override;

  /// Calculate the full test gradient (could later be modified for stabilization)
  ///@{
  RankTwoTensor testGrad_e(const unsigned int i) const;
  RankTwoTensor testGrad_n(const unsigned int i) const;
  ///@}

  /// Compute the stabilized trial function gradient tensor
  ///@{
  RankTwoTensor trialGrad_e(const unsigned int k) const;
  RankTwoTensor trialGrad_n(const unsigned int k) const;
  ///@}

private:
  /// Incremental traction vector from material moduli
  ///@{
  RealVectorValue dTract_e(const RankTwoTensor & grad_shape) const;
  RealVectorValue dTract_n(const RankTwoTensor & grad_shape) const;
  ///@}

protected:
  /// Input property to allow user modifying penalty parameter
  const Real _pen_scale;
  
  /// the displacement jump in global and interface coordiantes;
  ///@{
  const MaterialProperty<RealVectorValue> & _displacement_jump_global;
  const MaterialProperty<RealVectorValue> & _interface_displacement_jump;
  ///@}

  /// the traction average and jump in global coordinates
  ///@{
  const MaterialProperty<RealVectorValue> & _traction_aver_global;
  const MaterialProperty<RealVectorValue> & _traction_jump_global;
  ///@}

  /// the traction average in interface coordinates 
  const MaterialProperty<RealVectorValue> & _interface_traction_aver;

  /// The derivative of 1st PK stress wrt deformation gradient
  ///@{
  const MaterialProperty<RankFourTensor> & _dpk1dF_e;
  const MaterialProperty<RankFourTensor> & _dpk1dF_n;
  ///@}

private:

  /// hard code the penalty for now
  const Real pencoef = 1000.0; 
};
