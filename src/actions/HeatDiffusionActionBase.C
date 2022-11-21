//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "HeatDiffusionActionBase.h"
#include "CommonHeatDiffusionAction.h"
#include "ActionWarehouse.h"
#include "AddAuxVariableAction.h"
#include "InputParameterWarehouse.h"

// map vector name shortcuts to tensor material property names
std::map<std::string, std::string> HeatDiffusionActionBase::_rank_one_cartesian_component_table = {
    {"total_t", "hold_t"}, {"grad_t", "grad_t"}, {"heat_flux", "heat_flux"}};
const std::vector<char> HeatDiffusionActionBase::_component_table = {'x', 'y', 'z'};

InputParameters
HeatDiffusionActionBase::validParams()
{
  InputParameters params = Action::validParams();

  params.addRequiredParam<std::vector<VariableName>>("temp",
                                                     "The temperature variable for the problem");

  params.addParam<std::string>("base_name", "Material property base name");
  params.addParam<bool>("add_variables", false, "Add the temperature variable");
  params.addParam<bool>("use_automatic_differentiation",
                        false,
                        "Flag to use automatic differentiation (AD) objects when possible");

  // Advanced
  params.addParam<std::vector<AuxVariableName>>("save_in", "The temperature residuals");
  params.addParam<std::vector<AuxVariableName>>("diag_save_in",
                                                "The temperature diagonal preconditioner terms");
  params.addParamNamesToGroup("save_in diag_save_in", "Advanced");

  // Output
  params.addParam<MultiMooseEnum>("generate_output",
                                  HeatDiffusionActionBase::outputPropertiesType(),
                                  "Add scalar quantity output for heat flux and/or grad T");

  params.addParam<MultiMooseEnum>(
      "material_output_order",
      HeatDiffusionActionBase::materialOutputOrders(),
      "Specifies the order of the FE shape function to use for this variable.");

  params.addParam<MultiMooseEnum>(
      "material_output_family",
      HeatDiffusionActionBase::materialOutputFamilies(),
      "Specifies the family of FE shape functions to use for this variable.");
  params.addParamNamesToGroup("generate_output material_output_order material_output_family",
                              "Output");
  params.addParam<bool>("verbose", false, "Display extra information.");

  return params;
}

HeatDiffusionActionBase::HeatDiffusionActionBase(const InputParameters & parameters)
  : Action(parameters), _use_ad(getParam<bool>("use_automatic_differentiation"))
{
  // FIXME: suggest to use action of action to add this to avoid changing the input parameters in
  // the warehouse.
  const auto & params = _app.getInputParameterWarehouse().getInputParameters();
  InputParameters & pars(*(params.find(uniqueActionName())->second.get()));

  // check if a container block with common parameters is found
  auto action = _awh.getActions<CommonHeatDiffusionAction>();
  if (action.size() == 1)
    pars.applyParameters(action[0]->parameters());

  // append additional_generate_output
  if (isParamValid("additional_generate_output"))
  {
    MultiMooseEnum generate_output = getParam<MultiMooseEnum>("generate_output");
    MultiMooseEnum additional_generate_output =
        getParam<MultiMooseEnum>("additional_generate_output");

    MultiMooseEnum material_output_order = getParam<MultiMooseEnum>("material_output_order");
    MultiMooseEnum additional_material_output_order =
        getParam<MultiMooseEnum>("additional_material_output_order");

    MultiMooseEnum material_output_family = getParam<MultiMooseEnum>("material_output_family");
    MultiMooseEnum additional_material_output_family =
        getParam<MultiMooseEnum>("additional_material_output_family");

    for (auto & output : additional_generate_output)
      generate_output.push_back(output);
    for (auto & order : additional_material_output_order)
      material_output_order.push_back(order);
    for (auto & family : additional_material_output_family)
      material_output_family.push_back(family);

    pars.set<MultiMooseEnum>("generate_output") = generate_output;
    pars.set<MultiMooseEnum>("material_output_order") = material_output_order;
    pars.set<MultiMooseEnum>("material_output_family") = material_output_family;
  }
}

MultiMooseEnum
HeatDiffusionActionBase::materialOutputOrders()
{
  auto orders = AddAuxVariableAction::getAuxVariableOrders().getRawNames();

  return MultiMooseEnum(orders);
}

MultiMooseEnum
HeatDiffusionActionBase::materialOutputFamilies()
{
  return MultiMooseEnum("MONOMIAL LAGRANGE");
}

MultiMooseEnum
HeatDiffusionActionBase::outputPropertiesType()
{
  std::string options = "";
  for (auto & r2tc : _rank_one_cartesian_component_table)
    for (unsigned int a = 0; a < 3; ++a)
      options += (options == "" ? "" : " ") + r2tc.first + '_' + _component_table[a];

  return MultiMooseEnum(options, "", true);
}

void
HeatDiffusionActionBase::addCartesianComponentOutput(const std::string & enum_name,
                                                     const std::string & prop_name)
{
  if (prop_name.empty())
    // the enum name is the actual tensor material property name
    _rank_one_cartesian_component_table.emplace(enum_name, enum_name);
  else
    // supply a different name for the enum options (this is done for
    // 'strain' -> 'grad_T' in the TMA)
    _rank_one_cartesian_component_table.emplace(enum_name, prop_name);
}
