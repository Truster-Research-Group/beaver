//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "HeatInterAction.h"
#include "AddAuxVariableAction.h"
#include "Factory.h"
#include "FEProblem.h"
#include "Conversion.h"

registerMooseAction("BeaverApp", HeatInterAction, "add_interface_kernel");
registerMooseAction("BeaverApp", HeatInterAction, "add_material");
registerMooseAction("BeaverApp", HeatInterAction, "add_master_action_material");
registerMooseAction("BeaverApp", HeatInterAction, "add_aux_variable");
registerMooseAction("BeaverApp", HeatInterAction, "add_aux_kernel");
registerMooseAction("BeaverApp", HeatInterAction, "add_kernel");
registerMooseAction("BeaverApp", HeatInterAction, "validate_coordinate_systems");

InputParameters
HeatInterAction::validParams()
{
  InputParameters params = HeatInterActionBase::validParams();
  params.addClassDescription("Action to create an instance of the "
                             "VMNT kernel for the temperature field");
  params.addRequiredParam<std::vector<BoundaryName>>(
      "boundary", "The list of boundary IDs from the mesh where the VMNT interface will be applied");
  return params;
}

HeatInterAction::HeatInterAction(const InputParameters & params)
  : HeatInterActionBase(params),
    _temp(getParam<std::vector<VariableName>>("temp")),
    _ntemp(_temp.size()),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _base_name_e(isParamValid("base_name_e") ? getParam<std::string>("base_name_e") : ""),
    _base_name_n(isParamValid("base_name_n") ? getParam<std::string>("base_name_n") : ""),
    _boundary(getParam<std::vector<BoundaryName>>("boundary")),
    _fluxaver(getParam<MooseEnum>("flux_type").getEnum<FluxAver>()),
    _nis_flag(getParam<int>("nis_flag")),
    _use_flux_penalty(getParam<bool>("use_flux_penalty")),
    _save_in_e(getParam<std::vector<AuxVariableName>>("save_in_e")),
    _diag_save_in_e(getParam<std::vector<AuxVariableName>>("diag_save_in_e")),
    _save_in_n(getParam<std::vector<AuxVariableName>>("save_in_n")),
    _diag_save_in_n(getParam<std::vector<AuxVariableName>>("diag_save_in_n")),
    _material_output_order(getParam<MultiMooseEnum>("material_output_order")),
    _material_output_family(getParam<MultiMooseEnum>("material_output_family")),
    _verbose(getParam<bool>("verbose"))
{
  // We can't enforce consistency between the number of temperature variables and the mesh
  // dimension. Hence we only check we have a reasonable number of temperature variables
  if (_ntemp != 1)
    mooseError("the VMNT Action requires 1 temperature variable.");

  switch (_fluxaver)
  {
    case FluxAver::Simple:
    {
      _vmnt_kernel_name = "VMNTInterfaceKernelSimple";
      _temp_jump_provider_name = "ComputeTemperatureJumpLocal";
      _heat_flux_calculator_name = "ComputeHeatFluxAverJumpSimple";
      break;
    }
    case FluxAver::Complex:
    {
      _vmnt_kernel_name = "VMNTInterfaceKernelComplex";
      _temp_jump_provider_name = "ComputeTemperatureJumpLocal";
      _heat_flux_calculator_name = "ComputeHeatFluxAverJumpComplex";
      break;
    }
    default:
      mooseError("HeatInterAction Error: Invalid kernel parameter. Allowed values are: "
                 "Simple or Complex");
  }

  if (_save_in_e.size() != 0 && _save_in_e.size() != _ntemp)
    mooseError(
        "Number of save_in_e variables should equal to the number of temperature variables ",
        _ntemp);
  if (_diag_save_in_e.size() != 0 && _diag_save_in_e.size() != _ntemp)
    mooseError("Number of diag_save_in_e variables should equal to the number of temperature "
               "variables ",
               _ntemp);
  if (_save_in_n.size() != 0 && _save_in_n.size() != _ntemp)
    mooseError(
        "Number of save_in_n variables should equal to the number of temperature variables ",
        _ntemp);

  if (_diag_save_in_n.size() != 0 && _diag_save_in_n.size() != _ntemp)
    mooseError("Number of diag_save_in_n variables should equal to the number of temperature "
               "variables ",
               _ntemp);

  // convert output variable names to lower case
  for (const auto & out : getParam<MultiMooseEnum>("generate_output"))
  {
    std::string lower(out);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    _generate_output.push_back(lower);
  }

  if (!_generate_output.empty())
    verifyOrderAndFamilyOutputs();
}

