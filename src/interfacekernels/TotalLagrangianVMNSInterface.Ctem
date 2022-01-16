//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TotalLagrangianVMNSInterface.h"

registerMooseObject("BeaverApp", TotalLagrangianVMNSInterface);

InputParameters
TotalLagrangianVMNSInterface::validParams()
{
  InputParameters params = VMNSInterfaceKernelBase::validParams();

  params.addClassDescription("Total Lagrangian interface equilibrium kernel");

  // This kernel requires use_displaced_mesh to be off
  params.suppressParameter<bool>("use_displaced_mesh");

  return params;
}

TotalLagrangianVMNSInterface::TotalLagrangianVMNSInterface(const InputParameters & parameters)
  : VMNSInterfaceKernelBase(parameters),
    _pk1_E(getMaterialPropertyByName<RankTwoTensor>(_base_name + "pk1_stress")),
    _pk1_N(getNeighborMaterialPropertyByName<RankTwoTensor>(_base_name + "pk1_stress")),
    _dpk1_E(getMaterialPropertyByName<RankFourTensor>(_base_name + "pk1_jacobian")),
    _dpk1_N(getNeighborMaterialPropertyByName<RankFourTensor>(_base_name + "pk1_jacobian")),
    _avg_grad_trial_E(_grad_phi.size()),
    _avg_grad_trial_N(_grad_phi_neighbor.size()),
    _unstabilized_def_grad_E(
        getMaterialPropertyByName<RankTwoTensor>(_base_name + "unstabilized_deformation_gradient")),
    _unstabilized_def_grad_N(
        getNeighborMaterialPropertyByName<RankTwoTensor>(_base_name + "unstabilized_deformation_gradient")),
    _aF_E(getMaterialPropertyByName<RankTwoTensor>(_base_name + "avg_deformation_gradient")),
    _aF_N(getNeighborMaterialPropertyByName<RankTwoTensor>(_base_name + "avg_deformation_gradient")),
    _inv_inc_def_grad_E(getMaterialPropertyByName<RankTwoTensor>(_base_name + "inv_inc_def_grad")),
    _inv_inc_def_grad_N(getNeighborMaterialPropertyByName<RankTwoTensor>(_base_name + "inv_inc_def_grad")),
    _def_grad_E(getMaterialPropertyByName<RankTwoTensor>(_base_name + "deformation_gradient")),
    _def_grad_N(getNeighborMaterialPropertyByName<RankTwoTensor>(_base_name + "deformation_gradient"))
{
}
// something like this
Real
TotalLagrangianVMNSInterface::computeQpResidual(Moose::DGResidualType type)
{
  Real r = 0;

  Real jump_value = 0;

  if (_jump != nullptr)
  {
    jump_value = (*_jump)[_qp];
    mooseAssert(jump_value == _u[_qp] - _neighbor_value[_qp], "");
  }
  else
    jump_value = _u[_qp] - _neighbor_value[_qp];

  switch (type)
  {
    case Moose::Element:
      r = _test[_i][_qp] * _penalty * jump_value;
      break;

    case Moose::Neighbor:
      r = _test_neighbor[_i][_qp] * -_penalty * jump_value;
      break;
  }

  return r;
  // return _pk1[_qp].doubleContraction(testGrad(_component));
}

Real
TotalLagrangianVMNSInterface::computeQpJacobian(Moose::DGJacobianType type)
{
  Real jac = 0;

  switch (type)
  {
    case Moose::ElementElement:
      jac = _test[_i][_qp] * _penalty * _phi[_j][_qp];
      break;

    case Moose::ElementNeighbor:
      jac = _test[_i][_qp] * _penalty * -_phi_neighbor[_j][_qp];
      break;

    case Moose::NeighborElement:
      jac = _test_neighbor[_i][_qp] * -_penalty * _phi[_j][_qp];
      break;

    case Moose::NeighborNeighbor:
      jac = _test_neighbor[_i][_qp] * -_penalty * -_phi_neighbor[_j][_qp];
      break;
  }

  return jac;
  return materialJacobian(testGrad(_component), trialGrad(_component), type);
}

Real
TotalLagrangianVMNSInterface::computeQpOffDiagJacobian(Moose::DGJacobianType type, unsigned int jvar)
{
  for (unsigned int cc = 0; cc < _ndisp; cc++)
    if (jvar == _disp_nums[cc])
      return materialJacobian(testGrad(_component), trialGrad(cc), type);

  // Bail if jvar not coupled
  if (getJvarMap()[jvar] < 0)
    return 0.0;

  // Off diagonal temperature term due to eigenstrain
  // return eigenstrainJacobianComponent(
  //     mapJvarToCvar(jvar), _dpk1[_qp], testGrad(_component), _temperature->phi()[_j][_qp]);
}

