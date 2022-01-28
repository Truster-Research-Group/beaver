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
#include "DerivativeMaterialInterface.h"

/**
 * ComputeTempGradBase is the base class for thermal gradient vectors
 */
class ComputeTempGradBase : public DerivativeMaterialInterface<Material>
{
public:
  static InputParameters validParams();

  ComputeTempGradBase(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;

  /// Temperature variable
  const VariableValue & _temp;

  /// Gradient of temperature
  const VariableGradient & _grad_temp;

  /// Base name of the material system
  const std::string _base_name;

  // Thermal gradient vector
  MaterialProperty<RealVectorValue> & _grad_T; // analogy: _mechanical_strain;

  // Holder gradient vector
  MaterialProperty<RealVectorValue> & _hold_T; // analogy: _total_strain;
};
