//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "Conversion.h"
#include "FEProblem.h"
#include "Factory.h"
#include "MooseMesh.h"
#include "MooseObjectAction.h"
#include "HeatDiffusionAction.h"
#include "Material.h"

#include "BlockRestrictable.h"

#include "AddVariableAction.h"

#include "libmesh/string_to_enum.h"
#include <algorithm>

registerMooseAction("BeaverApp", HeatDiffusionAction, "meta_action");

registerMooseAction("BeaverApp", HeatDiffusionAction, "setup_mesh_complete");

registerMooseAction("BeaverApp", HeatDiffusionAction, "validate_coordinate_systems");

registerMooseAction("BeaverApp", HeatDiffusionAction, "add_variable");

registerMooseAction("BeaverApp", HeatDiffusionAction, "add_aux_variable");

registerMooseAction("BeaverApp", HeatDiffusionAction, "add_kernel");

registerMooseAction("BeaverApp", HeatDiffusionAction, "add_aux_kernel");

registerMooseAction("BeaverApp", HeatDiffusionAction, "add_material");

registerMooseAction("BeaverApp", HeatDiffusionAction, "add_master_action_material");

registerMooseAction("BeaverApp", HeatDiffusionAction, "add_scalar_kernel");

registerMooseAction("BeaverApp", HeatDiffusionAction, "add_user_object");

InputParameters
HeatDiffusionAction::validParams()
{
  InputParameters params = HeatDiffusionActionBase::validParams();
  params.addClassDescription("Set up heat divergence kernels "
                             "and materials in sequence");

  // parameters specified here only appear in the input file sub-blocks of the
  // Master action, not in the common parameters area
  params.addParam<std::vector<SubdomainName>>("block",
                                              "The list of ids of the blocks (subdomain) "
                                              "that the stress divergence kernels will be "
                                              "applied to");
  params.addParamNamesToGroup("block", "Advanced");

  params.addParam<MultiMooseEnum>("additional_generate_output",
                                  HeatDiffusionActionBase::outputPropertiesType(),
                                  "Add scalar quantity output for stress and/or strain (will be "
                                  "appended to the list in `generate_output`)");
  params.addParam<MultiMooseEnum>(
      "additional_material_output_order",
      HeatDiffusionActionBase::materialOutputOrders(),
      "Specifies the order of the FE shape function to use for this variable.");

  params.addParam<MultiMooseEnum>(
      "additional_material_output_family",
      HeatDiffusionActionBase::materialOutputFamilies(),
      "Specifies the family of FE shape functions to use for this variable.");

  params.addParamNamesToGroup("additional_generate_output additional_material_output_order "
                              "additional_material_output_family",
                              "Output");
  params.addParam<std::string>(
      "grad_base_name",
      "The base name used for the strain. If not provided, it will be set equal to base_name");
  params.addParam<std::vector<TagName>>(
      "extra_vector_tags",
      "The tag names for extra vectors that residual data should be saved into");
  params.addParam<Real>("scaling", "The scaling to apply to the displacement variables");
  params.addParam<Point>("direction", "Direction stress/strain is calculated in");

  return params;
}

const std::map<unsigned int, std::string> HeatDiffusionAction::_order_mapper = {
    {1, "FIRST"}, {3, "THIRD"}, {4, "FOURTH"}, {6, "SIXTH"}, {9, "NINTH"}};

