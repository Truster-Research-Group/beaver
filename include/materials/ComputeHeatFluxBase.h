//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Material.h"
#include "RankTwoTensorForward.h"

/// Provide heat flux in the form required for the Heat Diffusion kernels
///
/// This base class represents a material interface "contract"
///
/// As input it takes the thermal gradient provided by
/// ComputeTempGrad:
///   1) "grad_T" -- the thermal gradient
///
/// And return:
///   1) "heat_flux" -- the heat flux vector
///   2) "diff_tensor" -- the derivative of Q with respect
///                           to the thermal gradient
///
class ComputeHeatFluxBase : public Material
{
public:
  static InputParameters validParams();
  ComputeHeatFluxBase(const InputParameters & parameters);

protected:
  /// Initialize everything with zeros
  virtual void initQpStatefulProperties() override;
  /// Update all properties (here just the flux/derivatives)
  virtual void computeQpProperties() override;
  /// Provide for the actual heat flux updates
  virtual void computeQpFluxUpdate() = 0;

protected:

  /// Prepend to the material properties
  const std::string _base_name;

  /// The heat flux vector Q
  MaterialProperty<RealVectorValue> & _heat_flux; // analogy: _stress
  /// The derivative of Q wrt the thermal gradient
  MaterialProperty<RankTwoTensor> & _diff_tensor; // analogy: _dstress_dstrain, _Jacobian_mult
};
