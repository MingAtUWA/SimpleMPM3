#include "SimulationsOMP_pcp.h"

#include <exception>
#include "MatModelIdToPointerMap.h"
#include "Model_hdf5_utilities.h"
#include "Model_T3D_ME_mt_hdf5_utilities.h"

namespace Model_T3D_ME_mt_hdf5_utilities
{
using namespace Model_hdf5_utilities;

int output_background_mesh_to_hdf5_file(
	Model_T3D_ME_mt& md,
	ResultFile_hdf5& rf,
	hid_t grp_id
	)
{
	if (grp_id < 0)
		return -1;

	hid_t bg_mesh_grp_id = rf.create_group(grp_id, "BackgroundMesh");

	size_t node_num = md.get_node_num();
	size_t elem_num = md.get_elem_num();

	// bg mesh properties
	const char* bg_mesh_type = "T2D_mt";
	rf.write_attribute(bg_mesh_grp_id, "type", strlen(bg_mesh_type), bg_mesh_type);
	rf.write_attribute(bg_mesh_grp_id, "node_num", node_num);
	rf.write_attribute(bg_mesh_grp_id, "element_num", elem_num);

	// bg grid properties
	rf.write_attribute(bg_mesh_grp_id, "bg_grid_hx", md.get_bg_grid_hx());
	rf.write_attribute(bg_mesh_grp_id, "bg_grid_hy", md.get_bg_grid_hy());
	rf.write_attribute(bg_mesh_grp_id, "bg_grid_hz", md.get_bg_grid_hz());

	// node coordinates
	NodeData *nodes_data = new NodeData[node_num];
	Model_T3D_ME_mt::NodePos *node_pos = md.node_pos;
	for (size_t n_id = 0; n_id < node_num; ++n_id)
	{
		NodeData &node_data = nodes_data[n_id];
		node_data.id = n_id;
		Model_T3D_ME_mt::NodePos &np = node_pos[n_id];
		node_data.x = np.x;
		node_data.y = np.y;
		node_data.z = np.z;
	}
	hid_t nd_dt_id = get_node_dt_id();
	rf.write_dataset(
		bg_mesh_grp_id,
		"NodeData",
		node_num,
		nodes_data,
		nd_dt_id
		);
	H5Tclose(nd_dt_id);
	delete[] nodes_data;

	// element indices
	ElementData *elems_data = new ElementData[elem_num];
	Model_T3D_ME_mt::ElemNodeIndex *e_node_index = md.elem_node_id;
	double* e_vol = md.elem_vol;
	Model_T3D_ME_mt::ElemShapeFuncABC* e_sf_abc = md.elem_sf_abc;
	Model_T3D_ME_mt::ElemShapeFuncD* e_sf_d = md.elem_sf_d;
	for (size_t e_id = 0; e_id < elem_num; ++e_id)
	{
		ElementData &elem_data = elems_data[e_id];
		elem_data.id = e_id;
		Model_T3D_ME_mt::ElemNodeIndex &eni = e_node_index[e_id];
		elem_data.n1 = eni.n1;
		elem_data.n2 = eni.n2;
		elem_data.n3 = eni.n3;
		elem_data.n4 = eni.n4;
		elem_data.vol = e_vol[e_id];
		Model_T3D_ME_mt::ElemShapeFuncABC& esfabc = e_sf_abc[e_id];
		elem_data.a1 = esfabc.a1;
		elem_data.b1 = esfabc.b1;
		elem_data.c1 = esfabc.c1;
		elem_data.a2 = esfabc.a2;
		elem_data.b2 = esfabc.b2;
		elem_data.c2 = esfabc.b2;
		elem_data.a3 = esfabc.a3;
		elem_data.b3 = esfabc.b3;
		elem_data.c3 = esfabc.c3;
		elem_data.a4 = esfabc.a4;
		elem_data.b4 = esfabc.b4;
		elem_data.c4 = esfabc.c4;
		Model_T3D_ME_mt::ElemShapeFuncD& esfd = e_sf_d[e_id];
		elem_data.d1 = esfd.d1;
		elem_data.d2 = esfd.d2;
		elem_data.d3 = esfd.d3;
		elem_data.d4 = esfd.d4;
	}
	hid_t ed_dt_id = get_element_dt_id();
	rf.write_dataset(
		bg_mesh_grp_id,
		"ElementData",
		elem_num,
		elems_data,
		ed_dt_id
		);
	H5Tclose(ed_dt_id);
	delete[] elems_data;

	// output bg mesh...

	rf.close_group(bg_mesh_grp_id);
	return 0;
}

int load_background_mesh_from_hdf5_file(
	Model_T3D_ME_mt &md,
	ResultFile_hdf5 &rf,
	hid_t grp_id
	)
{
	if (grp_id < 0)
		return -1;

	hid_t bg_mesh_grp_id = rf.open_group(grp_id, "BackgroundMesh");

	size_t elem_num, node_num;
	rf.read_attribute(bg_mesh_grp_id, "node_num", node_num);
	rf.read_attribute(bg_mesh_grp_id, "element_num", elem_num);
	md.alloc_mesh(node_num, elem_num);

	// nodes
	NodeData *nodes_data = new NodeData[node_num];
	hid_t nd_dt_id = get_node_dt_id();
	rf.read_dataset(
		bg_mesh_grp_id,
		"NodeData",
		node_num,
		nodes_data,
		nd_dt_id
		);
	H5Tclose(nd_dt_id);
	Model_T3D_ME_mt::NodePos *node_pos = md.node_pos;
	for (size_t n_id = 0; n_id < node_num; ++n_id)
	{
		Model_T3D_ME_mt::NodePos& np = node_pos[n_id];
		NodeData &node_data = nodes_data[n_id];
		np.x = node_data.x;
		np.y = node_data.y;
		np.z = node_data.z;
	}
	delete[] nodes_data;

	// elements
	ElementData *elems_data = new ElementData[elem_num];
	hid_t ed_dt_id = get_element_dt_id();
	rf.read_dataset(
		bg_mesh_grp_id,
		"ElementData",
		elem_num,
		elems_data,
		ed_dt_id
		);
	H5Tclose(ed_dt_id);
	Model_T3D_ME_mt::ElemNodeIndex* e_node_id = md.elem_node_id;
	Model_T3D_ME_mt::ElemShapeFuncABC* e_sf_abc = md.elem_sf_abc;
	Model_T3D_ME_mt::ElemShapeFuncD* e_sf_d = md.elem_sf_d;
	for (size_t e_id = 0; e_id < elem_num; ++e_id)
	{
		ElementData &elem_data = elems_data[e_id];
		Model_T3D_ME_mt::ElemNodeIndex& eni = e_node_id[e_id];
		eni.n1 = elem_data.n1;
		eni.n2 = elem_data.n2;
		eni.n3 = elem_data.n3;
		eni.n4 = elem_data.n4;
		Model_T3D_ME_mt::ElemShapeFuncABC &esfabc = e_sf_abc[e_id];
		esfabc.a1 = elem_data.a1;
		esfabc.a2 = elem_data.a2;
		esfabc.a3 = elem_data.a3;
		esfabc.a4 = elem_data.a4;
		esfabc.b1 = elem_data.b1;
		esfabc.b2 = elem_data.b2;
		esfabc.b3 = elem_data.b3;
		esfabc.b4 = elem_data.b4;
		esfabc.c1 = elem_data.c1;
		esfabc.c2 = elem_data.c2;
		esfabc.c3 = elem_data.c3;
		esfabc.c4 = elem_data.c4;
		Model_T3D_ME_mt::ElemShapeFuncD& esfd = e_sf_d[e_id];
		esfd.d1 = elem_data.d1;
		esfd.d2 = elem_data.d2;
		esfd.d3 = elem_data.d3;
		esfd.d4 = elem_data.d4;
	}
	delete[] elems_data;

	// init bg_grid
	double bg_grid_hx, bg_grid_hy, bg_grid_hz;
	rf.read_attribute(bg_mesh_grp_id, "bg_grid_hx", bg_grid_hx);
	rf.read_attribute(bg_mesh_grp_id, "bg_grid_hy", bg_grid_hy);
	rf.read_attribute(bg_mesh_grp_id, "bg_grid_hz", bg_grid_hz);

	// read bg mesh...

	rf.close_group(bg_mesh_grp_id);
	return 0;
}

int output_boundary_condition_to_hdf5_file(
	Model_T3D_ME_mt& md,
	ResultFile_hdf5& rf,
	hid_t grp_id
	)
{
	if (grp_id < 0 || md.pcl_num == 0)
		return -1;

	hid_t bc_grp_id = rf.create_group(grp_id, "BoundaryCondition");
	
	rf.write_attribute(bc_grp_id, "node_num", md.node_num);

	hid_t n_vbc_dt_id = get_node_vbc_dt_id();
	NodeVBCData* nvbc_data = new NodeVBCData[md.node_num];
	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		NodeVBCData& nvbc = nvbc_data[n_id];
		Model_T3D_ME_mt::NodeHasVBC& md_nvbc = md.node_has_vbc[n_id];
		nvbc.has_vx_bc = md_nvbc.has_vx_bc;
		nvbc.has_vy_bc = md_nvbc.has_vy_bc;
		nvbc.has_vz_bc = md_nvbc.has_vz_bc;
	}
	rf.write_dataset(bc_grp_id, "NodeVelocityBC", md.node_num, nvbc_data, n_vbc_dt_id);
	delete[] nvbc_data;
	H5Tclose(n_vbc_dt_id);
	