HeatDiffusionAction::HeatDiffusionAction(const InputParameters & params)
  : HeatDiffusionActionBase(params),
    _temp(getParam<std::vector<VariableName>>("temp")),
    _ntemp(_temp.size()),
    _coupled_temp(_ntemp),
    _save_in(getParam<std::vector<AuxVariableName>>("save_in")),
    _diag_save_in(getParam<std::vector<AuxVariableName>>("diag_save_in")),
    _subdomain_names(getParam<std::vector<SubdomainName>>("block")),
    _subdomain_ids(),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _material_output_order(getParam<MultiMooseEnum>("material_output_order")),
    _material_output_family(getParam<MultiMooseEnum>("material_output_family")),
    _direction_valid(params.isParamSetByUser("direction")),
    _verbose(getParam<bool>("verbose"))
{

  // convert vector of VariableName to vector of VariableName
  for (unsigned int i = 0; i < _ntemp; ++i)
    _coupled_temp[i] = _temp[i];

  if (_save_in.size() != 0 && _save_in.size() != _ntemp)
    mooseError("Number of save_in variables should equal to the number of displacement variables ",
               _ntemp);

  if (_diag_save_in.size() != 0 && _diag_save_in.size() != _ntemp)
    mooseError(
        "Number of diag_save_in variables should equal to the number of displacement variables ",
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

  if (!getParam<bool>("add_variables") && params.isParamSetByUser("scaling"))
    paramError("scaling",
               "The scaling parameter has no effect unless add_variables is set to true. Did you "
               "mean to set 'add_variables = true'?");

  // Get direction for tensor component if set by user
  if (_direction_valid)
    _direction = getParam<Point>("direction");
}

void
HeatDiffusionAction::act()
{
  std::string ad_prepend = "";
  if (_use_ad)
    ad_prepend = "AD";

  // Consistency checks across subdomains
  actSubdomainChecks();

  // Gather info from all other HeatDiffusionAction
  actGatherActionParameters();

  // Deal with the optional AuxVariable based tensor quantity output
  actOutputGeneration();

  // Meta action which optionally spawns other actions
  if (_current_task == "meta_action")
  {
  }

  // Add variables
  else if (_current_task == "add_variable")
  {
    if (getParam<bool>("add_variables"))
    {
      auto params = _factory.getValidParams("MooseVariable");
      // determine necessary order
      const bool second = _problem->mesh().hasSecondOrderElements();

      params.set<MooseEnum>("order") = second ? "SECOND" : "FIRST";
      params.set<MooseEnum>("family") = "LAGRANGE";
      if (isParamValid("scaling"))
        params.set<std::vector<Real>>("scaling") = {getParam<Real>("scaling")};

      // Loop through the displacement variables
      for (const auto & disp : _temp)
      {
        // Create displacement variables
        _problem->addVariable("MooseVariable", disp, params);
      }
    }
  }
  // Add Materials
  else if (_current_task == "add_master_action_material")
  {
    actTempGradMat();
  }

  // Add Stress Divergence (and optionally WeakPlaneStress) Kernels
  else if (_current_task == "add_kernel")
  {
    for (unsigned int i = 0; i < _ntemp; ++i)
    {
      auto tensor_kernel_type = getKernelType();
      auto params = getKernelParameters(ad_prepend + tensor_kernel_type);

      std::string kernel_name = "VMNT_" + name() + Moose::stringify(i);

      // params.set<unsigned int>("component") = i;

      params.set<NonlinearVariableName>("variable") = _temp[i];

      if (_save_in.size() == _ntemp)
        params.set<std::vector<AuxVariableName>>("save_in") = {_save_in[i]};
      if (_diag_save_in.size() == _ntemp)
        params.set<std::vector<AuxVariableName>>("diag_save_in") = {_diag_save_in[i]};

      _problem->addKernel(ad_prepend + tensor_kernel_type, kernel_name, params);
    }
  }
  else if (_current_task == "add_scalar_kernel")
  {
  }
  else if (_current_task == "add_user_object")
  {
  }
}

void
HeatDiffusionAction::actSubdomainChecks()
{
  // Do the coordinate system check only once the problem is created
  if (_current_task == "setup_mesh_complete")
  {
    // get subdomain IDs
    for (auto & name : _subdomain_names)
      _subdomain_ids.insert(_mesh->getSubdomainID(name));
  }

  if (_current_task == "validate_coordinate_systems")
  {
    // use either block restriction list or list of all subdomains in the mesh
    const auto & check_subdomains =
        _subdomain_ids.empty() ? _problem->mesh().meshSubdomains() : _subdomain_ids;
    if (check_subdomains.empty())
      mooseError("No subdomains found");

    // make sure all subdomains are using the same coordinate system
    _coord_system = _problem->getCoordSystem(*check_subdomains.begin());
    for (auto subdomain : check_subdomains)
      if (_problem->getCoordSystem(subdomain) != _coord_system)
        mooseError("The HeatDiffusion action requires all subdomains to have the same coordinate "
                   "system.");
  }
}

void
HeatDiffusionAction::actOutputGeneration()
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

      index++;
    }
  }

  // Add output AuxKernels
  else if (_current_task == "add_aux_kernel")
  {
    std::string ad_prepend = _use_ad ? "AD" : "";
    // Loop through output aux variables
    unsigned int index = 0;
    for (auto out : _generate_output)
    {
      if (_material_output_family[index] == "MONOMIAL")
      {
        InputParameters params = emptyInputParameters();
     
         // RankOneCartesianComponent
        if ([&]() {
            for (const auto & r2q : _rank_one_cartesian_component_table)
              for (unsigned int a = 0; a < 3; ++a)
                  if (r2q.first + '_' + _component_table[a] == out)
                  {
                    params = _factory.getValidParams("MaterialRealVectorValueAux");
                    params.applyParameters(parameters());
                    params.set<MaterialPropertyName>("property") = _base_name + r2q.second;
                    params.set<AuxVariableName>("variable") = _base_name + out;
                    params.set<unsigned int>("component") = a;
                    params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_END;

                    _problem->addAuxKernel(
                        ad_prepend + "MaterialRealVectorValueAux", _base_name + out + '_' + name(), params);
                    return true;
                  }
            return false;
          }())
        continue;

        paramError("generate_output", "Unable to add output Material for '", out, "'");
      }
      index++;
    }
  }
}

