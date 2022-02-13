//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ComputeTractionAverJumpBase.h"
/**
 * Compute the interface traction average and jump across a discontinuous interface.
 * Does a MOOSE look-up property request to get the traction vector of
 * the two sides. The definition of the average is given in the derived classes,
 * which can be a simple 50/50 average or a weighted average.
 */
class ComputeTractionAverJumpLocal : public ComputeTractionAverJumpBase
{
public:
  static InputParameters validParams();
  ComputeTractionAverJumpLocal(const InputParameters & parameters);

protected:
  /// Calculate for the actual traction average and jump
  void computeQpTracAverJump() override;

  /// Obtain the diffusivity tensors from materials on e and n sides
  void obtainQpModuliTensors() override;

  /// Provide for the traction average and jump in global coordinates.
  virtual void computeGlobalTracAverJump() = 0;

  /// Provide for the traction average in interface coordinates.
  virtual void computeLocalTractionAver();

  /// the coupled (from ComputeLagrangianStressBase) 1st PK stress 
  /// and neighbor 1st PK stress values
  ///@{
  const MaterialProperty<RankTwoTensor> & _pk1_stress_e;
  const MaterialProperty<RankTwoTensor> & _pk1_stress_n;
  ///@}

  /// The coupled (from ComputeLagrangianStressBase) derivative of 
  /// 1st PK stress wrt the deformation gradient.
  /// Used to construct _dQdT_e and _dQdT_n in the base class.
  ///@{
  const MaterialProperty<RankFourTensor> & _pk1_jacobian_e;
  const MaterialProperty<RankFourTensor> & _pk1_jacobian_n;
  ///@}

  /// the rotation matrix transforming from the interface to the global coordinate systems
  const MaterialProperty<RankTwoTensor> & _czm_total_rotation;
};
