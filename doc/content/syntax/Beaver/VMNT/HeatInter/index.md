# VMNT Heat Flux Interface System

!syntax description /Beaver/VMNT/HeatInter/HeatInterAction

The Beaver Variational Multiscale Nitsche (VMN) Temperature, Heat Flux Interface Master Action is a convenience object to simplify setting up a heat diffusion interface system.

## Description

The `Beaver` system provides a discontinous interface modeling capability that can be used to introduce damageable models running at the interfaces between regions modeled with continuum finite elements. The VMN model system is formulated in terms of temperature jumps, heat fluxes, weighted-average type, and potentially damage evolution model. 

An instance of the VMN system is designated by a unique (optionally empty) strin `base_name` to link the six required ingredients together and distinguish from other instances of the VMN system:

1. `BreakMeshByBlockGenerator`
2. `ComputeTemperatureJump`
3. `ComputeHeatFluxAverJump`
4. `VMNTInterfaceKernel`
5. `GeometricStabilityParams` (optional, under development)
6. `DamagedTemperatureJump` (optional, under development)

See the bottom of the page for more about `base_name`.

Identical to the `TensorMechanics` Cohesive Zone Modeling (CZM) system, the [BreakMeshByBlockGenerator](BreakMeshByBlockGenerator.md) is utilized to create the cohesive zone interface  by splitting a monolithic mesh into blocks by adding the required nodes and boundaries between each block pair. The split mesh allows the computing of a temperature jump $\llbracket u \rrbracket$ at each quadrature point on the interface. The schematic below is an example of using `BreakMeshByBlockGenerator` on a 3-blocks, 2-dimensional mesh. The generated interfaces are highlighted in yellow.

!media media/tensor_mechanics/BreakMeshByBlock.png style=width:100%;

The `ComputeTemperatureJump` object computes the temperature jump across the VMN interface according to the selected formulation. 
The `ComputeHeatFluxAverJump` looks of the value of the heat flux from the element and neighbor side of the blocks adjacent to the interface and computes a weighted average and jump.
The `VMNTInterfaceKernel` imposes continuity of the temperature and heat flux fields (modulated by potential damage) by adding the proper residual to the system, and provides the analytic Jacobian.
The `GeometricStabilityParams` ...
The `DamagedTemperatureJump` ...

The `HeatInterAction` action automatically adds the proper `ComputeTemperatureJump`, `ComputeHeatFluxAverJump`, `VMNTInterfaceKernel`, and `GeometricStabilityParams` based on the `flux_type` parameter value (see inputs).
The flowchart below ...



!alert note
Even when using the `HeatInterAction` action it is the responsibility of the user to add the (optional) `DamagedTemperatureJump` constitutive model and `BreakMeshByBlockGenerator` in the input file.

### Supported Flux Average Formulations

The system supports two different flux average formulations:

1. `Simple`
2. `Complex`

Each type of formulations requires adding a different `ComputeTemperatureJumpLocal`, `VMNTInterfaceKernel`, `ComputeHeatFluxAverJump`, and `GeometricStabilityParams`.

The `Simple` formulations requires adding:

1. [ComputeTemperatureJumpLocal](ComputeTemperatureJumpLocal.md)
2. [VMNTInterfaceKernelSimple](VMNTInterfaceKernelSimple.md)
3. [ComputeHeatFluxAverJumpSimple](ComputeHeatFluxAverJumpSimple.md)
4. No stability parameter calculation, values of 1/2 and hard-coded penalty are used.

The `Complex` formulations requires adding:

1. Under Development
2. Under Development
3. Under Development
4. Under Development

As mentioned in previous section, the `HeatInterAction` action adds the appropriate objects depending on the selected heat flux formulation.

!alert note
Both the `Small Strain` and `Total Lagrangian` bulk formulations are compatible with these heat flux formulations, since it is the job of the bulk formulation to compute the heat flux vector either with a linear or nonlinear approach, and the numerical flux found in the Nitsche formulations is identical for linear or nonlinear flux measures when posed on the reference (undeformed) configuration.

