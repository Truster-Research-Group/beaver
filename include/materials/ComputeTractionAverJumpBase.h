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
 * This interface material class computes the traction average and jump .
 * The traction on each side is dotted with the unit normal vector to
 * obtain the component flowing across the interface. The derived classes
 * will request the traction vector from the element and neighbor side
 * using the parameters '_base_name_e' and '_base_name_n' provided, which
 * must point to an active Kernel/Material sequence in the adjacent blocks.
 */
class ComputeTractionAverJumpBase : public InterfaceMaterial
{
public:
  static InputParameters validParams();
  ComputeTractionAverJumpBase(const InputParameters & parameters);

protected:
  /// Initialize everything with zeros
  virtual void initQpStatefulProperties() override;
  /// Update all properties (global and local, average and jump)
  virtual void computeQpProperties() override;
  /// Provide for the actual traction average and jump
  virtual void computeQpTracAverJump() = 0;
  /// Obtain the moduli tensors from materials on e and n sides
  virtual void obtainQpModuliTensors() = 0;

  /// Base name of the material system
  const std::string _base_name;

  /// Base name of traction on element side
  const std::string _base_name_e;

  /// Base name of traction on neighbor side
  const std::string _base_name_n;

  /// the interface normal in the undeformed configuration
  const MooseArray<Point> & _normals;

  /// the traction average and jump in global coordinates
  ///@{
  MaterialProperty<RealVectorValue> & _traction_aver_global;
  MaterialProperty<RealVectorValue> & _traction_jump_global;
  ///@}

  /// the traction average in interface coordinates 
  MaterialProperty<RealVectorValue> & _interface_traction_aver;

  /// The derivative of 1st PK stress wrt deformation gradient
  ///@{
  MaterialProperty<RankFourTensor> & _dpk1dF_e;
  MaterialProperty<RankFourTensor> & _dpk1dF_n;
  ///@}

};
