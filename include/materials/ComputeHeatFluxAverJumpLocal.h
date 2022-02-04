//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ComputeHeatFluxAverJumpBase.h"
/**
 * Compute the interface heat flux average and jump across a discontinuous interface.
 * Does a MOOSE look-up property request to get the heat flux vector of
 * the two sides. The definition of the average is given in the derived classes,
 * which can be a simple 50/50 average or a weighted average.
 */
class ComputeHeatFluxAverJumpLocal : public ComputeHeatFluxAverJumpBase
{
public:
  static InputParameters validParams();
  ComputeHeatFluxAverJumpLocal(const InputParameters & parameters);

protected:
  /// Calculate for the actual heat flux average and jump
  void computeQpFluxAverJump() override;

  /// Obtain the diffusivity tensors from materials on e and n sides
  void obtainQpDiffTensors() override;

  /// Provide for the heat flux average and jump in global coordinates.
  virtual void computeGlobalFluxAverJump() = 0;

  /// Provide for the heat flux average in interface coordinates.
  virtual void computeLocalHeatFluxAver() = 0;

  /// the coupled flux and neighbor flux values
  ///@{
  const MaterialProperty<RealVectorValue> & _heat_flux_e;
  const MaterialProperty<RealVectorValue> & _heat_flux_n;
  ///@}

  /// The coupled derivative of Q wrt the thermal gradient.
  ///@{
  const MaterialProperty<RankTwoTensor> & _diff_tensor_e;
  const MaterialProperty<RankTwoTensor> & _diff_tensor_n;
  ///@}
};
