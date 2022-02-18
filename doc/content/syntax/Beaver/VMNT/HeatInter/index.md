# VMNT Heat Flux Interface System

!syntax description /Beaver/VMNT/HeatInter/HeatInterAction

The Beaver VMNS Heat Flux Interface Master Action is a convenience object to simplify setting up a heat diffusion interface system.

### Subblocks

The subblocks of the HeatInter action are what triggers MOOSE objects to be built.
If the diffusion is similar and thus not boundary restricted, a single subblock can be used:

!listing test/tests/actions/heat_inter/vmnt_simple1.i block=HeatInter

or if different interface models are needed, multiple subblocks with 
boundary restrictions can be used.

!listing test/tests/actions/heat_inter/vmnt_bn_act_aux.i block=HeatInter

Parameters supplied at the `[Beaver/VMNT/HeatInter]` level act as
defaults for the HeatInter action subblocks.

!syntax parameters /Beaver/VMNT/HeatInter

## Associated Actions

!syntax list /Beaver/VMNT/HeatInter objects=True actions=False subsystems=False

!syntax list /Beaver/VMNT/HeatInter objects=False actions=False subsystems=True

!syntax list /Beaver/VMNT/HeatInter objects=False actions=True subsystems=False
