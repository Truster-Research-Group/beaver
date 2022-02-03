//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ComputeTemperatureJumpBase.h"
/**
 * Compute the interface temperature jump across a discontinuous interface.
 */
class ComputeTemperatureJumpLocal : public ComputeTemperatureJumpBase
{
public:
  static InputParameters validParams();
  ComputeTemperatureJumpLocal(const InputParameters & parameters);

protected:
  /// compute the total displacement jump in interface coordinates
  void computeLocalTemperatureJump() override;
};