## Example Input File Syntax

The following examples show how to use the `HeatInterAction` action.

### Subblocks

The subblocks of the HeatInter action are what triggers MOOSE objects to be built.
If the diffusion is similar and thus not boundary restricted, a single subblock can be used:
!listing test/tests/actions/heat_inter/vmnt_bn0_act_aux.i block=HeatInter

or if different interface models are needed, multiple subblocks with 
boundary restrictions can be used.

!listing test/tests/actions/heat_inter/vmnt_bn_act_aux.i block=HeatInter

Parameters supplied at the `[Beaver/VMNT/HeatInter]` level act as
defaults for the HeatInter action subblocks.

Typically, boundary-specific subblocks are needed due to one or both of the following:

- A call to `BreakMeshByBlockGenerator` with parameter `split_interface = true`
- Differing values of `base_name` in the BULK regions adjacent to the interfaces

See the section below to understand more about `base_name`.

!alert note
It is up to the user to determine ALL of the boundary names generated by `BreakMeshByBlockGenerator` in order to assign the potentially different VMN interface parameters or damage model. This is particularly important when the BULK regions have a non-empty `base_name`; it is recommended that scripting outside of MOOSE be utilized to detect and write down all the needed cases.

The `generate_output` parameter adds scalar quantities of the traction and displacement jump to the outputs. Available options are: `traction_x traction_y traction_z normal_traction tangent_traction jump_x jump_y jump_z normal_jump tangent_jump pk1_traction_x pk1_traction_y pk1_traction_z `.
The name `traction` refers to the Cauchy traction, `pk1_traction` refers to the the first Piola-Kirchoff traction, and `jump` refers to the displacement jump across the cohesive interface. All the above vectors are defined in the global coordinate system.
The `normal_traction` and `tangent_traction` are scalar values compute using [CZMRealVectorScalar](CZMRealVectorScalar.md) (the same is true for the `normal_jump` and `tangent_jump`).

### Understanding the "base_name" parameters

Three names are needed by the `ComputeHeatFluxAverJump` material:

1. `base_name` is the name that is pre-pended to all kernel, variable, and material objects generated by the action for this interface system instance
2. `base_name_e` is the name that was assigned by the `HeatDiffusionAction` to the BULK region on the Element side of the `boundary` specified for this system
3. `base_name_n` is the name that was assigned by the `HeatDiffusionAction` to the BULK region on the Neighbor side of the `boundary` specified for this system

The first `base_name` is a choice by the user of this action to prefix all the interface material objects of this interface instance. Two potential reasons for doing this are (1) have a different name of the variables in the output file or (2) have multiple VMN systems on the same `boundary` to deal with overlapping bulk physical systems. Otherwise, the user is fine to leave this parameter empty.

The other two names are needed so that the `heat_flux` material property value can be looked up "by name" for the quadrature points on both sides of the interface. Since the `HeatDiffusionAction` would have assigned a `base_name` to the BULK region on the Element side, this interface object needs to look up the properties with the same prefix, which is accomplished through the parameter `base_name_e`. Note that the `BreakMeshByBlockGenerator` ALWAYS designates the Element side of an interface/boundary be the block with a lower `blockID`, the user can infer which is the Element and which is the Neighbor side of any interface.

This system is general and allows mixing of blocks and boundaries with and without `base_name`; the file just gets complicated rather quickly. See the example which does NOT use the action:

!listing test/tests/interfacekernels/VMNT_simple/vmnt_bn_act_aux.i block=Materials

!alert note
It is the responsibility of the user to add the `base_name` to the (optional) `DamagedTemperatureJump` constitutive model that completes the instance of the VNM system.

!syntax parameters /Beaver/VMNT/HeatInter

## Associated Actions

!syntax list /Beaver/VMNT/HeatInter objects=True actions=False subsystems=False

!syntax list /Beaver/VMNT/HeatInter objects=False actions=False subsystems=True

!syntax list /Beaver/VMNT/HeatInter objects=False actions=True subsystems=False
