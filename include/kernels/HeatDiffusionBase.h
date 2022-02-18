//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Kernel.h"
#include "DerivativeMaterialInterface.h"

/// Base class of the "Heat Diffusion" kernel system
///
/// This class provides a common structure for a modular diffusion
/// system
///
/// This class provides common input properties and helper methods,
/// most of the math has to be done in the subclasses
///
class HeatDiffusionBase
  : public DerivativeMaterialInterface<Kernel>
{
public:
  static InputParameters validParams();
  HeatDiffusionBase(const InputParameters & parameters);

protected:
  /// Override residual calculation to provide for element-level averaging
  virtual void computeResidual() override;
  /// Override  on-diagonal Jacobian to provide for element-level averaging
  virtual void computeJacobian() override;

  /// Override with any initial setup for the residual
  virtual void precalculateResidual() override;

  /// Override with any initial setup for the kernel
  virtual void precalculateJacobian() override;

protected:
  /// Prepend to the material properties
  const std::string _base_name;

};
