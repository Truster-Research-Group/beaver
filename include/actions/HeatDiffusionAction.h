//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "HeatDiffusionActionBase.h"
#include "libmesh/point.h"

/**
 * Master action for defining sequenced heat diffusion kernel,
 * material, and output auxvariables and auxkernels
 */
class HeatDiffusionAction : public HeatDiffusionActionBase
{
public:
  static InputParameters validParams();

  HeatDiffusionAction(const InputParameters & params);

  virtual void act();

protected:
  /// Consistency checks across subdomains
  void actSubdomainChecks();
  /// Deal with the optional AuxVariable based tensor quantity output
  void actOutputGeneration();
  /// Gather info from all other HeatDiffusionAction
  void actGatherActionParameters();
  /// Ensure material output order and family vectors are same size as generate output
  void verifyOrderAndFamilyOutputs();
  /// Create thermal gradient material object
  void actTempGradMat();

  /// Provide name of heat diffusion kernel to use, based on other input parameters
  virtual std::string getKernelType();
  /// Generate relevant parameters for the kernel
  virtual InputParameters getKernelParameters(std::string type);

  /**
   * Helper function to decode `generate_outputs` options using a "table" of
   * scalar output quantities and a "setup" lambda that performs the input parameter
   * setup for the output material object.
   */
  template <typename T, typename T2>
  bool setupOutput(std::string out, T table, T2 setup);

  ///@{ temperature variables
  std::vector<VariableName> _temp;

  /// Number of temperature variables
  unsigned int _ntemp;

  /// Coupled displacement variables
  std::vector<VariableName> _coupled_temp;
  ///@}

  ///@{ residual debugging
  std::vector<AuxVariableName> _save_in;
  std::vector<AuxVariableName> _diag_save_in;
  ///@}

  /// MOOSE coordinate system currently active in this 'problem'
  Moose::CoordinateSystemType _coord_system;

  /// if this vector is not empty the variables, kernels and materials are restricted to these subdomains
  std::vector<SubdomainName> _subdomain_names;

  /// set generated from the passed in vector of subdomain names
  std::set<SubdomainID> _subdomain_ids;

  /// set generated from the combined block restrictions of all VMNT/HeatDiff action blocks
  std::set<SubdomainID> _subdomain_id_union;

  /// base name for the current master action block
  const std::string _base_name;

  /// use displaced mesh (true unless _strain is SMALL)
  bool _use_displaced_mesh;

  ///@{ output materials to generate scalar stress/strain tensor quantities
  std::vector<std::string> _generate_output;
  MultiMooseEnum _material_output_order;
  MultiMooseEnum _material_output_family;
  ///@}

  /// boolean used to determine if cylindrical axis points are passed
  bool _direction_valid;
  
  /// boolean for log file listing of generated output materials
  bool _verbose;

  /// points used to determine axis of rotation for cyclindrical stress/strain quantities
  Point _direction;

  /// booleans used to determine if spherical center point is passed
  bool _spherical_center_point_valid;

  /// center point for spherical stress/strain quantities
  Point _spherical_center_point;

  // Helper to translate into MOOSE talk
  static const std::map<unsigned int, std::string> _order_mapper;
};

template <typename T, typename T2>
bool
HeatDiffusionAction::setupOutput(std::string out, T table, T2 setup)
{
  for (const auto & t1 : table)
  {
    // find the officially supported properties
    for (const auto & t2 : t1.second.second)
      if (t1.first + '_' + t2 == out)
      {
        const auto it = _rank_one_cartesian_component_table.find(t2);
        if (it != _rank_one_cartesian_component_table.end())
        {
          setup(it->second, t1.second.first);
          return true;
        }
        else
          mooseError("Internal error. The permitted tensor shortcuts must be keys in the "
                     "'_rank_one_cartesian_component_table'.");
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
