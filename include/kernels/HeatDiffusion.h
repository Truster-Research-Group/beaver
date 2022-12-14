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

protected:
  /// The heat flux vector, 'get' from material _heat_flux; TM-analogy: _stress
  const MaterialProperty<RealVectorValue> & _Qvec;
  /// The derivative of the Q heat flux with respect to the
  /// temperature, 'get' from material _diff_tensor; TM-analogy: _Jacobian_mult
  const MaterialProperty<RankTwoTensor> & _dQdT;
};
