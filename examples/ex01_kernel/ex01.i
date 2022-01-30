[Mesh]
  [./lowlef]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 8
    xmax = 8
    ny = 8
    ymax = 8
    elem_type = QUAD4
  [../]
  [./lowrig]
    type = SubdomainBoundingBoxGenerator
    input = 'lowlef'
    block_id = 1
    bottom_left = '4 0 0'
    top_right = '8 4 4'
  [../]
  [./upplef]
    type = SubdomainBoundingBoxGenerator
    input = 'lowrig'
    block_id = 2
    bottom_left = '0 4 0'
    top_right = '4 8 4'
  [../]
  [./upprig]
    type = SubdomainBoundingBoxGenerator
    input = 'upplef'
    block_id = 3
    bottom_left = '4 4 0'
    top_right = '8 8 4'
  [../]
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[AuxVariables]
  [./postflux10]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./postflux2]
    order = CONSTANT
    family = MONOMIAL_VEC
    block = 2
  [../]
[]

[Kernels]
  [./diff0]
    type = HeatDiffusion
    variable = u
    block = 0
  [../]
  [./diff1]
    type = HeatDiffusion
    variable = u
    base_name = G1
    block = 1
  [../]
  [./diff2]
    type = HeatDiffusion
    variable = u
    base_name = G2
    block = 2
  [../]
  [./diff3]
    type = HeatDiffusion
    variable = u
    base_name = G3
    block = 3
  [../]
[]

[AuxKernels]
  [./postflux10]
    type = MaterialRealVectorValueAux
    property = G1_heat_flux
    variable = postflux10
    component = 0
    block = 1
  [../]
  [./postflux2]
    type = VectorMaterialRealVectorValueAux
    property = G2_heat_flux
    variable = postflux2
    block = 2
  [../]
[]

[Materials]
  [./mat0]
    type = GenericConstantMaterial
    prop_names = 'D0'
    prop_values = '1'
    block = 0
  []
  [./mat12]
    type = GenericConstantMaterial
    prop_names = 'D12'
    prop_values = '20'
  []
  [./mat3]
    type = GenericConstantMaterial
    prop_names = 'D3'
    prop_values = '1'
  [../]
  [./flux0]
    type = ComputeIsotropicHeat
    diffusivity = D0
    block = 0
  []
  [./flux1]
    type = ComputeIsotropicHeat
    diffusivity = D12
    base_name = G1
  []
  [./flux2]
    type = ComputeIsotropicHeat
    diffusivity = D12
    base_name = G2
    block = '1 2'
  []
  [./flux3]
    type = ComputeIsotropicHeat
    diffusivity = D3
    base_name = G3
    block = 3
  [../]
  [./grad0]
    type = ComputeTempGrad
    temp = u
    block = 0
  []
  [./grad1]
    type = ComputeTempGrad
    temp = u
    base_name = G1
  []
  [./grad2]
    type = ComputeTempGrad
    temp = u
    base_name = G2
    block = '1 2'
  []
  [./grad3]
    type = ComputeTempGrad
    temp = u
    base_name = G3
    block = 3
  [../]
[]

[BCs]
  [./fix]
    type = DirichletBC
    variable = u
    boundary = left
    value = 0
  [../]
  [./Qx]
    type = NeumannBC
    variable = u
    boundary = right
    value = 1
  [../]
[]

[Executioner]
  type = Steady
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
