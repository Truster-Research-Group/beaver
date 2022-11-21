//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "InterfaceKernel.h"
#include "DerivativeMaterialInterface.h"

/// Base class for implementing Variational Multiscale Nitsche (VMN)
/// Thermal (T) mechanics weak interface coupling models in 1D, 2D, and 3D.
/// Constructed in the spirit of the "Heat Diffusion" kernel.
///
/// This class provides common input properties and helper methods,
/// most of the math has to be done in the subclasses
///
class VMNTInterfaceKernelBase
  : public DerivativeMaterialInterface<InterfaceKernel>
{
public:
  static InputParameters validParams();
  VMNTInterfaceKernelBase(const InputParameters & parameters);

protected:

  Real computeQpResidual(Moose::DGResidualType type) override;
  Real computeQpJacobian(Moose::DGJacobianType type) override;
  
  /// Stability/penalty term for residual
  virtual Real computeResiStab(const Moose::DGResidualType type) const = 0;
  /// Consistency term for residual
  virtual Real computeResiCons(const Moose::DGResidualType type) const = 0;
  /// Symmetrizing/adjoint term for residual
  virtual Real computeResiSymm(const Moose::DGResidualType type) const = 0;
  /// Flux/penalty term for residual
  virtual Real computeResiFlux(const Moose::DGResidualType type) const = 0;
  
  /// Stability/penalty term for Jacobian
  virtual Real computeJacoStab(const Moose::DGJacobianType type) const = 0;
  /// Consistency term for Jacobian
  virtual Real computeJacoCons(const Moose::DGJacobianType type) const = 0;
  /// Symmetrizing/adjoint term for Jacobian
  virtual Real computeJacoSymm(const Moose::DGJacobianType type) const = 0;
  /// Flux/penalty term for Jacobian
  virtual Real computeJacoFlux(const Moose::DGJacobianType type) const = 0;
  /// Damage term for Jacobian
  virtual Real computeJacoDebo(const Moose::DGJacobianType type) const = 0;

protected:

  /// Prepend to the material properties
  const std::string _base_name;

  /// Symmetrizing parameter, called theta or beta in some papers
  /// 0=incomplete IIPG, -1=symmetric SIPG, +1=non-symmetric NIPG
  const int _nis_flag;

  /// If true use flux penalty term
  const bool _use_flux_penalty;

};
