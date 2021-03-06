#include "Tests_pcp.h"

#include "ItemArray.hpp"
#include "utils.h"
#include "Model_T2D_ME_p.h"
#include "ModelData_T2D_ME_p.h"
#include "Step_T2D_ME_p.h"
#include "TimeHistory_T2D_ME_p_complete.h"
#include "TimeHistory_ConsoleProgressBar.h"
#include "QtApp_Prep_T2D_ME_p.h"

#include "test_simulations.h"

void test_t2d_me_p_test(int argc, char** argv)
{
	Model_T2D_ME_p model;
	model.load_mesh_from_hdf5("..\\..\\Asset\\square_mesh.h5");

	//for (size_t n_id = 0; n_id < model.get_node_num(); ++n_id)
	//{
	//	Model_T2D_ME_p::Node& n = model.get_nodes()[n_id];
	//	std::cout << "node " << n.id << "\n";
	//	for (size_t e_id = 0; e_id < n.n2e_num; ++e_id)
	//	{
	//		Model_T2D_ME_p::NodeToElem& n2e = n.n2es[e_id];
	//		std::cout << "  elem " << n2e.e_id << "\n";
	//	}
	//	std::cout << "\n";
	//}

	model.init_search_grid(0.5, 0.5);

	ParticleGenerator2D<Model_T2D_ME_p> pcl_generator;
	pcl_generator.generate_pcls_at_2nd_gauss(model);
	model.init_pcls(pcl_generator, 10.0);

	size_t pcl_num = model.get_pcl_num();
	Model_T2D_ME_p::Particle* pcls = model.get_pcls();
	MatModel::LinearElasticity* mms = model.add_LinearElasticity(pcl_num);
	for (size_t pcl_id = 0; pcl_id < pcl_num; ++pcl_id)
	{
		Model_T2D_ME_p::Particle& pcl = pcls[pcl_id];
		MatModel::LinearElasticity& mm = mms[pcl_id];
		mm.set_param(1000.0, 0.0);
		pcl.set_mat_model(mm);
	}

	//size_t bc_id;
	//size_t* bc_ids;
	//IndexArray bc_pt_array(50);

	//bc_id = 0;
	//bc_pt_array.add(bc_id);
	//bc_id = 1;
	//bc_pt_array.add(bc_id);
	////bc_id = 4;
	////bc_pt_array.add(bc_id);
	//bc_ids = bc_pt_array.get_mem();
	//model.init_axs(bc_pt_array.get_num());
	//size_t ax_num = model.get_ax_num();
	//AccelerationBC* axs = model.get_axs();
	//for (size_t a_id = 0; a_id < ax_num; ++a_id)
	//{
	//	AccelerationBC& abc = axs[a_id];
	//	abc.node_id = bc_ids[a_id];
	//	abc.a = 0.0;
	//}

	//bc_pt_array.reset();
	//bc_id = 0;
	//bc_pt_array.add(bc_id);
	//bc_id = 2;
	//bc_pt_array.add(bc_id);
	//bc_id = 3;
	//bc_pt_array.add(bc_id);
	//bc_ids = bc_pt_array.get_mem();
	//model.init_vxs(bc_pt_array.get_num());
	//size_t vx_num = model.get_vx_num();
	//VelocityBC* vxs = model.get_vxs();
	//for (size_t v_id = 0; v_id < vx_num; ++v_id)
	//{
	//	VelocityBC& vbc = vxs[v_id];
	//	vbc.node_id = bc_ids[v_id];
	//	vbc.v = 0.0;
	//}

	//bc_pt_array.reset();
	//bc_id = 0;
	//bc_pt_array.add(bc_id);
	//bc_id = 2;
	//bc_pt_array.add(bc_id);
	//bc_id = 3;
	//bc_pt_array.add(bc_id);
	//bc_ids = bc_pt_array.get_mem();
	//model.init_ays(bc_pt_array.get_num());
	//size_t ay_num = model.get_ay_num();
	//AccelerationBC* ays = model.get_ays();
	//for (size_t a_id = 0; a_id < ay_num; ++a_id)
	//{
	//	AccelerationBC& abc = ays[a_id];
	//	abc.node_id = bc_ids[a_id];
	//	abc.a = 0.0;
	//}

	//bc_pt_array.reset();
	//bc_id = 0;
	//bc_pt_array.add(bc_id);
	//bc_id = 3;
	//bc_pt_array.add(bc_id);
	//bc_ids = bc_pt_array.get_mem();
	//model.init_vys(bc_pt_array.get_num());
	//size_t vy_num = model.get_vy_num();
	//VelocityBC* vys = model.get_vys();
	//for (size_t v_id = 0; v_id < vy_num; ++v_id)
	//{
	//	VelocityBC& vbc = vys[v_id];
	//	vbc.node_id = bc_ids[v_id];
	//	vbc.v = 0.0;
	//}

	//try
	//{
	//	model.init_bcs();
	//}
	//catch (const std::exception& e)
	//{
	//	std::cout << e.what() << "\n";
	//	return;
	//}

	//for (size_t a_id = 0; a_id < model.get_ax_num(); ++a_id)
	//{
	//	AccelerationBC& abc = model.get_axs()[a_id];
	//	std::cout <<  "ax " << abc.node_id << "\n";
	//}
	//for (size_t a_id = 0; a_id < model.get_ay_num(); ++a_id)
	//{
	//	AccelerationBC& abc = model.get_ays()[a_id];
	//	std::cout << "ay " << abc.node_id << "\n";
	//}
	//return;

	// vx bc
	IndexArray vx_bc_pt_array(50);
	find_2d_nodes_on_x_line(model, vx_bc_pt_array, 0.0);
	find_2d_nodes_on_x_line(model, vx_bc_pt_array, 1.0, false);
	size_t* vx_bc_n_id = vx_bc_pt_array.get_mem();
	model.init_vxs(vx_bc_pt_array.get_num());
	size_t vx_num = model.get_vx_num();
	VelocityBC* vxs = model.get_vxs();
	for (size_t v_id = 0; v_id < vx_num; ++v_id)
	{
		VelocityBC& vbc = vxs[v_id];
		vbc.node_id = vx_bc_n_id[v_id];
		vbc.v = 0.0;
	}

	// vy bc
	IndexArray vy_bc_pt_array(50);
	find_2d_nodes_on_y_line(model, vy_bc_pt_array, 0.0);
	size_t* vy_bc_n_id = vy_bc_pt_array.get_mem();
	model.init_vys(vy_bc_pt_array.get_num());
	size_t vy_num = model.get_vy_num();
	VelocityBC* vys = model.get_vys();
	for (size_t v_id = 0; v_id < vy_num; ++v_id)
	{
		VelocityBC& vbc = vys[v_id];
		vbc.node_id = vy_bc_n_id[v_id];
		vbc.v = 0.0;
	}

	// traction bc
	IndexArray tbc_pt_array(50);
	find_2d_pcls(model, tbc_pt_array, Rect(0.0, 1.0, 0.8, 1.0));
	size_t* tbc_pcl_id = tbc_pt_array.get_mem();
	model.init_tys(tbc_pt_array.get_num());
	size_t ty_num = model.get_ty_num();
	TractionBCAtPcl* tys = model.get_tys();
	for (size_t t_id = 0; t_id < ty_num; ++t_id)
	{
		TractionBCAtPcl& tbc = tys[t_id];
		tbc.pcl_id = tbc_pcl_id[t_id];
		tbc.t = 0.5 * -1.0;
	}

	//QtApp_Prep_T2D_ME_p md_disp(argc, argv);
	//md_disp.set_win_size(900, 900);
	//md_disp.set_model(model);
	//md_disp.set_pts_from_node_id(vx_bc_pt_array.get_mem(), vx_bc_pt_array.get_num(), 0.01);
	////md_disp.set_pts_from_node_id(vy_bc_pt_array.get_mem(), vy_bc_pt_array.get_num(), 0.01);
	////md_disp.set_pts_from_pcl_id(tbc_pt_array.get_mem(), tbc_pt_array.get_num(), 0.1);
	//md_disp.start();
	//return;
	
	ResultFile_hdf5 res_file_hdf5;
	res_file_hdf5.create("t2d_me_p_test.h5");

	ModelData_T2D_ME_p md;
	md.output_model(model, res_file_hdf5);

	TimeHistory_T2D_ME_p_complete out1("compression");
	out1.set_interval_num(100);
	out1.set_res_file(res_file_hdf5);
	out1.set_output_init_state();

	TimeHistory_ConsoleProgressBar out_pb;

	Step_T2D_ME_p step("step1");
	step.set_model(model);
	step.set_step_time(2.0e-5);
	step.set_dtime(1.0e-5);
	step.set_thread_num(2);
	step.add_time_history(out1);
	step.add_time_history(out_pb);
	step.solve();
}

#include "QtApp_Posp_T2D_ME_p.h"
#include "test_model_view.h"

void test_t2d_me_p_test_result(int argc, char** argv)
{
	ResultFile_hdf5 rf;
	rf.open("t2d_me_p_test.h5");

	QtApp_Posp_T2D_ME_p app(argc, argv);
	app.set_win_size(900, 900);
	app.set_fld_range(0.0, 1.0);
	app.set_res_file(rf, "compression", 0, "y");
	//app.set_png_name("t2d_me_s_test_rigid_circle");
	app.start();
}
