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
 * This interface material class computes the heat flux average and jump .
 * The heat flux on each side is dotted with the unit normal vector to
 * obtain the component flowing across the interface. The derived classes
 * will request the heat flux vector from the element and neighbor side
 * using the parameters '_base_name_e' and '_base_name_n' provided, which
 * must point to an active Kernel/Material sequence in the adjacent blocks.
 */
class ComputeHeatFluxAverJumpBase : public InterfaceMaterial
{
public:
  static InputParameters validParams();
  ComputeHeatFluxAverJumpBase(const InputParameters & parameters);

protected:
  /// Initialize everything with zeros
  virtual void initQpStatefulProperties() override;
  /// Update all properties (global and local, average and jump)
  virtual void computeQpProperties() override;
  /// Provide for the actual heat flux average and jump
  virtual void computeQpFluxAverJump() = 0;
  /// Obtain the diffusivity tensors from materials on e and n sides
  virtual void obtainQpDiffTensors() = 0;

  /// Base name of the material system
  const std::string _base_name;

  /// Base name of heat flux on element side
  const std::string _base_name_e;

  /// Base name of heat flux on neighbor side
  const std::string _base_name_n;

  /// the interface normal in the undeformed configuration
  const MooseArray<Point> & _normals;

  /// the heat flux average and jump in global coordinates; 
  /// TM-analogy: _traction_global
  ///@{
  MaterialProperty<Real> & _heatflux_aver_global;
  MaterialProperty<Real> & _heatflux_jump_global;
  ///@}

  /// the heat flux average in interface coordinates; 
  /// TM-analogy: _interface_traction
  MaterialProperty<Real> & _interface_heatflux_aver;

  /// The derivative of Q wrt the thermal gradient; TM-analogy: _dstress_dstrain
  ///@{
  MaterialProperty<RankTwoTensor> & _dQdT_e;
  MaterialProperty<RankTwoTensor> & _dQdT_n;
  ///@}

};
