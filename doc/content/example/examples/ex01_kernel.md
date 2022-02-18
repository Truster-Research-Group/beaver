# Example 1 : Heat Diffusion Kernel Basic

## Problem Statement

This example shows the usage of the Heat Diffusion kernel and materials in a MOOSE input file
via the long form, without an action.

We consider the steady-state diffusion equation on the 2D domain $\Omega$: find $u$ such that
$-\nabla \cdot \mathbf{q}(u) = 0 \in \Omega$, and $\mathbf{q}(u) \cdot \hat{n} = 1$ on the right and $u = 0$ on the left boundaries. Flux-free conditions are on top and bottom.

## Input File Syntax

This input file has several parts:

-  Mesh: geometry
-  Variables: solution field $u$
-  AuxVariables: for output of material $\mathbf{q}$
-  Kernels: weak form operators
-  AuxKernels: to extract the output material $\mathbf{q}$ to the AuxVariables
-  Materials: for heat flux, thermal gradient, and diffusivity
-  BCs
-  Executioner
-  Outputs

### Mesh

The domain for the problem is created with the `Mesh` block in the input file. The domain has
four square regions, having different diffusivity as listed later.

!listing examples/ex01_kernel/ex01.i block=Mesh

!media media/examples/ex01_mesh.png
       caption=Four block mesh
       style=width:40%;display:block;margin-left:auto;margin-right:auto;

### Variables

A single variable, 'u', is defined and approximated with linear Lagrange shape functions.

!listing examples/ex01_kernel/ex01.i block=Variables

### AuxVariables

Two manners are shown for extracting a component of heat flux $\mathbf{q}$ from the internal
properties of the system: a single component in block 1 and the entire vector in block 2.

!listing examples/ex01_kernel/ex01.i block=AuxVariables

### Kernels

The weak form of the problem statement is represented by `HeatDiffusion` Kernel objects.
Four instances of the Kernel are defined, each acting on the previously defined variable "u".
They are designated with arbitrary user-defined labels such as "diff1". Each Kernel is assigned
to one block by the "block = " parameter. Three of them have the parameter "base_name" set in
order to connect to the block-specific materials. As stated in the Syntax Documentation, the 
"base_name" for the `HeatDiffusion` Kernel, `ComputeIsotropicHeat` Material, and `ComputeTempGrad` Material
must be the same in order to connect. This is true regardless of which block the Materials are defined in,
as shown below.

-  Block 0: empty "base_name", Kernel 'diff0'
-  Block 1: "base_name=G1", Kernel 'diff1'
-  Block 2: "base_name=G2", Kernel 'diff2'
-  Block 3: "base_name=G3", Kernel 'diff3'

!listing examples/ex01_kernel/ex01.i block=Kernels

### AuxKernels

Two manners are shown for extracting a component of heat flux $\mathbf{q}$ from the internal
properties of the system: a single component in block 1 and the entire vector in block 2.

Note that the component-wise option is used by the Action system for `HeatDiffusion` with an 
automatic naming scheme.

!listing examples/ex01_kernel/ex01.i block=AuxKernels

### Materials

To allow for the kernel to refer to different implementation of heat fluxes, and for the heat
flux material to use different types of thermal gradients, separate `Materials` are defined for those
two objects and are connected through "base_name". A material and kernel must exist on the same block
in order to connect, but multiple kernels and materials with different base names can exist in a
block; also, a material might span several blocks.

Also, the `ComputeIsotropicHeat` Material requires a scalar "diffusivity" property which can be constant
or a spatial function. This property is a standard MOOSE object and also can span multiple blocks.

Each of those cases are provided in this file.

Thermal Gradient `ComputeTempGrad`

-  Material 'grad0': Block 0: empty "base_name" 
-  Material 'grad1': Blocks ALL: "base_name=G1" 
-  Material 'grad2': Blocks 1 & 2: "base_name=G2"  
-  Material 'grad3': Block 3: "base_name=G3"  

Diffusivity `GenericConstantMaterial`

-  Block 0: 'D0', value = 1
-  Block2 1 & 2: 'D12', value = 20
-  Block 3: 'D3', value = 1

Heat Flux `ComputeIsotropicHeat`

-  Material 'grad0': Block 0: empty "base_name"  diffusivity
-  Material 'grad1': Block 1: "base_name=G1" 
-  Material 'grad2': Blocks 1 & 2: "base_name=G2"  
-  Material 'grad3': Block 3: "base_name=G3" 

Notice that 'D12', 'flux2', 'grad1', and 'grad2' are defined over multiple blocks. The value of 'grad1'
is calculated everywhere but referenced only in block 1; 'D12' is used by both heat flux 'flux1' and 'flux2';
and 'flux2' and 'grad2' are computed in both blocks 1 & 2, but will only be referenced in block 2. Note that
trying to reference a material object within a block where it is not defined will result in an error.

!listing examples/ex01_kernel/ex01.i block=Materials

### Summary of linked Kernels and Materials

-  "base_name" empty: Block 0: Kernel 'diff0', Flux 'flux0', Diffusivity 'D0', Gradient 'grad0'
-  "base_name=G1": Block 1: Kernel 'diff1', Flux 'flux1', Diffusivity 'D12', Gradient 'grad1'
-  "base_name=G2": Block 2: Kernel 'diff2', Flux 'flux2', Diffusivity 'D12', Gradient 'grad2'
-  "base_name=G3": Block 3: Kernel 'diff3', Flux 'flux3', Diffusivity 'D3', Gradient 'grad3'


### Boundary Conditions (BCs)

Boundary conditions are defined in a manner similar to `Kernels`.  For this problem two Dirichlet
boundary conditions are required.  In the input file the two boundary conditions are specified
each using the `DirichletBC` object provided by MOOSE.

!listing examples/ex01_kernel/ex01.i block=BCs

Within each of the two sub-section, named "right" and "left" by the user, the boundary conditions
are linked to their associated variable(s) (i.e. "diffused" in this case) and boundaries.  The MOOSE
mesh generators define the labeled  boundaries "right" and "left" which we can refer to. 
Boundaries will also often be specified by number IDs - depending on how your mesh file was generated.

Note, the Neumann boundary conditions for this problem (on the top and bottom sides) are satisfied
implicitly and are not necessary for us to define. However, for non-zero Neumann or other boundary
conditions many built-in objects are provided by MOOSE (e.g., `NeumannBC`). You can also create
custom boundary conditions derived from the existing objects within MOOSE.

### Executioner

The type of problem to solve and the method for solving it is defined within the `Executioner`
block.  This problem is steady-state and will use the `Steady` Executioner and will use the
default solving method Preconditioned Jacobain Free Newton Krylov.

!listing examples/ex01_kernel/ex01.i block=Executioner

### Outputs

Here two types of outputs are enabled: output to the screen (console) and output to an Exodus II
file (exodus).

!listing examples/ex01_kernel/ex01.i block=Outputs

## Running the Problem

This example may be run using the following commands form the command line.

```bash
cd ~/projects/beaver/examples/ex01_kernel
make -j8
../../beaver-opt -i ex01.i
```

This will generate the results file, ex01_out.e, as shown in [example-1-results]. This file may be viewed using
Paraview, an external application that supports the Exodus II format.

!media media/examples/ex01_results.png
       id=example-1-results
       caption= Example 1 Results
       style=width:50%;display:block;margin-left:auto;margin-right:auto;

