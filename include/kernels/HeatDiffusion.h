//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "HeatDiffusionBase.h"

/// Enforce heat diffusion with a linear formulation
///
/// This kernel should be used with the "ComputeHeatFlux" heat flux system
/// and the "ComputeTempGrad" system for thermal gradients.
///
class HeatDiffusion : public HeatDiffusionBase
{
public:
  static InputParameters validParams();
  HeatDiffusion(const InputParameters & parameters);

protected:
  /// Implement the R^{\alpha}=\int_{V}Q_{i}\phi_{,i}^{\alpha}dV residual
  virtual Real computeQpResidual() override;
  /// On diagonal Jacobian, only involves the heat diffusion kernel
  virtual Real computeQpJacobian() override;

private:
  /// None

protected:
  /// The heat flux vector
  const MaterialProperty<RealVectorValue> & _Qvec; // analogy: _stress
  /// The derivative of the Q heat flux with respect to the
  /// temperature
  const MaterialProperty<RankTwoTensor> & _dQdT; // analogy: _Jacobian_mult
};
