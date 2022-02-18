//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "VMNSInterfaceKernelBase.h"

/// Enforce equilibrium with a total Lagrangian formulation
///
/// This class enforces equilibrium when used in conjunction with
/// the corresponding strain calculator (CalculateStrainLagrangianKernel)
/// and with either a stress calculator that provides the
/// 1st PK stress ("pk1_stress") and the derivative of the 1st PK stress
/// with respect to the deformation gradient ("pk1_jacobian")
///
/// This kernel should be used with the new "ComputeLagrangianStressBase"
/// stress update system and the "ComputeLagrangianStrain" system for strains.
///
class TotalLagrangianVMNSInterface : public VMNSInterfaceKernelBase
{
public:
  static InputParameters validParams();
  TotalLagrangianVMNSInterface(const InputParameters & parameters);

protected:
  /// Implement the R^{\alpha}=SOMETHING residual
  virtual Real computeQpResidual(Moose::DGResidualType type) override;
  /// On diagonal Jacobian, only involves the solid mechanics kernel
  virtual Real computeQpJacobian(Moose::DGJacobianType type) override;
  /// Off diagonal Jacobian, solid mechanics + homogenization constraint
  virtual Real computeQpOffDiagJacobian(Moose::DGJacobianType type, unsigned int jvar) override;

  /// Trial gradient averaging
  virtual void precalculateJacobian() override;

  /// Stabilize a generic gradient tensor
  RankTwoTensor stabilizeGrad(const RankTwoTensor & Gb, const RankTwoTensor & Ga) override;

  /// Calculate the full test gradient (could later be modified for stabilization)
  RankTwoTensor testGrad(unsigned int i);

  /// Compute the stabilized trial function gradient tensor
  RankTwoTensor trialGrad(unsigned int k);

private:
  /// The material part of the Jacobian
  Real materialJacobian(const RankTwoTensor & grad_test, const RankTwoTensor & grad_trial);

  /// Off diagonal Jacobian coming through eigenstrain
  Real eigenstrainJacobianComponent(unsigned int cvar,
                                    const RankFourTensor & C,
                                    const RankTwoTensor & grad_test,
                                    const Real & phi);

  /// Calculate the average gradient of some type (test or trial)
  void avgGrad(const VariablePhiGradient & grads, std::vector<RealVectorValue> & res);

  /// Compute the average trial function gradient
  void computeAverageGradPhi();

protected:
  /// ELEMENT side
  /// The 1st Piola-Kirchhoff stress
  const MaterialProperty<RankTwoTensor> & _pk1_E;
  /// The derivative of the PK1 stress with respect to the
  /// deformation gradient
  const MaterialProperty<RankFourTensor> & _dpk1_E;

  /// Averaged trial function gradients
  std::vector<RealVectorValue> _avg_grad_trial_E;

  /// The unmodified deformation gradient
  const MaterialProperty<RankTwoTensor> & _unstabilized_def_grad_E;

  /// The element-average deformation gradient
  const MaterialProperty<RankTwoTensor> & _aF_E;

  /// The inverse increment deformation gradient
  const MaterialProperty<RankTwoTensor> & _inv_inc_def_grad_E;

  /// The actual (stabilized) deformation gradient
  const MaterialProperty<RankTwoTensor> & _def_grad_E;

  /// NEIGHBOR side
  /// The 1st Piola-Kirchhoff stress
  const MaterialProperty<RankTwoTensor> & _pk1_N;
  /// The derivative of the PK1 stress with respect to the
  /// deformation gradient
  const MaterialProperty<RankFourTensor> & _dpk1_N;

  /// Averaged trial function gradients
  std::vector<RealVectorValue> _avg_grad_trial_N;

  /// The unmodified deformation gradient
  const MaterialProperty<RankTwoTensor> & _unstabilized_def_grad_N;

  /// The element-average deformation gradient
  const MaterialProperty<RankTwoTensor> & _aF_N;

  /// The inverse increment deformation gradient
  const MaterialProperty<RankTwoTensor> & _inv_inc_def_grad_N;

  /// The actual (stabilized) deformation gradient
  const MaterialProperty<RankTwoTensor> & _def_grad_N;
};
