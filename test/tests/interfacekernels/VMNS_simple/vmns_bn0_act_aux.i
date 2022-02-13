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

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  large_kinematics = false
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
  [disp_z]
  []
[]

[AuxVariables]
  [./tag]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Modules]
  [TensorMechanics]
    [Master]
      [all]
        strain = SMALL
        new_system = true
        formulation = TOTAL
        volumetric_locking_correction = false
        generate_output = 'cauchy_stress_xx cauchy_stress_yy cauchy_stress_zz cauchy_stress_xy '
                          'cauchy_stress_xz cauchy_stress_yz mechanical_strain_xx mechanical_strain_yy mechanical_strain_zz mechanical_strain_xy '
                          'mechanical_strain_xz mechanical_strain_yz'
      []
    []
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

[AuxKernels]
  [./tag]
    type = MaterialRealAux
    property = traction_aver_global_x
    variable = tag
    boundary = interface
    check_boundary_restricted = false
  [../]
[]

[Materials]
  [elastic_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 1000.0
    poissons_ratio = 0.25
  []
  [compute_stress]
    type = ComputeLagrangianLinearElasticStress
  []
  [./dispjump]
    type = CZMComputeDisplacementJumpSmallStrain
    boundary = interface
  [../]
  [./tracaver]
    type = ComputeTractionAverJumpSimple
    boundary = interface
  [../]
  [./tag]
    type = VMNSRealVectorCartesianComponent
    real_vector_value = traction_aver_global 
    property_name = traction_aver_global_x
    index = 0
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

[Outputs]
  exodus = true
[]
