[Mesh]
  [./lowlef]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 8
    xmax = 8
    ny = 8
    ymax = 8
    nz = 8
    zmax = 8
    elem_type = HEX8
  [../]
#  [./lowlef]
#    type = SubdomainBoundingBoxGenerator
#    input = 'gen'
#    block_id = 1
#    bottom_left = '0 0 0'
#    top_right = '4 4 8'
#  [../]
  [./lowrig]
    type = SubdomainBoundingBoxGenerator
    input = 'lowlef'
    block_id = 1
    bottom_left = '4 0 0'
    top_right = '8 4 8'
  [../]
  [./upplef]
    type = SubdomainBoundingBoxGenerator
    input = 'lowrig'
    block_id = 2
    bottom_left = '0 4 0'
    top_right = '4 8 8'
  [../]
  [./upprig]
    type = SubdomainBoundingBoxGenerator
    input = 'upplef'
    block_id = 3
    bottom_left = '4 4 0'
    top_right = '8 8 8'
  [../]
  [./breakmesh]
    type = BreakMeshByBlockGenerator
    input = upprig
    split_interface = true
  []
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  large_kinematics = false
[]

[Modules]
  [TensorMechanics]
    [Master]
      add_variables = true
      strain = SMALL
      volumetric_locking_correction = false
      generate_output = 'cauchy_stress_xx cauchy_stress_yy cauchy_stress_zz cauchy_stress_xy '
                          'cauchy_stress_xz cauchy_stress_yz mechanical_strain_xx mechanical_strain_yy mechanical_strain_zz mechanical_strain_xy '
                          'mechanical_strain_xz mechanical_strain_yz'
      [./G0]
        block = 0
      new_system = true
      formulation = TOTAL
      [../]
      [./G1]
        base_name = G1
        strain_base_name = G1
        block = 1
      new_system = true
      formulation = TOTAL
      [../]
      [./G2]
        block = 2
        base_name = G2
        strain_base_name = G2
      new_system = true
      formulation = TOTAL
      [../]
      [./G3]
        block = 3
        base_name = G3
        strain_base_name = G3
      new_system = true
      formulation = TOTAL
      [../]
    []
  []
[]

[Beaver/VMNS/TracInter]
  [./G0G1]
    boundary = 'Block0_Block1'
    base_name_n = G1
    trac_type = 'simple'
    nis_flag = -1
    use_trac_penalty = true
    generate_output = 'trac_aver_x trac_aver_y trac_aver_z jump_x jump_y jump_z normal_jump tangent_jump'
    #generate_output = 'trac_aver_x trac_aver_y trac_aver_z normal_trac_aver tangent_trac_aver jump_x jump_y jump_z normal_jump tangent_jump'
  [../]
  [./G0G2]
    boundary = 'Block0_Block2'
    base_name_n = G2
    trac_type = 'simple'
    nis_flag = -1
    use_trac_penalty = true
    generate_output = 'trac_aver_x trac_aver_y trac_aver_z jump_x jump_y jump_z normal_jump tangent_jump'
    #generate_output = 'trac_aver_x trac_aver_y trac_aver_z normal_trac_aver tangent_trac_aver jump_x jump_y jump_z normal_jump tangent_jump'
  [../]
  [./G1G3]
    boundary = 'Block1_Block3'
    base_name = G1G3
    base_name_e = G1
    base_name_n = G3
    trac_type = 'simple'
    nis_flag = -1
    use_trac_penalty = true
    generate_output = 'trac_aver_x trac_aver_y trac_aver_z jump_x jump_y jump_z normal_jump tangent_jump'
    #generate_output = 'trac_aver_x trac_aver_y trac_aver_z normal_trac_aver tangent_trac_aver jump_x jump_y jump_z normal_jump tangent_jump'
  [../]
  [./G2G3]
    boundary = 'Block2_Block3'
    base_name = G2G3
    base_name_e = G2
    base_name_n = G3
    trac_type = 'simple'
    nis_flag = -1
    use_trac_penalty = true
    generate_output = 'trac_aver_x trac_aver_y trac_aver_z jump_x jump_y jump_z normal_jump tangent_jump'
    #generate_output = 'trac_aver_x trac_aver_y trac_aver_z normal_trac_aver tangent_trac_aver jump_x jump_y jump_z normal_jump tangent_jump'
  [../]
[]

# Four materials and 4 kernels, linked by block
[Materials]
  [elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 1000.0
    poissons_ratio = 0.25
    block = '0 1 2 3'
  []
  [stress_G0]
    type = ComputeLagrangianLinearElasticStress
    block = 0
  []
  [stress_G1]
    type = ComputeLagrangianLinearElasticStress
    block = 1
    base_name = 'G1'
  []
  [stress_G2]
    type = ComputeLagrangianLinearElasticStress
    block = 2
    base_name = G2
  []
  [stress_G3]
    type = ComputeLagrangianLinearElasticStress
    block = 3
    base_name = G3
  []
[]

[BCs]
  [left]
    type = DirichletBC
    preset = true
    variable = disp_x
    boundary = left
    value = 0.0
  []

  [bottom]
    type = DirichletBC
    preset = true
    variable = disp_y
    boundary = bottom
    value = 0.0
  []

  [back]
    type = DirichletBC
    preset = true
    variable = disp_z
    boundary = back
    value = 0.0
  []

  [right]
    type = DirichletBC
    preset = true
    variable = disp_x
    boundary = right
    value = 0.1
  []

  [top]
    type = DirichletBC
    preset = true
    variable = disp_y
    boundary = top
    value = 0.05
  []
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
    variable = 'G2G3_trac_aver_x'
  []
[]

[Outputs]
  csv = true
  exodus = true
[]

