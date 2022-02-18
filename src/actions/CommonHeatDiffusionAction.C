//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "CommonHeatDiffusionAction.h"
#include "HeatDiffusionAction.h"
#include "ActionWarehouse.h"

registerMooseAction("BeaverApp", CommonHeatDiffusionAction, "meta_action");

InputParameters
CommonHeatDiffusionAction::validParams()
{
  InputParameters params = HeatDiffusionActionBase::validParams();
  params.addClassDescription("Store common heat diffusion parameters");
  return params;
}

CommonHeatDiffusionAction::CommonHeatDiffusionAction(const InputParameters & parameters)
  : Action(parameters)
{
}

void
CommonHeatDiffusionAction::act()
{
  // check if sub-blocks block are found which will use the common parameters
  auto action = _awh.getActions<HeatDiffusionActionBase>();
  if (action.size() == 0)
    mooseWarning("Common parameters are supplied, but not used in ", parameters().blockLocation());
}
