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
#include "JvarMapInterface.h"

/// Base class for implementing DG cohesive zone models (CZM) for 1D,2D, and 3D
/// traction separation laws. This kernel operates only on
/// a single displacement compenent.
/// One kernel is required for each displacement component.
class VMNSInterfaceKernelBase : public JvarMapKernelInterface<InterfaceKernel>
{
public:
  static InputParameters validParams();
  VMNSInterfaceKernelBase(const InputParameters & parameters);

protected:
  Real computeQpResidual(Moose::DGResidualType type) override;
  Real computeQpJacobian(Moose::DGJacobianType type) override;
  Real computeQpOffDiagJacobian(Moose::DGJacobianType type, unsigned int jvar) override;

  /// Stability/penalty term for residual[_component]
  virtual Real computeResiStab(const Moose::DGResidualType type) const = 0;

  /// Stability/penalty term for Jacobian 
  /// of residual[_component] w.r.t displacement[component_j]
  virtual Real computeJacoStab(const unsigned int & component_j,
                               const Moose::DGJacobianType & type) const = 0;

  /// Base name of the material system that this kernel applies to
  const std::string _base_name;

  /// the displacement component this kernel is operating on (0=x, 1=y, 2 =z)
  const unsigned int _component;

  /// number of displacement components
  const unsigned int _ndisp;

  /// Coupled displacement component variable IDs
  ///@{
  std::vector<unsigned int> _disp_var;
  std::vector<unsigned int> _disp_neighbor_var;
  ///@}

  // pointer to displacement variables
  std::vector<MooseVariable *> _vars;

  /// Symmetrizing parameter, called theta or beta in some papers
  /// 0=incomplete IIPG, -1=symmetric SIPG, +1=non-symmetric NIPG
  const int _nis_flag;

  /// If true use flux penalty term
  const bool _use_flux_penalty;
};
