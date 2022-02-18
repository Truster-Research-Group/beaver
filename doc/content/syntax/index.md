# Beaver Input File Syntax

The Beaver application has access to the usual MOOSE objects as well as specially developed objects. The available objects and their input syntax are listed below, and can be browsed through the headers at the right.

Master actions are listed within the 'Beaver' Module near the top of the listing. 
These master actions simplify the input of sequenced gradient-flux-*kernel systems, 
grouped by governing PDE (e.g. VMNT = heat diffusion PDE) and subgrouped by kernel type (kernel, interface-kernel, scalar-kernel).

Low-level syntax of individual objects are listed further down.


!syntax complete groups=BeaverApp
