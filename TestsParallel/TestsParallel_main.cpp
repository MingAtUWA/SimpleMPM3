#include "TestsParallel_pcp.h"

#include "MaterialModelInitializer.h"
#include "test_simulations_omp.h"
#include "test_model_view_omp.h"

int main(int argc, char *argv[])
{
	//test_von_mises(argc, argv);

	//test_Step_OMP(argc, argv);

	//test_t2d_me_mt_test1(argc, argv);
	
	//test_t2d_me_s_test2(argc, argv);
	//test_t2d_me_s_test2_result(argc, argv);

	//test_t2d_me_mt_1d_compression(argc, argv);
	//test_t2d_me_mt_1d_compression_result(argc, argv);

	//test_t2d_me_mt_cap_compression(argc, argv);
	//test_t2d_me_mt_cap_compression_result(argc, argv);

	//test_t2d_me_mt_strip_footing_smaller(argc, argv);
	//test_t2d_me_mt_strip_footing_smaller_result(argc, argv);

	//test_t2d_me_mt_strip_footing(argc, argv);
	//test_t2d_me_mt_strip_footing_result(argc, argv);

	//test_t3d_me_mt_1d_compression(argc, argv);
	//test_t3d_me_mt_1d_compression_result(argc, argv);

	//test_t3d_me_s_1d_compression(argc, argv);
	//test_t3d_me_s_1d_compression_result(argc, argv);

	//test_omp_barrier_time();
	//test_rigid_cylinder(argc, argv);
	//test_rigid_cone(argc, argv);
	//test_rigid_mesh_contact(argc, argv);
	//test_rigid_mesh_contact2(argc, argv);
	//test_contact_model_3d(argc, argv);
	
	//test_t3d_me_mt_cap_compression(argc, argv);
	//test_t3d_me_mt_cap_compression_restart(argc, argv);
	//test_t3d_me_mt_cap_compression_restart2(argc, argv);
	//test_t3d_me_mt_cap_compression_result(argc, argv);
	//test_t3d_me_mt_cap_compression_result_div(argc, argv);

	//test_t3d_me_mt_block_sliding(argc, argv);
	//test_t3d_me_mt_block_sliding_result(argc, argv);

	//test_t3d_rigid_mesh(argc, argv);
	//test_t3d_me_mt_test_rigid_mesh(argc, argv);
	//test_t3d_me_mt_test_rigid_mesh_result(argc, argv);

	//test_t3d_me_mt_cylinder_foundation_create_model(argc, argv);
	//test_t3d_me_mt_cylinder_foundation(argc, argv);
	//test_t3d_me_mt_cylinder_foundation_restart(argc, argv);
	//test_t3d_me_mt_cylinder_foundation_restart2(argc, argv);
	//test_t3d_me_mt_cylinder_foundation_result(argc, argv);

	//test_t3d_me_mt_cylinder_foundation_create_model_den(argc, argv);
	//test_t3d_me_mt_cylinder_foundation_den(argc, argv);
	//test_t3d_me_mt_cylinder_foundation_restart_den(argc, argv);
	//test_t3d_me_mt_cylinder_foundation_restart_den2(argc, argv);
	//test_t3d_me_mt_cylinder_foundation_result_den(argc, argv);

	//test_t3d_me_mt_cylinder_foundation_create_model_ch_den(argc, argv);
	//test_t3d_me_mt_cylinder_foundation_ch_den(argc, argv);
	//test_t3d_me_mt_cylinder_foundation_result_ch_den2(argc, argv);

	//test_t2d_chm_mt_1d_consolidation(argc, argv);
	//test_t2d_chm_mt_1d_consolidation_restart(argc, argv);
	//test_t2d_chm_mt_1d_consolidation_static_result(argc, argv);
	//test_t2d_chm_mt_1d_consolidation_ani_result(argc, argv);

	//test_t2d_chm_mt_test_rigid_circle(argc, argv);
	//test_t2d_chm_mt_test_rigid_circle_result(argc, argv);
	
	//test_t2d_chm_mt_pipe_conference(argc, argv);
	//test_t2d_chm_mt_pipe_conference_result(argc, argv);

	//test_t2d_chm_mt_pipe_conference_den(argc, argv);
	//test_t2d_chm_mt_pipe_conference_den_restart(argc, argv);
	//test_t2d_chm_mt_pipe_conference_den_result(argc, argv);

	//test_t3d_me_mt_test_spudcan_model(argc, argv);
	//test_t3d_me_mt_spudcan_coarse_model(argc, argv);
	//test_t3d_me_mt_spudcan_coarse(argc, argv);
	//test_t3d_me_mt_spudcan_coarse_result(argc, argv);

	MatModel::MaterialModelInitializer::init();
	//test_sand_hypoplasticity_integration();
	//test_sand_hypoplasticity_Herleand_Gudehus_1999();
	//test_sand_hypoplasticity_Wichtmann_2019();

	test_t3d_me_mt_triaxial_compression(argc, argv);
	//test_t3d_me_mt_triaxial_compression_result(argc, argv);

	//test_t3d_chm_mt_1d_consolidation(argc, argv);
	//test_t3d_chm_mt_1d_consolidation_result(argc, argv);

	//test_sort_pcl_task();
	//test_sort_pcl_task2();
	//test_sort_tri_mesh_node_task();
	//test_sort_tri_mesh_node_task2();

	//test_t2d_me_tbb_1d_compression(argc, argv);
	//test_t2d_me_tbb_1d_compression_result(argc, argv);

	//test_t2d_me_tbb_cap_compression(argc, argv);
	//test_t2d_me_tbb_cap_compression_result(argc, argv);

	system("pause");
	return 0;
}