	rf.close_group(bc_grp_id);
	return 0;
}

int load_boundary_condition_from_hdf5_file(
	Model_T3D_ME_mt& md,
	ResultFile_hdf5& rf,
	hid_t grp_id
	)
{
	if (grp_id < 0)
		return -1;
	
	hid_t md_id = rf.get_model_data_grp_id();

	hid_t bc_id = rf.open_group(md_id, "BoundaryCondition");
	
	rf.read_attribute(bc_id, "node_num", md.node_num);
	
	hid_t n_vbc_dt_id = get_node_vbc_dt_id();
	NodeVBCData *nvbc_data = new NodeVBCData[md.node_num];
	rf.read_dataset(bc_id, "NodeVelocityBC", md.node_num, nvbc_data, n_vbc_dt_id);
	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		NodeVBCData& nvbc = nvbc_data[n_id];
		Model_T3D_ME_mt::NodeHasVBC &md_nvbc = md.node_has_vbc[n_id];
		md_nvbc.has_vx_bc = nvbc.has_vx_bc;
		md_nvbc.has_vy_bc = nvbc.has_vy_bc;
		md_nvbc.has_vz_bc = nvbc.has_vz_bc;
	}
	delete[] nvbc_data;
	H5Tclose(n_vbc_dt_id);

	rf.close_group(bc_id);
	return 0;
}

