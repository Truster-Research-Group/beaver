# VMNSRealVectorCartesianComponent

!syntax description /Materials/VMNSRealVectorCartesianComponent

## Description

This is a Material model used to extract components of a real vector  defined on a VMNS interface in a Cartesian coordinate system. 
This can be used regardless of the coordinate system used by the model.

This Material model is set up by [TracInter](TracInter/index.md) automatically
when traction or displacement-jump components are requested in the generate_output parameter, but can also be set up directly by the user.  

The `VMNSRealVectorCartesianComponent` takes as arguments the values of the `index` to save into a MaterialProperty.  [eq:real_vecto_indices] shows the component values for each vector component.

!equation id=eq:real_vecto_indices
\V_{i} \implies \begin{bmatrix}
                      \V_{0} \\
                      \V_{1} \\
                      \V_{2}
                      \end{bmatrix}

!syntax parameters /Materials/VMNSRealVectorCartesianComponent

!syntax inputs /Materials/VMNSRealVectorCartesianComponent

!syntax children /Materials/VMNSRealVectorCartesianComponent