void
TotalLagrangianVMNSInterface::precalculateJacobian()
{
  if (_stabilize_strain)
    computeAverageGradPhi();
}

RankTwoTensor
TotalLagrangianVMNSInterface::stabilizeGrad(const RankTwoTensor & Gb, const RankTwoTensor & Ga)
{
  // Take the base gradient (Gb) and the average gradient (Ga) and
  // stabilize

  // Stabilization depends on the kinematics
  if (_large_kinematics)
  {
    // Horrible thing, see the documentation for how we get here
    const Real dratio = std::pow(_aF[_qp].det() / _unstabilized_def_grad[_qp].det(), 1.0 / 3.0);
    const Real fact = (_aF[_qp].inverse().transpose().doubleContraction(Ga) -
                       _unstabilized_def_grad[_qp].inverse().transpose().doubleContraction(Gb)) /
                      3.0;
    return dratio * (Gb + fact * _unstabilized_def_grad[_qp]);
  }
  // Small strain modification is linear
  return Gb + (Ga.trace() - Gb.trace()) / 3.0 * RankTwoTensor::Identity();
}

Real
TotalLagrangianVMNSInterface::materialJacobian(const RankTwoTensor & grad_test,
                                               const RankTwoTensor & grad_trial,
                                               Moose::DGJacobianType type)
{
  Real jac = 0;

  switch (type)
  {
    case Moose::ElementElement:
      jac = _test[_i][_qp] * _penalty * _phi[_j][_qp];
      break;

    case Moose::ElementNeighbor:
      jac = _test[_i][_qp] * _penalty * -_phi_neighbor[_j][_qp];
      break;

    case Moose::NeighborElement:
      jac = _test_neighbor[_i][_qp] * -_penalty * _phi[_j][_qp];
      break;

    case Moose::NeighborNeighbor:
      jac = _test_neighbor[_i][_qp] * -_penalty * -_phi_neighbor[_j][_qp];
      break;
  }

  return jac;
  // return grad_test.doubleContraction(_dpk1[_qp] * grad_trial);
}

Real
TotalLagrangianVMNSInterface::eigenstrainJacobianComponent(unsigned int cvar,
                                                              const RankFourTensor & C,
                                                              const RankTwoTensor & grad_test,
                                                              const Real & phi)
{
  // Multiple eigenstrains may depend on the same coupled var
  RankTwoTensor total_deigen;
  for (const auto deigen_darg : _deigenstrain_dargs[cvar])
    total_deigen += (*deigen_darg)[_qp];

  // This kernel needs a switch on large versus small deformations
  RankTwoTensor A;
  RankTwoTensor B;
  if (_large_kinematics)
  {
    A = RankTwoTensor::Identity();
    B = RankTwoTensor::Identity();
  }
  else
  {
    A = _inv_inc_def_grad[_qp].inverse();
    B = _def_grad[_qp];
  }

  RankFourTensor U = 0.5 * (A.mixedProductIkJl(B.transpose()) + A.mixedProductIlJk(B.transpose()));

  return -(C * U * total_deigen).doubleContraction(grad_test) * phi;
}

RankTwoTensor
TotalLagrangianVMNSInterface::testGrad(unsigned int i)
{
  return gradOp(i, _grad_test[_i][_qp]);
}

RankTwoTensor
TotalLagrangianVMNSInterface::trialGrad(unsigned int k)
{
  // The trick here is for the standard solids formulation you can work
  // with trial function gradient vectors (i.e. don't worry about the
  // other displacement components).  However for the
  // stabilized methods the "trace" term introduces non-zeros on
  // k indices other than the current trial function index...
  if (_stabilize_strain)
    return fullGrad(k, _stabilize_strain, _grad_phi[_j][_qp], _avg_grad_trial[_j]);
  // Don't need the averaged value
  else
    return fullGrad(k, _stabilize_strain, _grad_phi[_j][_qp], RealVectorValue());
}

void
TotalLagrangianVMNSInterface::computeAverageGradPhi()
{
  // Use the generic "averager" to make the average
  avgGrad(_grad_phi, _avg_grad_trial);
}

void
TotalLagrangianVMNSInterface::avgGrad(const VariablePhiGradient & grads,
                                         std::vector<RealVectorValue> & res)
{
  // Average a gradient and store in res
  // This function does the averaging over the reference configuration,
  // so it's quite straightforward
  res.resize(grads.size());
  for (unsigned int beta = 0; beta < grads.size(); ++beta)
  {
    // Zero
    res[beta].zero();
    Real v = 0.0;

    // Sum over qp
    for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
    {
      v += _JxW[_qp] * _coord[_qp];
      res[beta] += _JxW[_qp] * _coord[_qp] * grads[beta][_qp];
    }

    // Divide by the volume
    res[beta] /= v;
  }
}