int output_ori_pcl_data_to_hdf5_file(
	Model_T3D_ME_mt& md,
	ResultFile_hdf5& rf,
	hid_t grp_id
	)
{
	if (grp_id < 0)
		return -1;

	hid_t pcl_data_grp_id = rf.create_group(grp_id, "ParticleData");

	size_t ori_pcl_num = md.get_ori_pcl_num();
	rf.write_attribute(pcl_data_grp_id, "ori_pcl_num", ori_pcl_num);
	size_t pcl_num = md.get_pcl_num();
	rf.write_attribute(pcl_data_grp_id, "pcl_num", pcl_num);

	ParticleData* pcl_data = new ParticleData[pcl_num];
	size_t p_id;
	for (p_id = 0; p_id < pcl_num; ++p_id)
	{
		ParticleData& pd = pcl_data[p_id];
		pd.from_pcl(md, p_id, 0);
	}
	hid_t pcl_dt_id = get_pcl_dt_id();
	int res = rf.write_dataset(
		pcl_data_grp_id,
		"field",
		pcl_num,
		pcl_data,
		pcl_dt_id
	);
	H5Tclose(pcl_dt_id);
	delete[] pcl_data;

	rf.close_group(pcl_data_grp_id);
	return res;
}

int output_pcl_data_to_hdf5_file(
	Model_T3D_ME_mt& md,
	Step_T3D_ME_mt& stp,
	ResultFile_hdf5& rf,
	hid_t grp_id
)
{
	if (grp_id < 0)
		return -1;

	hid_t pcl_data_grp_id = rf.create_group(grp_id, "ParticleData");

	size_t ori_pcl_num = md.get_ori_pcl_num();
	rf.write_attribute(pcl_data_grp_id, "ori_pcl_num", ori_pcl_num);
	size_t pcl_num = stp.get_pcl_num();
	rf.write_attribute(pcl_data_grp_id, "pcl_num", pcl_num);

	int res = 0;
	if (pcl_num)
	{
		size_t sorted_var_id = stp.get_pcl_sorted_var_id();
		const size_t* new_to_prev_pcl_map = stp.get_new_to_prev_pcl_map();
		ParticleData* pcl_data = new ParticleData[pcl_num];
		size_t p_id;
		for (p_id = 0; p_id < pcl_num; ++p_id)
		{
			ParticleData& pd = pcl_data[p_id];
			pd.from_pcl(md, p_id, sorted_var_id, new_to_prev_pcl_map);
		}
		hid_t pcl_dt_id = get_pcl_dt_id();
		res = rf.write_dataset(
			pcl_data_grp_id,
			"field",
			pcl_num,
			pcl_data,
			pcl_dt_id
			);
		H5Tclose(pcl_dt_id);
		delete[] pcl_data;
	}

	rf.close_group(pcl_data_grp_id);
	return res;
}

