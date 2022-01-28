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

class HeatDiffusionActionBase : public Action
{
public:
  static InputParameters validParams();

  HeatDiffusionActionBase(const InputParameters & params);

  static MultiMooseEnum outputPropertiesType();
  static MultiMooseEnum materialOutputOrders();
  static MultiMooseEnum materialOutputFamilies();

public:
  ///@{ table data for output generation
  static const std::vector<char> _component_table;
  ///@}

  static void addCartesianComponentOutput(const std::string & name,
                                          const std::string & prop_name = "");

protected:
  static std::map<std::string, std::string> _rank_one_cartesian_component_table;

  const bool _use_ad;
};
