//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Action.h"

/**
 * Base class for heat diffusion kernel and material system
 */
class HeatDiffusionActionBase : public Action
{
public:
  static InputParameters validParams();

  HeatDiffusionActionBase(const InputParameters & params);

  /// Construct available material output quantities with components
  static MultiMooseEnum outputPropertiesType();
  /// Available shape function orders, 
  static MultiMooseEnum materialOutputOrders();
  /// Available shape function families, MONOMIAL is default
  static MultiMooseEnum materialOutputFamilies();

public:
  ///@{ table of x,y,z indices for output generation
  static const std::vector<char> _component_table;
  ///@}

  /// Do a name swap if the input syntax is shorter
  static void addCartesianComponentOutput(const std::string & name,
                                          const std::string & prop_name = "");

protected:
  /// List of available material output quantities
  static std::map<std::string, std::string> _rank_one_cartesian_component_table;

  /// Boolean for using AD MOOSE objects; not yet implemented for VMNT
  const bool _use_ad;
};
