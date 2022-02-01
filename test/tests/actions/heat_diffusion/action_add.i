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

[GlobalParams]
  temp = 'u'
[]

[Beaver/VMNT/HeatDiff]
  [./all]
    temp = 'u'
    add_variables = true
  [../]
[]

[Materials]
  [./mat0]
    type = GenericConstantMaterial
    prop_names = 'D0'
    prop_values = '1'
    block = 0
  []
  [./mat1]
    type = GenericConstantMaterial
    prop_names = 'D1'
    prop_values = '20'
    block = 1
  []
  [./mat2]
    type = GenericConstantMaterial
    prop_names = 'D2'
    prop_values = '20'
    block = 2
  []
  [./mat3]
    type = GenericConstantMaterial
    prop_names = 'D3'
    prop_values = '1'
    block = 3
  [../]
  [./flux0]
    type = ComputeIsotropicHeat
    diffusivity = D0
    block = 0
  []
  [./flux1]
    type = ComputeIsotropicHeat
    diffusivity = D1
    block = 1
  []
  [./flux2]
    type = ComputeIsotropicHeat
    diffusivity = D2
    block = 2
  []
  [./flux3]
    type = ComputeIsotropicHeat
    diffusivity = D3
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

[Postprocessors]
  [max]
    type = ElementExtremeValue
    variable = u
  []
[]

[Outputs]
  csv = true
[]