void
HeatInterAction::addRequiredVMNTInterfaceKernels()
{
  for (unsigned int i = 0; i < _ntemp; ++i)
  {
    // Create unique kernel name for each temperature component
    std::string unique_kernel_name = _vmnt_kernel_name + "_" + _name + "_" + Moose::stringify(i);

    InputParameters paramsk = _factory.getValidParams(_vmnt_kernel_name);

    // paramsk.set<unsigned int>("component") = i;
    paramsk.set<NonlinearVariableName>("variable") = _temp[i];
    paramsk.set<std::vector<VariableName>>("neighbor_var") = {_temp[i]};
    // paramsk.set<std::vector<VariableName>>("temp") = _temp;
    paramsk.set<std::vector<BoundaryName>>("boundary") = _boundary;
    if (!_base_name.empty())
    {
      // _base_name has a '_' at the end, so look up the original parameter
      paramsk.set<std::string>("base_name") = getParam<std::string>("base_name");
    }
    paramsk.set<int>("nis_flag") = _nis_flag;
    paramsk.set<bool>("use_flux_penalty") = _use_flux_penalty;

    //// SOMEDAY, probably need to add pen_scale or how to handle other properties!!!


    std::string save_in_side;
    std::vector<AuxVariableName> save_in_var_names;
    if (_save_in_e.size() == _ntemp || _save_in_n.size() == _ntemp)
    {
      prepareSaveInInputs(save_in_var_names, save_in_side, _save_in_e, _save_in_n, i);
      paramsk.set<std::vector<AuxVariableName>>("save_in") = save_in_var_names;
      paramsk.set<MultiMooseEnum>("save_in_var_side") = save_in_side;
    }
    if (_diag_save_in_e.size() == _ntemp || _diag_save_in_n.size() == _ntemp)
    {
      prepareSaveInInputs(
          save_in_var_names, save_in_side, _diag_save_in_e, _diag_save_in_n, i);
      paramsk.set<std::vector<AuxVariableName>>("diag_save_in") = save_in_var_names;
      paramsk.set<MultiMooseEnum>("diag_save_in_var_side") = save_in_side;
    }
    _problem->addInterfaceKernel(_vmnt_kernel_name, unique_kernel_name, paramsk);
  }
}

void
HeatInterAction::addRequiredVMNTInterfaceMaterials()
{
  // Create unique material name for the "ComputeTemperatureJump" object
  std::string unique_material_name = _temp_jump_provider_name + "_" + _name;
  InputParameters paramsm = _factory.getValidParams(_temp_jump_provider_name);
  paramsm.set<std::vector<BoundaryName>>("boundary") = _boundary;
  ;
  paramsm.set<std::vector<VariableName>>("temp") = _temp;
  if (!_base_name.empty())
  {
    // _base_name has a '_' at the end, so look up the original parameter
    paramsm.set<std::string>("base_name") = getParam<std::string>("base_name");
  }
  _problem->addInterfaceMaterial(_temp_jump_provider_name, unique_material_name, paramsm);

  // Create unique material name for the "ComputeHeatFluxAverJump" object
  unique_material_name = _heat_flux_calculator_name + "_" + _name;
  paramsm = _factory.getValidParams(_heat_flux_calculator_name);
  paramsm.set<std::vector<BoundaryName>>("boundary") = _boundary;
  ;
  if (!_base_name.empty())
  {
    // _base_name has a '_' at the end, so look up the original parameter
    paramsm.set<std::string>("base_name") = getParam<std::string>("base_name");
  }
  if (!_base_name_e.empty())
  {
    // this parameter is clean, no '_'
    paramsm.set<std::string>("base_name_e") = _base_name_e;
  }
  if (!_base_name_n.empty())
  {
    // this parameter is clean, no '_'
    paramsm.set<std::string>("base_name_n") = _base_name_n;
  }
  _problem->addInterfaceMaterial(
      _heat_flux_calculator_name, unique_material_name, paramsm);
}

void
HeatInterAction::act()
{

  chekMultipleActionParameters();

  if (_current_task == "add_interface_kernel")
    addRequiredVMNTInterfaceKernels();
  else if (_current_task == "add_master_action_material")
    addRequiredVMNTInterfaceMaterials();

  // optional, add required outputs
  actOutputGeneration();
}

void
HeatInterAction::addRelationshipManagers(Moose::RelationshipManagerType input_rm_type)
{
  InputParameters ips = _factory.getValidParams(_vmnt_kernel_name);
  addRelationshipManagers(input_rm_type, ips);
}

void
HeatInterAction::prepareSaveInInputs(std::vector<AuxVariableName> & save_in_names,
                                        std::string & save_in_side,
                                        const std::vector<AuxVariableName> & var_name_e,
                                        const std::vector<AuxVariableName> & var_name_n,
                                        const int & i) const
{
  save_in_names.clear();
  save_in_side.clear();
  if (var_name_e.size() == _ntemp)
  {
    save_in_names.push_back(var_name_e[i]);
    save_in_side += "m";
    if (var_name_n.size() == _ntemp)
      save_in_side += " ";
  }
  if (var_name_n.size() == _ntemp)
  {
    save_in_names.push_back(var_name_n[i]);
    save_in_side += "s";
  }
}

