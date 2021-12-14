//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "BeaverTestApp.h"
#include "BeaverApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

InputParameters
BeaverTestApp::validParams()
{
  InputParameters params = BeaverApp::validParams();
  return params;
}

BeaverTestApp::BeaverTestApp(InputParameters parameters) : MooseApp(parameters)
{
  BeaverTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

BeaverTestApp::~BeaverTestApp() {}

void
BeaverTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  BeaverApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"BeaverTestApp"});
    Registry::registerActionsTo(af, {"BeaverTestApp"});
  }
}

void
BeaverTestApp::registerApps()
{
  registerApp(BeaverApp);
  registerApp(BeaverTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
BeaverTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  BeaverTestApp::registerAll(f, af, s);
}
extern "C" void
BeaverTestApp__registerApps()
{
  BeaverTestApp::registerApps();
}
