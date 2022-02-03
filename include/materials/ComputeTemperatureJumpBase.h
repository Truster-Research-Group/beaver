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
 * This interface material class computes the temperature jump .
 * It is a scalar, so no transformation between local and global coordinates
 * is needed, but we will still copy it in computeLocalTemperatureJump.
 */
class ComputeTemperatureJumpBase : public InterfaceMaterial
{
public:
  static InputParameters validParams();
  ComputeTemperatureJumpBase(const InputParameters & parameters);

protected:
  void computeQpProperties() override;
  void initQpStatefulProperties() override;

  /// method used to compute the temperature jump in interface coordinates.
  virtual void computeLocalTemperatureJump() = 0;

  /// Base name of the material system
  const std::string _base_name;

  /// the interface normal in the undeformed configuration
  const MooseArray<Point> & _normals;

  /// the coupled temperature and neighbor temperature values
  ///@{
  const VariableValue & _temp;
  const VariableValue & _temp_neighbor;
  ///@}

  /// the temperature jump in global and interface coordiantes
  ///@{
  MaterialProperty<Real> & _temp_jump_global;
  MaterialProperty<Real> & _interface_temp_jump;
  ///@}
};
