//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "TracInterActionBase.h"

class TracInterAction : public TracInterActionBase
{
public:
  static InputParameters validParams();
  TracInterAction(const InputParameters & params);

  /// Method adding the proper relationship manager
  using Action::addRelationshipManagers;
  virtual void addRelationshipManagers(Moose::RelationshipManagerType input_rm_type) override;

  void act() override;

protected:
  void actOutputGeneration();
  void actOutputMatProp();

  /// adds the required interfacekernels based on the selected VMNS formulation
  void addRequiredVMNSInterfaceKernels();

  /// adds the required interface materials based on the selected VMNS formulation
  void addRequiredVMNSInterfaceMaterials();

  /// verifies order and family of output variables
  void verifyOrderAndFamilyOutputs();

  /// method to prepare save_in and diag_save_in inputs for the interface kernel
  void prepareSaveInInputs(std::vector<AuxVariableName> & /*save_in_names*/,
                           std::string & /*save_in_side*/,
                           const std::vector<AuxVariableName> & /*var_name_master*/,
                           const std::vector<AuxVariableName> & /*var_name_slave*/,
                           const int & /*i*/) const;

  /// method checking multiple TracInterAction block inputs
  void chekMultipleActionParameters();

  template <typename T, typename T2>
  bool setupOutput(std::string out, T table, T2 setup);

  /// the displacement variable names
  std::vector<VariableName> _displacements;

  /// number of displacement components
  const unsigned int _ndisp;

  /// Base name of the material system
  /// Constructed WITH a "_" appended
  const std::string _base_name;

  /// Base name of heat flux on element side
  /// Constructed WITHOUT a "_" appended
  const std::string _base_name_e;

  /// Base name of heat flux on neighbor side
  /// Constructed WITHOUT a "_" appended
  const std::string _base_name_n;

  /// Base name of the material system
  const std::vector<BoundaryName> _boundary;

  /// Traction average formulation
  enum class TracAver
  {
    Simple,
    Complex
  } _tracaver;

  /// Symmetrizing parameter, called theta or beta in some papers
  /// 0=incomplete IIPG, -1=symmetric SIPG, +1=non-symmetric NIPG
  const int _nis_flag;

  /// If true use traction penalty term
  const bool _use_trac_penalty;

  ///@{ residual debugging
  std::vector<AuxVariableName> _save_in_master;
  std::vector<AuxVariableName> _diag_save_in_master;
  std::vector<AuxVariableName> _save_in_slave;
  std::vector<AuxVariableName> _diag_save_in_slave;
  ///@}

  /// kernel's and materials's names
  ///@{
  std::string _vmns_kernel_name;
  std::string _disp_jump_provider_name;
  std::string _equilibrium_traction_calculator_name;
  ///@}

  /// output materials to generate scalar traction/jump vector quantities
  std::vector<std::string> _generate_output;
  MultiMooseEnum _material_output_order;
  MultiMooseEnum _material_output_family;

  /// set generated from the combined boundary restrictions of all TracInterAction action blocks
  std::set<BoundaryName> _boundary_name_union;

  /// set generated from the combined boundary restrictions of all TracInterAction action blocks
  std::set<TracAver> _trac_formulation_union;

  /// display info
  const bool _verbose;
};

template <typename T, typename T2>
bool
TracInterAction::setupOutput(std::string out, T table, T2 setup)
{
  for (const auto & t1 : table)
  {
    // find the officially supported properties
    for (const auto & t2 : t1.second.second)
      if (t1.first + '_' + t2 == out)
      {
        const auto it = _real_vector_cartesian_component_table.find(t2);
        if (it != _real_vector_cartesian_component_table.end())
        {
          setup(it->second, t1.second.first);
          return true;
        }
        else
          mooseError("Internal error. The permitted vector shortcuts must be keys in the "
                     "'_real_vector_cartesian_component_table'.");
      }

    // check for custom properties
    auto prefix = t1.first + '_';
    if (out.substr(0, prefix.length()) == prefix)
    {
      setup(out.substr(prefix.length()), t1.second.first);
      return true;
    }
  }

  return false;
}
