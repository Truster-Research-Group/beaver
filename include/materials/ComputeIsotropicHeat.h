//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ComputeHeatFlux.h"

/// Isotropic Heat Diffusion
///
class ComputeIsotropicHeat : public ComputeHeatFlux
{
public:
  static InputParameters validParams();
  ComputeIsotropicHeat(const InputParameters & parameters);

protected:

  /// Actual stress/Jacobian update
  virtual void computeQpHeatFlux() override;

protected:
  const MaterialProperty<Real> & _D;
};
