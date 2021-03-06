#include "Tests_pcp.h"

#include "ItemArray.hpp"
#include "ParticleGenerator3D.hpp"
#include "LinearElasticity.h"
#include "Model_T3D_ME_s.h"
#include "Step_T3D_ME_s.h"
#include "ModelData_T3D_ME_s.h"
#include "TimeHistory_ConsoleProgressBar.h"
#include "TimeHistory_T3D_ME_s_complete.h"
#include "QtApp_Prep_T3D_ME_s.h"

#include "utils.h"
#include "test_simulations.h"

void test_t3d_me_s_1d_compression(int argc, char **argv)
{
	Model_T3D_ME_s model;
	model.load_mesh_from_hdf5("../../Asset/brick_mesh_1.00_2x2x10.h5");
	std::cout << "node num: " << model.get_node_num() << "\n"
			  << "elem num: " << model.get_elem_num() << "\n";

	model.init_search_grid(0.05, 0.05, 0.05);

	ParticleGenerator3D<Model_T3D_ME_s> pg;
	//Cube bar_box = { 0.0, 0.1, 0.0, 0.1, 0.0, 0.5 };
	//pg.generate_pcls_grid(bar_box, 0.02, 0.02, 0.02);
	pg.generate_pcls_second_order_gauss(model);
	model.init_pcls(pg, 10.0);
	std::cout << "pcl num: " << model.get_pcl_num() << "\n";

	size_t pcl_num = model.get_pcl_num();
	Model_T3D_ME_s::Particle *pcls = model.get_pcls();
	MatModel::LinearElasticity *mms = model.add_LinearElasticity(pcl_num);
	for (size_t pcl_id = 0; pcl_id < pcl_num; ++pcl_id)
	{
		Model_T3D_ME_s::Particle &pcl = pcls[pcl_id];
		MatModel::LinearElasticity &mm = mms[pcl_id];
		mm.set_param(100.0, 0.0);
		pcl.set_mat_model(mm);
	}

	IndexArray tbc_pcl_array(100);
	find_3d_pcls(model, tbc_pcl_array, Cube(0.0, 0.2, 0.0, 0.2, 1.0 - 0.02, 1.0));
	size_t* tbc_pcl_id = tbc_pcl_array.get_mem();
	model.init_tzs(tbc_pcl_array.get_num());
	for (size_t t_id = 0; t_id < model.tz_num; ++t_id)
	{
		TractionBCAtPcl& tbc = model.tzs[t_id];
		tbc.pcl_id = tbc_pcl_id[t_id];
		tbc.t = 1.666667e-3 * -0.01;
	}

	IndexArray vx_bc_pt_array(100);
	find_3d_nodes_on_x_plane(model, vx_bc_pt_array, 0.0);
	find_3d_nodes_on_x_plane(model, vx_bc_pt_array, 0.2, false);
	size_t *vx_bc_n_id = vx_bc_pt_array.get_mem();
	model.init_vxs(vx_bc_pt_array.get_num());
	for (size_t v_id = 0; v_id < model.vx_num; ++v_id)
	{
		VelocityBC &vbc = model.vxs[v_id];
		vbc.node_id = vx_bc_n_id[v_id];
		vbc.v = 0.0;
	}

	IndexArray vy_bc_pt_array(100);
	find_3d_nodes_on_y_plane(model, vy_bc_pt_array, 0.0);
	find_3d_nodes_on_y_plane(model, vy_bc_pt_array, 0.2, false);
	size_t *vy_bc_n_id = vy_bc_pt_array.get_mem();
	model.init_vys(vy_bc_pt_array.get_num());
	for (size_t v_id = 0; v_id < model.vy_num; ++v_id)
	{
		VelocityBC &vbc = model.vys[v_id];
		vbc.node_id = vy_bc_n_id[v_id];
		vbc.v = 0.0;
	}
	
	IndexArray vz_bc_pt_array(100);
	find_3d_nodes_on_z_plane(model, vz_bc_pt_array, 0.0);
	size_t *vz_bc_n_id = vz_bc_pt_array.get_mem();
	model.init_vzs(vz_bc_pt_array.get_num());
	for (size_t v_id = 0; v_id < model.vz_num; ++v_id)
	{
		VelocityBC &vbc = model.vzs[v_id];
		vbc.node_id = vz_bc_n_id[v_id];
		vbc.v = 0.0;
	}

	QtApp_Prep_T3D_ME_s md_disp(argc, argv);
	md_disp.set_win_size(900, 900);
	md_disp.set_view_dir(30.0, 30.0);
	md_disp.set_light_dir(90.0, 30.0);
	md_disp.set_model(model);
	//md_disp.set_pts_from_node_id(vx_bc_pt_array.get_mem(), vx_bc_pt_array.get_num(), 0.01);
	//md_disp.set_pts_from_node_id(vy_bc_pt_array.get_mem(), vy_bc_pt_array.get_num(), 0.01);
	//md_disp.set_pts_from_node_id(vz_bc_pt_array.get_mem(), vz_bc_pt_array.get_num(), 0.01);
	md_disp.set_pts_from_pcl_id(tbc_pcl_array.get_mem(), tbc_pcl_array.get_num(), 0.012);
	md_disp.start();
	return;

	ResultFile_hdf5 res_file_hdf5;
	res_file_hdf5.create("t3d_me_s_1d_compression.h5");

	ModelData_T3D_ME_s md;
	md.output_model(model, res_file_hdf5);

	TimeHistory_T3D_ME_s_complete out1("compression");
	out1.set_res_file(res_file_hdf5);
	out1.set_output_init_state();
	out1.set_interval_num(50);
	TimeHistory_ConsoleProgressBar out_cpb;

	Step_T3D_ME_s step("step1");
	step.set_model(model);
	step.set_step_time(10.0);
	step.set_dtime(1.0e-5);
	step.add_time_history(out1);
	step.add_time_history(out_cpb);
	step.solve();
}

#include "QtApp_Posp_T3D_ME_s.h"
#include "test_model_view.h"

void test_t3d_me_s_1d_compression_result(int argc, char **argv)
{
	ResultFile_hdf5 rf;
	rf.open("t3d_me_s_1d_compression.h5");
	
	//QtApp_Posp_T3D_ME_s app(argc, argv, QtApp_Posp_T3D_ME_s::SingleFrame);
	//app.set_res_file(rf, "compression", 2, Hdf5Field::z);
	QtApp_Posp_T3D_ME_s app(argc, argv, QtApp_Posp_T3D_ME_s::Animation);
	app.set_res_file(rf, "compression", Hdf5Field::s33);
	app.set_ani_time(5.0);
	app.set_win_size(900, 900);
	app.set_view_dir(30.0f, 30.0f);
	app.set_light_dir(90.0f, 30.0f);
	app.set_color_map_fld_range(-0.01, 0.0);
	app.set_color_map_geometry(0.7f, 0.45f, 0.5f);
	//app.set_png_name("t3d_me_s_1d_compression");
	//app.set_gif_name("t3d_me_s_1d_compression");
	app.start();
}
