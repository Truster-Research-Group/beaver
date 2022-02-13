//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ComputeTractionAverJumpLocal.h"
/**
 * Compute the interface traction average and jump across a discontinuous interface.
 * Uses a 50/50 simple average
 */
class ComputeTractionAverJumpSimple : public ComputeTractionAverJumpLocal
{
public:
  static InputParameters validParams();
  ComputeTractionAverJumpSimple(const InputParameters & parameters);

protected:

  /// Calculate the traction average and jump in global coordinates.
  void computeGlobalTracAverJump() override;

};
