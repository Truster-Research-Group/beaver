//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "HeatInterActionBase.h"
#include "CommonHeatInterAction.h"
#include "ActionWarehouse.h"
#include "AddAuxVariableAction.h"
#include "MooseEnum.h"

// map vector name shortcuts to VMNT material property names
const std::map<std::string, std::string>
    HeatInterActionBase::_real_output_table = {
        {"heatflux_aver_global", "heatflux_aver_global"},
        {"temp_jump_global", "temp_jump_global"},
        {"heatflux_jump_global", "heatflux_jump_global"}};
        // {"flux_aver", "heatflux_aver_global"},
        // {"jump", "temp_jump_global"},
        // {"flux_jump", "_heatflux_jump_global"}};

InputParameters
HeatInterActionBase::validParams()
{
  InputParameters params = Action::validParams();
  params.addClassDescription("Action to create an instance of the "
                             "VMNT kernel for the temperature field");
  params.addRequiredParam<std::vector<VariableName>>(
      "temp", "The temperature variable for the problem");
  MooseEnum fluxaverType("SIMPLE COMPLEX", "SIMPLE");
  params.addParam<MooseEnum>("flux_type", fluxaverType, "Flux averaging type");
  params.addParam<int>("nis_flag", 0, "Integer for IIPG=0, SIPG=-1, or NIPG=+1 method");
  params.addParam<bool>("use_flux_penalty", false, "Use heat flux jump penalty");

  // Advanced
  params.addParam<std::string>("base_name", "Material property base name");
  params.addParam<std::string>("base_name_e", "Base name of heat flux on element side");
  params.addParam<std::string>("base_name_n", "Base name of heat flux on neighbor side");
  params.addParam<std::vector<AuxVariableName>>("save_in_e",
                                                "The temperature residuals on the  master side");
  params.addParam<std::vector<AuxVariableName>>(
      "diag_save_in_e", "The temperature diagonal preconditioner terms on the  master side");
  params.addParam<std::vector<AuxVariableName>>("save_in_n",
                                                "The temperature residuals on the  slave side");
  params.addParam<std::vector<AuxVariableName>>(
      "diag_save_in_n", "The temperature diagonal preconditioner terms on the  slave side");
  params.addParamNamesToGroup("save_in_e diag_save_in_e save_in_n diag_save_in_n",
                              "Advanced");
  params.addParam<bool>("verbose", false, "Display extra information.");
  // Output
  params.addParam<MultiMooseEnum>("generate_output",
                                  HeatInterActionBase::outputPropertiesType(),
                                  "Add scalar quantity output for temparature and/or flux");
  params.addParam<MultiMooseEnum>(
      "material_output_order",
      HeatInterActionBase::materialOutputOrders(),
      "Specifies the order of the FE shape function to use for this variable.");
  params.addParam<MultiMooseEnum>(
      "material_output_family",
      HeatInterActionBase::materialOutputFamilies(),
      "Specifies the family of FE shape functions to use for this variable.");
  params.addParamNamesToGroup("generate_output material_output_order material_output_family",
                              "Output");
  params.addParam<MultiMooseEnum>("additional_generate_output",
                                  HeatInterActionBase::outputPropertiesType(),
                                  "Add scalar quantity output for for temparature and/or flux (will be "
                                  "appended to the list in `generate_output`)");
  params.addParam<MultiMooseEnum>(
      "additional_material_output_order",
      HeatInterActionBase::materialOutputOrders(),
      "Specifies the order of the FE shape function to use for this variable.");

  params.addParam<MultiMooseEnum>(
      "additional_material_output_family",
      HeatInterActionBase::materialOutputFamilies(),
      "Specifies the family of FE shape functions to use for this variable.");

  params.addParamNamesToGroup("additional_generate_output additional_material_output_order "
                              "additional_material_output_family",
                              "Output");
  return params;
}

HeatInterActionBase::HeatInterActionBase(const InputParameters & params) : Action(params)
{
  // check if a container block with common parameters is found
  auto action = _awh.getActions<CommonHeatInterAction>();
  if (action.size() == 1)
    _pars.applyParameters(action[0]->parameters());

  // append additional_generate_output to generate_output
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

    _pars.set<MultiMooseEnum>("generate_output") = generate_output;
    _pars.set<MultiMooseEnum>("material_output_order") = material_output_order;
    _pars.set<MultiMooseEnum>("material_output_family") = material_output_family;
  }
}

MultiMooseEnum
HeatInterActionBase::outputPropertiesType()
{
  std::string options = "";
  for (auto & vc : _real_output_table)
    // for (unsigned int a = 0; a < 3; ++a)
      options += (options == "" ? "" : " ") + vc.first;

  return MultiMooseEnum(options, "", true);
}

MultiMooseEnum
HeatInterActionBase::materialOutputOrders()
{
  auto orders = AddAuxVariableAction::getAuxVariableOrders().getRawNames();

  return MultiMooseEnum(orders);
}

MultiMooseEnum
HeatInterActionBase::materialOutputFamilies()
{
  return MultiMooseEnum("MONOMIAL");
}
