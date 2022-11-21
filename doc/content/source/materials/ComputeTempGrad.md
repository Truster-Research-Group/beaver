# ComputeTempGrad

!syntax description /Materials/ComputeTempGrad

## Description

The ComputeTempGrad material borrows from the Tensor Mechanics module philosophy of spliting the differential operator, flux variable, and primal gradient variable into separate MOOSE objects linked by a 'base_name'. 
The first example, [Example 1](../../example/examples/ex01_kernel.md), in BEAVER shows how to manually connect the kernel and the two material objects in this sequence. 

!listing examples/ex01_kernel/ex01.i block=Materials/grad1

This material consumes a temperature variable 'temp' and returns the thermal gradient $\nabla T$ and places it in the variable 'grad_T'.

The thermal gradient vector vector will be comsumed through a heat flux material object such as ComputeIsotropicHeat and is linked using the parameter 'base_name', whose default value is the empty string.

!syntax parameters /Materials/ComputeTempGrad

!syntax inputs /Materials/ComputeTempGrad

!syntax children /Materials/ComputeTempGrad
