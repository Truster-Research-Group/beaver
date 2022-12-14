//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "VMNTInterfaceKernelBase.h"

/// VMNT Interface Kernel for heat diffusion using simple average
/// for the flux. No damage is implemented.
class VMNTInterfaceKernelSimple : public VMNTInterfaceKernelBase
{
public:
  static InputParameters validParams();
  VMNTInterfaceKernelSimple(const InputParameters & parameters);

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
  Real computeJacoStab(const Moose::DGJacobianType type) const override;
  /// Consistency term for Jacobian
  Real computeJacoCons(const Moose::DGJacobianType type) const override;
  /// Symmetrizing/adjoint term for Jacobian
  Real computeJacoSymm(const Moose::DGJacobianType type) const override;
  /// Flux/penalty term for Jacobian
  Real computeJacoFlux(const Moose::DGJacobianType type) const override;
  /// Damage term for Jacobian
  Real computeJacoDebo(const Moose::DGJacobianType type) const override;

  /// Input property to allow user modifying penalty parameter
  const Real _pen_scale;

  /// the temperature jump in global and interface coordiantes;
  /// TM-analogy: _displacement_jump_global, _interface_displacement_jump
  ///@{
  const MaterialProperty<Real> & _temp_jump_global;
  const MaterialProperty<Real> & _interface_temp_jump;
  ///@}

  /// the heat flux average and jump in global coordinates; 
  /// TM-analogy: _traction_global
  ///@{
  const MaterialProperty<Real> & _heatflux_aver_global;
  const MaterialProperty<Real> & _heatflux_jump_global;
  ///@}

  /// the heat flux average in interface coordinates; 
  /// TM-analogy: _interface_traction
  const MaterialProperty<Real> & _interface_heatflux_aver;

  /// The derivative of Q wrt the thermal gradient.
  ///@{
  const MaterialProperty<RankTwoTensor> & _dQdT_e;
  const MaterialProperty<RankTwoTensor> & _dQdT_n;
  ///@}

private:

  /// hard code the penalty for now
  const Real pencoef = 1000.0; 
};