int load_pcl_data_from_hdf5_file(
	Model_T3D_ME_mt& md,
	ResultFile_hdf5& rf,
	hid_t grp_id
	)
{
	if (grp_id < 0)
		return -1;

	hid_t pcl_data_grp_id = rf.open_group(grp_id, "ParticleData");

	size_t ori_pcl_num, pcl_num;
	rf.read_attribute(pcl_data_grp_id, "ori_pcl_num", ori_pcl_num);
	rf.read_attribute(pcl_data_grp_id, "pcl_num", pcl_num);
	md.ori_pcl_num = ori_pcl_num;
	md.pcl_num = pcl_num;

	if (pcl_num && rf.has_dataset(pcl_data_grp_id, "ParticleData"))
	{
		ParticleData* pcls_data = new ParticleData[pcl_num];
		hid_t pcl_dt_id = get_pcl_dt_id();
		int res = rf.read_dataset(
			pcl_data_grp_id,
			"field",
			pcl_num,
			pcls_data,
			pcl_dt_id
		);
		H5Tclose(pcl_dt_id);

		if (res)
		{
			delete[] pcls_data;
			rf.close_group(pcl_data_grp_id);
			return res;
		}

		MatModel::MatModelIdToPointerMap mm_id_map(md);
		
		md.alloc_pcls(pcl_num, ori_pcl_num);
		for (size_t p_id = 0; p_id < pcl_num; ++p_id)
		{
			ParticleData& pcl_data = pcls_data[p_id];
			MatModel::MaterialModel* pmat = mm_id_map.get_mm_by_id(pcl_data.mat_id);
			if (!pmat)
			{
				throw std::exception("func load_pcl_data_from_hdf5_file error: "
					"particle has no material model.");
			}
			pcl_data.to_pcl(md, p_id, 0, *pmat);
		}
		delete[] pcls_data;
	}

	rf.close_group(pcl_data_grp_id);
	return 0;
}


