# VMNS Traction Interface System

!syntax description /Beaver/VMNS/TracInter/TracInterAction

The Beaver VMNS Traction Interface Master Action is a convenience object to simplify setting up a VMNS traction interface system.

### Subblocks

The subblocks of the TracInter action are what triggers MOOSE objects to be built.
If the solid domain base names and the interface traction averaging type are similar and thus not boundary restricted, a single subblock can be used:

!listing test/tests/actions/traction_inter/vmns_simple1.i block=TracInter

or if different interface models are needed, multiple subblocks with 
boundary restrictions can be used.

!listing test/tests/actions/traction_inter/vmns_bn_act_aux.i block=TracInter

Parameters supplied at the `[Beaver/VMNS/TracInter]` level act as
defaults for the TracInter action subblocks.

!syntax parameters /Beaver/VMNS/TracInter

## Associated Actions

!syntax list /Beaver/VMNS/TracInter objects=True actions=False subsystems=False

!syntax list /Beaver/VMNS/TracInter objects=False actions=False subsystems=True

!syntax list /Beaver/VMNS/TracInter objects=False actions=True subsystems=False
