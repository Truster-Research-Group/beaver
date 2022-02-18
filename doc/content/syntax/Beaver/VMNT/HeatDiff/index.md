# VMNT Heat Diffusion System

!syntax description /Beaver/VMNT/HeatDiff/HeatDiffusionAction

The Beaver VMNT Heat Diffusion Master Action is a convenience object to simplify setting up a heat diffusion domain system.


### Subblocks

The subblocks of the HeatDiff action are what triggers MOOSE objects to be built.
If the diffusion is similar and thus not block restricted, a single subblock can be used:

!listing test/tests/actions/heat_diffusion/action_all.i block=HeatDiff

or if different diffusion models are needed, multiple subblocks with subdomain restrictions
can be used.

!listing test/tests/actions/heat_diffusion/action_grad2.i block=HeatDiff

Parameters supplied at the `[Beaver/VMNT/HeatDiff]` level act as
defaults for the HeatDiff action subblocks.

!syntax parameters /Beaver/VMNT/HeatDiff

## Associated Actions

!syntax list /Beaver/VMNT/HeatDiff objects=True actions=False subsystems=False

!syntax list /Beaver/VMNT/HeatDiff objects=False actions=False subsystems=True

!syntax list /Beaver/VMNT/HeatDiff objects=False actions=True subsystems=False
