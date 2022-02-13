//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "VMNSInterfaceKernelBase.h"

InputParameters
VMNSInterfaceKernelBase::validParams()
{
  InputParameters params = InterfaceKernel::validParams();
  params.addRequiredParam<unsigned int>("component",
                                        "the component of the "
                                        "displacement vector this kernel is working on:"
                                        " component == 0, ==> X"
                                        " component == 1, ==> Y"
                                        " component == 2, ==> Z");
  params.set<bool>("_use_undisplaced_reference_points") = true;
  params.suppressParameter<bool>("use_displaced_mesh");
  params.addRequiredCoupledVar("displacements", "the string containing displacement variables");
  params.addParam<std::string>("base_name", "Material property base name");
  params.addParam<int>("nis_flag", 0, "Integer for IIPG=0, SIPG=-1, or NIPG=+1 method");
  params.addParam<bool>("use_flux_penalty", false, "Use heat flux jump penalty");

  return params;
}

VMNSInterfaceKernelBase::VMNSInterfaceKernelBase(const InputParameters & parameters)
  : JvarMapKernelInterface<InterfaceKernel>(parameters),
    _base_name(isParamValid("base_name") && !getParam<std::string>("base_name").empty()
                   ? getParam<std::string>("base_name") + "_"
                   : ""),
    _component(getParam<unsigned int>("component")),
    _ndisp(coupledComponents("displacements")),
    _disp_var(_ndisp),
    _disp_neighbor_var(_ndisp),
    _vars(_ndisp),
    _nis_flag(getParam<int>("nis_flag")),
    _use_flux_penalty(getParam<bool>("use_flux_penalty"))
{
  // Enforce consistency
  if (_ndisp != _mesh.dimension())
    paramError("displacements", "Number of displacements must match problem dimension.");

  if (_ndisp > 3 || _ndisp < 1)
    mooseError("the VMNS material requires 1, 2 or 3 displacement variables");

  /// ADD error check that same shape functions used for all components

  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    _disp_var[i] = coupled("displacements", i);
    _disp_neighbor_var[i] = coupled("displacements", i);
    _vars[i] = getVar("displacements", i);
  }
}

Real
VMNSInterfaceKernelBase::computeQpResidual(Moose::DGResidualType type)
{

  Real r = 0.0;

  /// Stability/penalty term for residual
  r += computeResiStab(type);
  /// Consistency term for residual
  r += computeResiCons(type);
  /// Symmetrizing/adjoint term for residual
  if (_nis_flag != 0)
    r += computeResiSymm(type);
  /// Flux/penalty term for residual
  if (_use_flux_penalty)
    r += computeResiFlux(type);

  return r;
}

Real
VMNSInterfaceKernelBase::computeQpJacobian(Moose::DGJacobianType type)
{
  Real j = 0.0;

  /// Stability/penalty term for Jacobian
  j += computeJacoStab(_component, type);
  /// Consistency term for Jacobian
  j += computeJacoCons(_component, type);
  /// Symmetrizing/adjoint term for Jacobian
  if (_nis_flag != 0)
    j += computeJacoSymm(_component, type);
  /// Flux/penalty term for Jacobian
  if (_use_flux_penalty)
    j += computeJacoFlux(_component, type);
  /// Damage term for Jacobian
  j += computeJacoDebo(_component, type);

  return j;
}

Real
VMNSInterfaceKernelBase::computeQpOffDiagJacobian(Moose::DGJacobianType type, unsigned int jvar)
{
  Real j = 0.0;

  // bail out if jvar is not coupled
  if (getJvarMap()[jvar] < 0)
    return 0.0;

  // Jacobian of the residul[_component] w.r.t to the coupled displacement
  // component[off_diag_component]
  for (unsigned int off_diag_component = 0; off_diag_component < _ndisp; ++off_diag_component)
  {
    if (jvar == _disp_var[off_diag_component])
    {
      /// Stability/penalty term for Jacobian
      j += computeJacoStab(off_diag_component, type);
      /// Consistency term for Jacobian
      j += computeJacoCons(off_diag_component, type);
      /// Symmetrizing/adjoint term for Jacobian
      if (_nis_flag != 0)
        j += computeJacoSymm(off_diag_component, type);
      /// Flux/penalty term for Jacobian
      if (_use_flux_penalty)
        j += computeJacoFlux(off_diag_component, type);
      /// Damage term for Jacobian
      j += computeJacoDebo(off_diag_component, type);
      return j;
    }
  }

  // this is the place where one should implement derivatives of the residual w.r.t. other variables
  return 0.0;
}

RankTwoTensor
VMNSInterfaceKernelBase::gradOp(const unsigned int m, const RealGradient & grad) const
{
  RankTwoTensor G;
  for (size_t j = 0; j < _ndisp; j++)
    G(m, j) = grad(j);

  return G;
}