void
HeatDiffusionAction::verifyOrderAndFamilyOutputs()
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
               << COLOR_DEFAULT << std::flush;

  // if no value was provided, chose the default MONOMIAL
  if (_material_output_family.size() == 0)
    _material_output_family.push_back("MONOMIAL");


  // ADD error check to only allow it to be MONOMIAL...

  // For only one family, make all families that value
  if (_material_output_family.size() == 1)
    _material_output_family =
        std::vector<std::string>(_generate_output.size(), _material_output_family[0]);

  if (_verbose)
    Moose::out << COLOR_CYAN << "*** Automatic applied material output families ***"
               << "\n"
               << _name << ": " << Moose::stringify(_material_output_family) << "\n"
               << COLOR_DEFAULT << std::flush;
}

void
HeatDiffusionAction::actGatherActionParameters()
{
  // Gather info about all other master actions when we add variables
  if (_current_task == "validate_coordinate_systems" && getParam<bool>("add_variables"))
  {
    auto actions = _awh.getActions<HeatDiffusionAction>();
    for (const auto & action : actions)
    {
      const auto size_before = _subdomain_id_union.size();
      const auto added_size = action->_subdomain_ids.size();
      _subdomain_id_union.insert(action->_subdomain_ids.begin(), action->_subdomain_ids.end());
      const auto size_after = _subdomain_id_union.size();

      if (size_after != size_before + added_size)
        mooseError("The block restrictions in the TensorMechanics/Master actions must be "
                   "non-overlapping.");

      if (added_size == 0 && actions.size() > 1)
        mooseError("No TensorMechanics/Master action can be block unrestricted if more than one "
                   "TensorMechanics/Master action is specified.");
    }
  }
}

void
HeatDiffusionAction::actTempGradMat()
{
  std::string type = "ComputeTempGrad";
  auto params = _factory.getValidParams(type);

  if (isParamValid("grad_base_name"))
    params.set<std::string>("base_name") = getParam<std::string>("grad_base_name");

  params.set<std::vector<VariableName>>("temp") = _coupled_temp;

  _problem->addMaterial(type, name() + "_strain", params);
}


std::string
HeatDiffusionAction::getKernelType()
{
  return "HeatDiffusion";
}

InputParameters
HeatDiffusionAction::getKernelParameters(std::string type)
{
  InputParameters params = _factory.getValidParams(type);
  params.applyParameters(
      parameters(),
      {"temp", "use_displaced_mesh", "save_in", "diag_save_in"});

  // Diffusion doesn't need a vector of displacements coupling; this would an an unregistered parameter
  // params.set<std::vector<VariableName>>("temp") = _coupled_temp;

  params.set<bool>("use_displaced_mesh") = _use_displaced_mesh;

  return params;
}
