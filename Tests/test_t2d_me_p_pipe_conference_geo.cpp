#include "Tests_pcp.h"

#include "ItemArray.hpp"
#include "utils.h"
#include "ModifiedCamClay.h"
#include "Model_T2D_ME_p.h"
#include "ModelData_T2D_ME_p.h"
#include "Step_T2D_ME_p_Geo.h"
#include "TimeHistory_T2D_ME_p_Geo_complete.h"
#include "TimeHistory_ConsoleProgressBar.h"
#include "QtApp_Prep_T2D_ME_p.h"

#include "test_simulations.h"

void test_t2d_me_p_pipe_conference_geo(int argc, char** argv)
{
	Model_T2D_ME_p model;
	model.load_mesh_from_hdf5("..\\..\\Asset\\rect_t_bar_conference_mesh.h5");
	model.init_search_grid(0.05, 0.05);

	ParticleGenerator2D<Model_T2D_ME_p> pcl_generator;
	pcl_generator.generate_pcls_in_grid_layout(Rect(-3.5, 3.5, -3.5, 0.0), 0.04, 0.04);
	pcl_generator.generate_pcls_in_grid_layout(Rect(-3.5, 3.5, -5.0, -3.5), 0.04, 0.04);
	pcl_generator.replace_with_pcls_in_grid_layout(Rect(-2.5, 2.5, -3.5, 0.0), 0.02, 0.02);
	pcl_generator.adjust_pcl_size_to_fit_elems(model);
	model.init_pcls(pcl_generator, 2650.0);

	size_t pcl_num = model.get_pcl_num();
	Model_T2D_ME_p::Particle* pcls = model.get_pcls();
	std::cout << "pcl_num: " << pcl_num << "\n";
	// mcc
	MatModel::ModifiedCamClay* mms = model.add_ModifiedCamClay(pcl_num);
	double K = 1.0 - sin(23.5 / 180.0 * 3.14159165359);
	double ini_stress[6] = { -12025.0, -20000.0, -12025.0, 0.0, 0.0, 0.0 };
	for (size_t p_id = 0; p_id < pcl_num; ++p_id)
	{
		Model_T2D_ME_p::Particle& pcl = pcls[p_id];
		pcl.s11 = ini_stress[0];
		pcl.s22 = ini_stress[1];
		pcl.s12 = 0.0;
		MatModel::ModifiedCamClay& mm = mms[p_id];
		mm.set_param_NC(0.3, 0.044, 0.205, 23.5, 3.6677, ini_stress);
		pcl.set_mat_model(mm);
	}

	model.init_rigid_circle(1.0e5, 0.0, 0.5 - 0.014, 0.5);
	//model.set_rigid_circle_velocity(0.0, -0.05, 0.0);
	
	// traction
	IndexArray mid_tbc_pt_array(50);
	find_2d_pcls(model, mid_tbc_pt_array, Rect(-2.5, 2.5, -0.011, 0.0));
	size_t* mid_tbc_pcl_ids = mid_tbc_pt_array.get_mem();
	size_t mid_tbc_num = mid_tbc_pt_array.get_num();
	
	IndexArray left_right_tbc_pt_array(50);
	find_2d_pcls(model, left_right_tbc_pt_array, Rect(-3.5, -2.5, -0.021, 0.0));
	find_2d_pcls(model, left_right_tbc_pt_array, Rect(2.5, 3.5, -0.021, 0.0), false);
	size_t *left_right_tbc_pcl_ids = left_right_tbc_pt_array.get_mem();
	size_t left_right_tbc_num = left_right_tbc_pt_array.get_num();
	
	model.init_tys(mid_tbc_num + left_right_tbc_num);
	TractionBCAtPcl* tys = model.get_tys();
	for (size_t t_id = 0; t_id < mid_tbc_num; ++t_id)
	{
		TractionBCAtPcl &tbc = tys[t_id];
		tbc.pcl_id = mid_tbc_pcl_ids[t_id];
		tbc.t = 0.02 * -20000.0;
	}
	for (size_t t_id = 0; t_id < left_right_tbc_num; ++t_id)
	{
		TractionBCAtPcl &tbc = tys[mid_tbc_num + t_id];
		tbc.pcl_id = left_right_tbc_pcl_ids[t_id];
		tbc.t = 0.04 * -20000.0;
	}

	IndexArray left_right_bc_pt_array(50);
	find_2d_nodes_on_x_line(model, left_right_bc_pt_array, -3.5);
	find_2d_nodes_on_x_line(model, left_right_bc_pt_array, 3.5, false);
	size_t* left_right_bc_n_id = left_right_bc_pt_array.get_mem();
	model.init_vxs(left_right_bc_pt_array.get_num());
	size_t vsx_num = model.get_vx_num();
	VelocityBC* vsxs = model.get_vxs();
	for (size_t v_id = 0; v_id < vsx_num; ++v_id)
	{
		VelocityBC &vbc = vsxs[v_id];
		vbc.node_id = left_right_bc_n_id[v_id];
		vbc.v = 0.0;
	}
	
	IndexArray bottom_bc_pt_array(50);
	find_2d_nodes_on_y_line(model, bottom_bc_pt_array, -5.0);
	size_t* bottom_bc_n_ids = bottom_bc_pt_array.get_mem();
	model.init_vys(bottom_bc_pt_array.get_num());
	size_t vsy_num = model.get_vy_num();
	VelocityBC* vsys = model.get_vys();
	for (size_t v_id = 0; v_id < vsy_num; ++v_id)
	{
		VelocityBC &vbc = vsys[v_id];
		vbc.node_id = bottom_bc_n_ids[v_id];
		vbc.v = 0.0;
	}

	//QtApp_Prep_T2D_ME_p md_disp(argc, argv);
	//md_disp.set_win_size(900, 900);
	//md_disp.set_model(model);
	////md_disp.set_pts_from_node_id(left_right_bc_pt_array.get_mem(), left_right_bc_pt_array.get_num(), 0.05);
	////md_disp.set_pts_from_node_id(bottom_bc_pt_array.get_mem(), bottom_bc_pt_array.get_num(), 0.05);
	////md_disp.set_pts_from_pcl_id(mid_tbc_pt_array.get_mem(), mid_tbc_pt_array.get_num(), 0.01);
	////md_disp.set_pts_from_pcl_id(left_right_tbc_pt_array.get_mem(), left_right_tbc_pt_array.get_num(), 0.015);
	//// all
	////md_disp.set_display_range(-3.6, 3.6, -5.1, 1.1);
	//// left
	////md_disp.set_display_range(-3.8, -2.2, -1.0, 1.0);
	//// middle
	//md_disp.set_display_range(-1.5, 1.5, -0.75, 0.25);
	//// right
	////md_disp.set_display_range(2.2, 3.8, -1.0, 1.0);
	//md_disp.start();
	//return;

	ResultFile_hdf5 res_file_hdf5;
	res_file_hdf5.create("t2d_me_p_pipe_conference_geo.h5");

	ModelData_T2D_ME_p md;
	md.output_model(model, res_file_hdf5);

	TimeHistory_T2D_ME_p_Geo_complete out("geostatic");
	out.set_res_file(res_file_hdf5);
	out.set_output_init_state();
	TimeHistory_ConsoleProgressBar out_pb;
	
	Step_T2D_ME_p_Geo step_gs("geo_step");
	step_gs.set_model(model);
	step_gs.set_step_time(1.0);
	step_gs.set_dtime(1.0e-5);
	step_gs.set_thread_num(6);
	// out
	out.set_interval_num(100);
	step_gs.add_time_history(out);
	// out_pb
	step_gs.add_time_history(out_pb);
	step_gs.solve();
}

#include "QtApp_Posp_T2D_ME_p.h"
#include "test_model_view.h"

void test_t2d_me_p_pipe_conference_geo_result(int argc, char** argv)
{
	ResultFile_hdf5 rf;
	rf.open("t2d_me_p_pipe_conference_geo.h5");

	QtApp_Posp_T2D_ME_p app(argc, argv, QtApp_Posp_T2D_ME_p::Animation);
	app.set_win_size(900, 900);
	app.set_res_file(rf, "geostatic", "s22");
	app.set_ani_time(5.0);
	app.set_display_range(-3.6, 3.6, -5.1, 0.6);
	app.set_fld_range(-20100.0, -19900.0);
	app.set_color_map_pos(0.6, 0.45, 0.5);
	//app.set_png_name("t2d_me_p_pipe_conference_geo");
	//app.set_gif_name("t2d_me_p_pipe_conference_geo");
	app.start();
}