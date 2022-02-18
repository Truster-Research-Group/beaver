//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ComputeHeatFluxAverJumpLocal.h"
/**
 * Compute the interface heat flux average and jump across a discontinuous interface.
 * Uses a 50/50 simple average
 */
class ComputeHeatFluxAverJumpSimple : public ComputeHeatFluxAverJumpLocal
{
public:
  static InputParameters validParams();
  ComputeHeatFluxAverJumpSimple(const InputParameters & parameters);

protected:

  /// Calculate the heat flux average and jump in global coordinates.
  void computeGlobalFluxAverJump() override;

  /// Calculate the heat flux average in interface coordinates.
  void computeLocalHeatFluxAver() override;

};
