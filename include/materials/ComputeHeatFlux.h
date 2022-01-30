//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ComputeHeatFluxBase.h"

/// Native interface for providing the Heat Flux vector
///
/// This class *implements* the Heat Flux Q update, providing
///   1) "heat_flux" -- the heat flux vector
///   2) "diff_tensor" -- the derivative of Q with respect
///                           to the thermal gradient
///
class ComputeHeatFlux : public ComputeHeatFluxBase
{
public:
  static InputParameters validParams();
  ComputeHeatFlux(const InputParameters & parameters);

protected:
  /// Calculate the heat flux update
  virtual void computeQpFluxUpdate() override;
  /// Provide for the actual heat flux update
  virtual void computeQpHeatFlux() = 0;

private:
  /// None

protected:
  /// Thermal gradient
  const MaterialProperty<RealVectorValue> & _grad_t;
};
