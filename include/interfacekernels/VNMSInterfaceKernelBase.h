//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "InterfaceKernel.h"
#include "DerivativeMaterialInterface.h"
#include "JvarMapInterface.h"

/// Base class for implementing Variational Multiscale Nitsche (VMN)
/// Solid (S) mechanics weak interface coupling models in 1D, 2D, and 3D.
/// Constructed in the spirit of the new "Lagrangian" kernel.
///
/// This class provides common input properties and helper methods,
/// most of the math has to be done in the subclasses
///
class VMNSInterfaceKernelBase
  : public JvarMapKernelInterface<DerivativeMaterialInterface<Kernel>>
{
public:
  static InputParameters validParams();
  VMNSInterfaceKernelBase(const InputParameters & parameters);

protected:

  /// Override residual calculation to provide for element-level averaging
  virtual void computeResidual() override;
  /// Override  on-diagonal Jacobian to provide for element-level averaging
  virtual void computeJacobian() override;
  /// Override off diagonal Jacobian loop for averaging for stabilization
  virtual void computeOffDiagJacobian(const unsigned int jvar) override;
  
  /// Override with any initial setup for the residual
  virtual void precalculateResidual() override;
  
  /// Override with any initial setup for the kernel
  virtual void precalculateJacobian() override;

  /// Helper to assemble the stabilized gradient operator
  virtual RankTwoTensor fullGrad(unsigned int m,
                                 bool use_stable,
                                 const RealGradient & base_grad,
                                 const RealGradient & avg_grad);

  /// Override with the actual stabilization calculation
  virtual RankTwoTensor stabilizeGrad(const RankTwoTensor & base, const RankTwoTensor & avg) = 0;

  /// Helper to assemble a full gradient operator from a gradient vector
  virtual RankTwoTensor gradOp(unsigned int m, const RealGradient & grad);

protected:

  /// If true use large deformation kinematics
  const bool _large_kinematics;

  /// If true calculate the deformation gradient derivatives for F_bar
  const bool _stabilize_strain { false }; // hard code false for now

  /// Prepend to the material properties
  const std::string _base_name;

  /// Which component of the vector residual this kernel is responsible for
  const unsigned int _component;
  /// Total number of displacements/size of residual vector
  const unsigned int _ndisp;

  /// The displacement numbers
  std::vector<unsigned int> _disp_nums;

  /// Temperature, if provided.  This is used only to get the trial functions
  const MooseVariable * _temperature;

  /// Eigenstrain derivatives wrt generate coupleds
  std::vector<std::vector<const MaterialProperty<RankTwoTensor> *>> _deigenstrain_dargs;

};
