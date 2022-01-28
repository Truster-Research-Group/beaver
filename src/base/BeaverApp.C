#include "BeaverApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
BeaverApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  // Do not use legacy material output, i.e., output properties on INITIAL as well as TIMESTEP_END
  params.set<bool>("use_legacy_material_output") = false;

  return params;
}

BeaverApp::BeaverApp(InputParameters parameters) : MooseApp(parameters)
{
  BeaverApp::registerAll(_factory, _action_factory, _syntax);
}

BeaverApp::~BeaverApp() {}

static void
associateSyntaxInner(Syntax & syntax, ActionFactory & /*action_factory*/)
{
  registerSyntax("CommonHeatDiffusionAction", "Beaver/VMNT/HeatDiff/");
  registerSyntax("HeatDiffusionAction", "Beaver/VMNT/HeatDiff/*");

  registerTask("output_penetration_info_vars", false);
  syntax.addDependency("output_penetration_info_vars", "add_output");
}

void
BeaverApp::registerAll(Factory & f, ActionFactory & af, Syntax & syntax)
{
  ModulesApp::registerAll(f, af, syntax);
  Registry::registerObjectsTo(f, {"BeaverApp"});
  Registry::registerActionsTo(af, {"BeaverApp"});

  /* register custom execute flags, action syntax, etc. here */
  associateSyntaxInner(syntax, af);
}

void
BeaverApp::registerApps()
{
  registerApp(BeaverApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
BeaverApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  BeaverApp::registerAll(f, af, s);
}
extern "C" void
BeaverApp__registerApps()
{
  BeaverApp::registerApps();
}
