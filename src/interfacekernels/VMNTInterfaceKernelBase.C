//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "VMNTInterfaceKernelBase.h"

InputParameters
VMNTInterfaceKernelBase::validParams()
{
  InputParameters params = InterfaceKernel::validParams();
  params.addClassDescription("Base class for interface kernel in VMNT formulations.");

  params.addParam<std::string>("base_name", "Interface base name for a diffusion, flux, and jump T sequence");
  params.addParam<int>("nis_flag", 0, "Integer for IIPG=0, SIPG=-1, or NIPG=+1 method");
  params.addParam<bool>("use_flux_penalty", false, "Use heat flux jump penalty");

  return params;
}

VMNTInterfaceKernelBase::VMNTInterfaceKernelBase(const InputParameters & parameters)
  : DerivativeMaterialInterface<InterfaceKernel>(parameters),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _nis_flag(getParam<int>("nis_flag")),
    _use_flux_penalty(getParam<bool>("use_flux_penalty"))
{
}

Real
VMNTInterfaceKernelBase::computeQpResidual(Moose::DGResidualType type)
{

  Real r = 0.0;

  /// Stability/penalty term for residual
  r += computeResiStab(type);
  /// Consistency term for residual
  r += computeResiCons(type);
  /// Symmetrizing/adjoint term for residual
  if (_nis_flag != 0)
    r += _nis_flag * computeResiSymm(type);
  /// Flux/penalty term for residual
  if (_use_flux_penalty)
    r += computeResiFlux(type);

  return r;
}

Real
VMNTInterfaceKernelBase::computeQpJacobian(Moose::DGJacobianType type)
{

  Real j = 0.0;
  
  /// Stability/penalty term for Jacobian
  j += computeJacoStab(type);
  /// Consistency term for Jacobian
  j += computeJacoCons(type);
  /// Symmetrizing/adjoint term for Jacobian
  if (_nis_flag != 0)
    j += _nis_flag * computeJacoSymm(type);
  /// Flux/penalty term for Jacobian
  if (_use_flux_penalty)
    j += computeJacoFlux(type);
  /// Damage term for Jacobian
  j += computeJacoDebo(type);

  return j;
}
