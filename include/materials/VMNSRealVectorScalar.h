//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "InterfaceMaterial.h"
/**
 * This interface material class extract a Cartesian component from a vector material property
 * defined on a VMNS interface
 */
class VMNSRealVectorScalar : public InterfaceMaterial
{
public:
  static InputParameters validParams();

  VMNSRealVectorScalar(const InputParameters & parameters);

protected:
  void computeQpProperties() override;

  /// Base name of the material system
  const std::string _base_name;

  /// scalar options
  enum class DirectionType
  {
    Normal,
    Tangent
  } _direction;

  /// the property created by this material
  MaterialProperty<Real> & _property;

  /// the vector material property
  const MaterialProperty<RealVectorValue> & _vector;

  /// the material property defining the czm normal
  const MaterialProperty<RankTwoTensor> & _czm_rotation;
};
