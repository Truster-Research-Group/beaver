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
  [./breakmesh]
    type = BreakMeshByBlockGenerator
    input = upprig
    split_interface = true
  []
[]

[AuxVariables]
  [./tjg]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./hag]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./hjg]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[GlobalParams]
  temp = 'u'
[]

[Beaver/VMNT/HeatDiff]
  add_variables = true
  generate_output = 'heat_flux_x heat_flux_y total_t_x total_t_y'
  [./diff0]
    block = 0
  [../]
  [./diff1]
    base_name = G1
    grad_base_name = G1
    block = 1
  [../]
  [./diff2]
    base_name = G2
    grad_base_name = G2
    block = 2
  [../]
  [./diff3]
    base_name = G3
    grad_base_name = G3
    block = 3
  [../]
[]

[Beaver/VMNT/HeatInter]
  generate_output = 'heatflux_aver_global temp_jump_global heatflux_jump_global'
  flux_type = 'simple'
  nis_flag = -1
  use_flux_penalty = true
  [./G0G1]
    boundary = Block0_Block1
    base_name_n = G1
  [../]
  [./G0G2]
    boundary = Block0_Block2
    base_name_n = G2
  [../]
  [./G1G3]
    boundary = Block1_Block3
    base_name = G1G3
    base_name_e = G1
    base_name_n = G3
  [../]
  [./G2G3]
    boundary = Block2_Block3
    base_name = G2G3
    base_name_e = G2
    base_name_n = G3
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
    base_name = G1
  []
  [./flux2]
    type = ComputeIsotropicHeat
    diffusivity = D2
    block = 2
    base_name = G2
  []
  [./flux3]
    type = ComputeIsotropicHeat
    diffusivity = D3
    block = 3
    base_name = G3
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
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu superlu_dist'
  solve_type = NEWTON
[]

[Postprocessors]
  [max]
    type = ElementExtremeValue
    variable = 'G2G3_heatflux_aver_global'
  []
[]

[Outputs]
  csv = true
[]