int output_material_model_to_hdf5_file(
	Model_T3D_ME_mt& md,
	ResultFile_hdf5& rf,
	hid_t grp_id
	)
{
	if (grp_id < 0)
		return -1;
	hid_t mc_grp_id = rf.create_group(grp_id, "MaterialModel");
	output_material_model_container_to_hdf5_file(md, rf, mc_grp_id);
	rf.close_group(mc_grp_id);
	return 0;
}

int load_material_model_from_hdf5_file(
	Model_T3D_ME_mt &md,
	ResultFile_hdf5 &rf,
	hid_t grp_id
	)
{
	if (grp_id < 0)
		return -1;
	hid_t mc_grp_id = rf.open_group(grp_id, "MaterialModel");
	load_material_model_container_from_hdf5_file(md, rf, mc_grp_id);
	rf.close_group(mc_grp_id);
	return 0;
}

// output the whole model to ModelData
int output_model_to_hdf5_file(
	Model_T3D_ME_mt &md,
	ResultFile_hdf5 &rf
	)
{
	hid_t md_grp_id = rf.get_model_data_grp_id();
	// background mesh
	output_background_mesh_to_hdf5_file(md, rf, md_grp_id);
	// boundary condition
	output_boundary_condition_to_hdf5_file(md, rf, md_grp_id);
	// particle data
	output_ori_pcl_data_to_hdf5_file(md, rf, md_grp_id);
	// material model
	output_material_model_to_hdf5_file(md, rf, md_grp_id);
	// rigid object
	return 0;
}

// output the particle data and material models to hdf5 (used by time history)
int time_history_complete_output_to_hdf5_file(
	Model_T3D_ME_mt &md,
	Step_T3D_ME_mt &stp,
	ResultFile_hdf5 &rf,
	hid_t frame_grp_id
	)
{
	// particle data
	output_pcl_data_to_hdf5_file(md, stp, rf, frame_grp_id);
	// material model
	output_material_model_to_hdf5_file(md, rf, frame_grp_id);
	// rigid object
	return 0;
}

// load model data from hdf5 to model data
int load_me_mt_model_from_hdf5_file(
	Model_T3D_ME_mt& md,
	Step_T3D_ME_mt& step,
	const char* hdf5_name,
	const char* th_name,
	size_t frame_id
	)
{
	ResultFile_hdf5 rf;
	rf.open(hdf5_name);
	hid_t file_id = rf.get_file_id();
	if (file_id < 0)
		return -1;

	// model data
	hid_t md_grp_id = rf.get_model_data_grp_id();
	// background mesh
	load_background_mesh_from_hdf5_file(md, rf, md_grp_id);
	// boundary condition
	load_boundary_condition_from_hdf5_file(md, rf, md_grp_id);

	// time history
	hid_t th_grp_id = rf.get_time_history_grp_id();
	hid_t th_id = rf.open_group(th_grp_id, th_name);
	char th_frame_name[30];
	snprintf(th_frame_name, 30, "frame_%zu", frame_id);
	hid_t th_frame_id = rf.open_group(th_id, th_frame_name);

	// material model
	load_material_model_from_hdf5_file(md, rf, th_frame_id);
	// particle data
	load_pcl_data_from_hdf5_file(md, rf, th_frame_id);

	// rigid object

	step.is_first_step = false;
	rf.read_attribute(th_frame_id, "total_time", step.start_time);
	rf.read_attribute(th_frame_id, "total_substep_num", step.prev_substep_num);

	rf.close_group(th_frame_id);
	rf.close_group(th_id);

	return 0;
}

};