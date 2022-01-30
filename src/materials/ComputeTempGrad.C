//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeTempGrad.h"
#include "Assembly.h"
#include "libmesh/quadrature.h"

registerMooseObject("BeaverApp", ComputeTempGrad);

InputParameters
ComputeTempGrad::validParams()
{
  InputParameters params = ComputeTempGradBase::validParams();
  params.addClassDescription("Compute a linear thermal gradient.");
  return params;
}

ComputeTempGrad::ComputeTempGrad(const InputParameters & parameters)
  : ComputeTempGradBase(parameters)
{
}

void
ComputeTempGrad::computeProperties()
{
  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
  {

    _hold_t[_qp] = _grad_temp[_qp];
  }

  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
  {

    _grad_t[_qp] = _hold_t[_qp];
  }
}
