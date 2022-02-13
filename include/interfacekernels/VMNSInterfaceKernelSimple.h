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
  
  /// Stability/penalty term for Jacobian
  Real computeJacoStab(const unsigned int & component_j,
                       const Moose::DGJacobianType & type) const override;

  /// Input property to allow user modifying penalty parameter
  const Real _pen_scale;
  
  /// the displacement jump in global and interface coordiantes;
  ///@{
  const MaterialProperty<RealVectorValue> & _displacement_jump_global;
  const MaterialProperty<RealVectorValue> & _interface_displacement_jump;
  ///@}

private:

  /// hard code the penalty for now
  const Real pencoef = 1000.0; 
};
