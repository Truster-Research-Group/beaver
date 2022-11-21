# ComputeIsotropicHeat

!syntax description /Materials/ComputeIsotropicHeat

## Description

The ComputeIsotropicHeat material borrows from the Tensor Mechanics module philosophy of spliting the differential operator, flux variable, and primal gradient variable into separate MOOSE objects linked by a 'base_name'. 
The first example, [Example 1](../../example/examples/ex01_kernel.md), in BEAVER shows how to manually connect the kernel and the two material objects in this sequence. 

!listing examples/ex01_kernel/ex01.i block=Materials/flux1

This material consumes a thermal gradient 'grad_T' computed by ComputeTempGrad and returns the heat flux vector $\mathbf{q}(T)$ using the linear isotropic definition:

!equation
\mathbf{q}(T) = \mathbf{K} \cdot \nabla T

Where the diffusivity tensor $\mathbf{K}$ is independent of $T$ and is obtained as $\mathbf{K} = D\mathbf{I}$ where $\mathbf{I}$ is the identity tensor.

The constant material diffusivity variable $D$ can be defined through a generic MOOSE property as follows:

!listing examples/ex01_kernel/ex01.i block=Materials/mat12

The heat flux vector $\mathbf{q}(T)$ links the kernel HeatDiffusion with the thermal gradient from ComputeTempGrad using the parameter 'base_name', whose default value is the empty string.

!syntax parameters /Materials/ComputeIsotropicHeat

!syntax inputs /Materials/ComputeIsotropicHeat

!syntax children /Materials/ComputeIsotropicHeat
