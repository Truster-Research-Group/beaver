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
  []
[]

# Large strain model; yet, the DG terms for Total Lagrangian
# don't care about the kinematics; they get the right stress
# from the Materials and then use it in the same format.
[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  large_kinematics = true
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
  [disp_z]
  []
[]

[Kernels]
  [sdx]
    type = TotalLagrangianStressDivergence
    variable = disp_x
    component = 0
  []
  [sdy]
    type = TotalLagrangianStressDivergence
    variable = disp_y
    component = 1
  []
  [sdz]
    type = TotalLagrangianStressDivergence
    variable = disp_z
    component = 2
  []
[]

[InterfaceKernels]
  [./vmns_x]
    type = VMNSInterfaceKernelSimple
    variable = disp_x
    neighbor_var = disp_x
    component = 0
    boundary = interface
    #pen_scale = 0.01
  [../]
  [./vmns_y]
    type = VMNSInterfaceKernelSimple
    variable = disp_y
    neighbor_var = disp_y
    component = 1
    boundary = interface
    #pen_scale = 0.01
  [../]
  [./vmns_z]
    type = VMNSInterfaceKernelSimple
    variable = disp_z
    neighbor_var = disp_z
    component = 2
    boundary = interface
    #pen_scale = 0.01
  [../]
[]

[AuxVariables]
  [strain_xx]
    order = CONSTANT
    family = MONOMIAL
  []
  [strain_yy]
    order = CONSTANT
    family = MONOMIAL
  []
  [strain_zz]
    order = CONSTANT
    family = MONOMIAL
  []
  [strain_xy]
    order = CONSTANT
    family = MONOMIAL
  []
  [strain_xz]
    order = CONSTANT
    family = MONOMIAL
  []
  [strain_yz]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_xx]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_yy]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_zz]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_xy]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_yz]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_xz]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [stress_xx]
    type = RankTwoAux
    rank_two_tensor = pk1_stress
    variable = stress_xx
    index_i = 0
    index_j = 0
    execute_on = timestep_end
  []
  [stress_yy]
    type = RankTwoAux
    rank_two_tensor = pk1_stress
    variable = stress_yy
    index_i = 1
    index_j = 1
    execute_on = timestep_end
  []
  [stress_zz]
    type = RankTwoAux
    rank_two_tensor = pk1_stress
    variable = stress_zz
    index_i = 2
    index_j = 2
    execute_on = timestep_end
  []
  [stress_xy]
    type = RankTwoAux
    rank_two_tensor = pk1_stress
    variable = stress_xy
    index_i = 0
    index_j = 1
    execute_on = timestep_end
  []
  [stress_xz]
    type = RankTwoAux
    rank_two_tensor = pk1_stress
    variable = stress_xz
    index_i = 0
    index_j = 2
    execute_on = timestep_end
  []
  [stress_yz]
    type = RankTwoAux
    rank_two_tensor = pk1_stress
    variable = stress_yz
    index_i = 1
    index_j = 2
    execute_on = timestep_end
  []

  [strain_xx]
    type = RankTwoAux
    rank_two_tensor = mechanical_strain
    variable = strain_xx
    index_i = 0
    index_j = 0
    execute_on = timestep_end
  []
  [strain_yy]
    type = RankTwoAux
    rank_two_tensor = mechanical_strain
    variable = strain_yy
    index_i = 1
    index_j = 1
    execute_on = timestep_end
  []
  [strain_zz]
    type = RankTwoAux
    rank_two_tensor = mechanical_strain
    variable = strain_zz
    index_i = 2
    index_j = 2
    execute_on = timestep_end
  []
  [strain_xy]
    type = RankTwoAux
    rank_two_tensor = mechanical_strain
    variable = strain_xy
    index_i = 0
    index_j = 1
    execute_on = timestep_end
  []
  [strain_xz]
    type = RankTwoAux
    rank_two_tensor = mechanical_strain
    variable = strain_xz
    index_i = 0
    index_j = 2
    execute_on = timestep_end
  []
  [strain_yz]
    type = RankTwoAux
    rank_two_tensor = mechanical_strain
    variable = strain_yz
    index_i = 1
    index_j = 2
    execute_on = timestep_end
  []
[]

# Four materials and 4 kernels, linked by block
[Materials]
  [elastic_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 1000.0
    poissons_ratio = 0.25
  []
  [compute_stress]
    type = ComputeStVenantKirchhoffStress
  []
  [compute_strain]
    type = ComputeLagrangianStrain
  []
  [./dispjump]
    type = CZMComputeDisplacementJumpSmallStrain
    boundary = interface
  [../]
  [./tracaver]
    type = ComputeTractionAverJumpSimple
    boundary = interface
  [../]
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
    variable = disp_x
  []
[]

[Outputs]
  csv = true
[]