void
HeatInterAction::verifyOrderAndFamilyOutputs()
{
  // Ensure material output order and family vectors are same size as generate output

  // check number of supplied orders and families
  if (_material_output_order.size() > 1 && _material_output_order.size() < _generate_output.size())
    paramError("material_output_order",
               "The number of orders assigned to material outputs must be: 0 to be assigned "
               "CONSTANT; 1 to assign all outputs the same value, or the same size as the number "
               "of generate outputs listed.");

  if (_material_output_family.size() > 1 &&
      _material_output_family.size() < _generate_output.size())
    paramError("material_output_family",
               "The number of families assigned to material outputs must be: 0 to be assigned "
               "MONOMIAL; 1 to assign all outputs the same value, or the same size as the number "
               "of generate outputs listed.");

  // if no value was provided, chose the default CONSTANT
  if (_material_output_order.size() == 0)
    _material_output_order.push_back("CONSTANT");

  // For only one order, make all orders the same magnitude
  if (_material_output_order.size() == 1)
    _material_output_order =
        std::vector<std::string>(_generate_output.size(), _material_output_order[0]);

  if (_verbose)
    Moose::out << COLOR_CYAN << "*** Automatic applied material output orders ***"
               << "\n"
               << _name << ": " << Moose::stringify(_material_output_order) << "\n"
               << COLOR_DEFAULT;

  // if no value was provided, chose the default MONOMIAL
  if (_material_output_family.size() == 0)
    _material_output_family.push_back("MONOMIAL");

  // For only one family, make all families that value
  if (_material_output_family.size() == 1)
    _material_output_family =
        std::vector<std::string>(_generate_output.size(), _material_output_family[0]);

  if (_verbose)
    Moose::out << COLOR_CYAN << "*** Automatic applied material output families ***"
               << "\n"
               << _name << ": " << Moose::stringify(_material_output_family) << "\n"
               << COLOR_DEFAULT;
}

void
HeatInterAction::actOutputGeneration()
{

  // Add variables (optional)
  if (_current_task == "add_aux_variable")
  {
    unsigned int index = 0;
    for (auto out : _generate_output)
    {
      const auto & order = _material_output_order[index];
      const auto & family = _material_output_family[index];

      std::string type = (order == "CONSTANT" && family == "MONOMIAL")
                             ? "MooseVariableConstMonomial"
                             : "MooseVariable";

      // Create output helper aux variables
      auto params = _factory.getValidParams(type);
      params.set<MooseEnum>("order") = order;
      params.set<MooseEnum>("family") = family;
      if (family == "MONOMIAL")
        _problem->addAuxVariable(type, _base_name + out, params);
      else
        _problem->addVariable(type, _base_name + out, params);

      index++;
    }
  }

  // Add output AuxKernels
  else if (_current_task == "add_aux_kernel")
  {

    // Loop through output aux variables
    unsigned int index = 0;
    for (auto out : _generate_output)
    {
      if (_material_output_family[index] == "MONOMIAL")
      {
        InputParameters params = _factory.getValidParams("MaterialRealAux");
        params.set<MaterialPropertyName>("property") = _base_name + out;
        params.set<AuxVariableName>("variable") = _base_name + out;
        params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_END;
        params.set<std::vector<BoundaryName>>("boundary") = _boundary;
        params.set<bool>("check_boundary_restricted") = false;
        _problem->addAuxKernel("MaterialRealAux", _base_name + out + '_' + name(), params);
        // Note: these are revised so the output "works"; Instead, could make
        // it fancier like the CZM case; namely, the _name has been left off
      }
      index++;
    }
  }
}

void
HeatInterAction::chekMultipleActionParameters()
{

  // Gather info about all other master actions when we add variables
  if (_current_task == "validate_coordinate_systems")
  {
    auto actions = _awh.getActions<HeatInterAction>();
    for (const auto & action : actions)
    {
      const auto size_before = _boundary_name_union.size();
      const auto added_size = action->_boundary.size();
      _boundary_name_union.insert(action->_boundary.begin(), action->_boundary.end());
      const auto size_after = _boundary_name_union.size();
      if (size_after != size_before + added_size)
        mooseError("The boundary restrictions in the HeatInterAction actions must be "
                   "non-overlapping.");
    }

    /////   NOTE, probably should allow for different formulations
    
    for (const auto & action : actions)
    {
      // check for different flux average definitions
      _flux_formulation_union.insert(action->_fluxaver);
      const auto size_after = _flux_formulation_union.size();

      if (size_after != 1)
        mooseError("All blocks of the HeatInterAction should have the same VMNT formulation");
    }
  }
}
