#include "Simulations_pcp.h"

#include "Model_hdf5_utilities.h"
#include "Model_T3D_CHM_s_hdf5_utilities.h"

namespace Model_T3D_CHM_s_hdf5_utilities
{
using namespace Model_hdf5_utilities;

int output_model_data_to_hdf5_file(Model_T3D_CHM_s &md, ResultFile_hdf5 &rf)
{
	hid_t md_id = rf.get_model_data_grp_id();
	
	// fluid properties
	rf.write_attribute(md_id, "Kf", md.Kf);
	rf.write_attribute(md_id, "k", md.k);
	rf.write_attribute(md_id, "miu", md.miu);

	hid_t bg_mesh_id = rf.create_group(md_id, "BackgroundMesh");

	// bg mesh attributes
	size_t node_num = md.get_node_num();
	Model_T3D_CHM_s::Node *nodes = md.get_nodes();
	size_t elem_num = md.get_elem_num();
	Model_T3D_CHM_s::Element *elems = md.get_elems();
	const char *bg_mesh_type = "T3D";
	rf.write_attribute(bg_mesh_id, "type", strlen(bg_mesh_type), bg_mesh_type);
	rf.write_attribute(bg_mesh_id, "node_num", node_num);
	rf.write_attribute(bg_mesh_id, "element_num", elem_num);
	// node coordinates
	Node3DData *nodes_data = new Node3DData[node_num];
	for (size_t n_id = 0; n_id < node_num; ++n_id)
	{
		Node3DData &node_data = nodes_data[n_id];
		Model_T3D_CHM_s::Node n = nodes[n_id];
		node_data.id = n.id;
		node_data.x = n.x;
		node_data.y = n.y;
		node_data.z = n.z;
	}
	hid_t nd_dt_id = get_nd_3d_dt_id();
	rf.write_dataset(
		bg_mesh_id,
		"NodeCoordinate",
		node_num,
		nodes_data,
		nd_dt_id
	);
	H5Tclose(nd_dt_id);
	delete[] nodes_data;
	// element indices
	Elem3DData *elems_data = new Elem3DData[elem_num];
	for (size_t e_id = 0; e_id < elem_num; ++e_id)
	{
		Elem3DData &elem_data = elems_data[e_id];
		Model_T3D_CHM_s::Element &e = elems[e_id];
		elem_data.id = e.id;
		elem_data.n1 = e.n1;
		elem_data.n2 = e.n2;
		elem_data.n3 = e.n3;
		elem_data.n4 = e.n4;
	}
	hid_t ed_dt_id = get_ed_3d_dt_id();
	rf.write_dataset(
		bg_mesh_id,
		"ElementTopology",
		elem_num,
		elems_data,
		ed_dt_id
	);
	H5Tclose(ed_dt_id);
	delete[] elems_data;
	rf.close_group(bg_mesh_id);

	return 0;
}


int load_model_data_from_hdf5_file(Model_T3D_CHM_s &md, ResultFile_hdf5 &rf)
{
	// model data output
	hid_t md_out_id = rf.get_model_data_grp_id();

	// liquid properties
	rf.read_attribute(md_out_id, "Kf", md.Kf);
	rf.read_attribute(md_out_id, "k", md.k);
	rf.read_attribute(md_out_id, "miu", md.miu);

	hid_t bg_mesh_id = rf.open_group(md_out_id, "BackgroundMesh");
	size_t node_num, elem_num;
	rf.read_attribute(md_out_id, "node_num", node_num);
	md.alloc_nodes(node_num);
	Model_T3D_CHM_s::Node *nodes = md.get_nodes();
	rf.read_attribute(md_out_id, "elem_num", elem_num);
	md.alloc_elements(elem_num);
	Model_T3D_CHM_s::Element *elems = md.get_elems();

	// nodes
	Node3DData *nodes_data = new Node3DData[node_num];
	hid_t nd_dt_id = get_nd_3d_dt_id();
	rf.read_dataset(
		bg_mesh_id,
		"NodeCoordinate",
		node_num,
		nodes_data,
		nd_dt_id
	);
	H5Tclose(nd_dt_id);
	for (size_t n_id = 0; n_id < node_num; ++n_id)
	{
		Node3DData &node_data = nodes_data[n_id];
		Model_T3D_CHM_s::Node &n = nodes[n_id];
		n.id = node_data.id;
		n.x = node_data.x;
		n.y = node_data.y;
		n.z = node_data.z;
	}
	delete[] nodes_data;

	// elements
	Elem3DData *elems_data = new Elem3DData[elem_num];
	hid_t ed_dt_id = get_ed_3d_dt_id();
	rf.read_dataset(
		bg_mesh_id,
		"ElementTopology",
		elem_num,
		elems_data,
		ed_dt_id
	);
	H5Tclose(ed_dt_id);
	for (size_t e_id = 0; e_id < elem_num; ++e_id)
	{
		Elem3DData &elem_data = elems_data[e_id];
		Model_T3D_CHM_s::Element &elem = elems[e_id];
		elem.id = elem_data.id;
		elem.n1 = elem_data.n1;
		elem.n2 = elem_data.n2;
		elem.n3 = elem_data.n3;
		elem.n4 = elem_data.n4;
	}
	delete[] elems_data;

	rf.close_group(bg_mesh_id);
	return 0;
}


int output_pcl_data_to_hdf5_file(Model_T3D_CHM_s &md, ResultFile_hdf5 &rf, hid_t frame_id)
{
	ParticleData *pcl_data = new ParticleData[md.pcl_num];
	for (size_t p_id = 0; p_id < md.pcl_num; ++p_id)
	{
		ParticleData &pd = pcl_data[p_id];
		Model_T3D_CHM_s::Particle &pcl = md.pcls[p_id];
		pd.from_pcl(pcl);
	}
	hid_t pcl_dt_id = get_pcl_dt_id();
	int res = rf.write_dataset(
		frame_id,
		"ParticleData",
		md.pcl_num,
		pcl_data,
		pcl_dt_id
	);
	H5Tclose(pcl_dt_id);
	delete[] pcl_data;
	// particle num
	hid_t pcl_data_id = rf.open_dataset(frame_id, "ParticleData");
	rf.write_attribute(pcl_data_id, "pcl_num", md.pcl_num);
	rf.close_dataset(pcl_data_id);
	return res;
}

int load_pcl_data_from_hdf5_file(Model_T3D_CHM_s &md, ResultFile_hdf5 &rf, hid_t frame_id)
{
	// particle num
	size_t pcl_num;
	hid_t pcl_data_id = rf.open_dataset(frame_id, "ParticleData");
	rf.read_attribute(pcl_data_id, "pcl_num", pcl_num);
	rf.close_dataset(pcl_data_id);

	// particle data
	ParticleData *pcls_data = new ParticleData[pcl_num];
	hid_t pcl_dt_id = get_pcl_dt_id();
	int res = rf.read_dataset(
		frame_id,
		"ParticleData",
		pcl_num,
		pcls_data,
		pcl_dt_id
	);
	H5Tclose(pcl_dt_id);
	if (res) return res;
	md.alloc_pcls(pcl_num);
	for (size_t p_id = 0; p_id < pcl_num; ++p_id)
	{
		ParticleData &pcl_data = pcls_data[p_id];
		Model_T3D_CHM_s::Particle &pcl = md.pcls[p_id];
		pcl_data.to_pcl(pcl);
	}
	delete[] pcls_data;
	return 0;
}


namespace
{
	template <typename Particle>
	inline size_t get_pcl_id(void *ext_data)
	{
		return static_cast<Particle *>(ext_data)->id;
	}
};


int output_material_model_to_hdf5_file(Model_T3D_CHM_s &md, ResultFile_hdf5 &rf, hid_t frame_id)
{
	using namespace MatModel;

	size_t mm_id, mm_num;
	hid_t mm_grp_id = rf.create_group(frame_id, "MaterialModel");

	// linear elasticity
	mm_num = md.get_num_LinearElasticity();
	if (mm_num)
	{
		LinearElasticityStateData *mm_data = new LinearElasticityStateData[mm_num];
		mm_id = 0;
		for (LinearElasticity *iter = md.first_LinearElasticity(); 
			 iter; iter = md.next_LinearElasticity(iter))
		{
			mm_data[mm_id].pcl_id = get_pcl_id<Model_T3D_CHM_s::Particle>(iter->ext_data);
			mm_data[mm_id].from_mm(*iter);
			++mm_id;
		}
		hid_t le_dt_id = get_le_hdf5_dt_id();
		rf.write_dataset(mm_grp_id, "LinearElasticity", mm_num, mm_data, le_dt_id);
		H5Tclose(le_dt_id);
		delete[] mm_data;
		hid_t mm_dset_id = rf.open_dataset(mm_grp_id, "LinearElasticity");
		rf.write_attribute(mm_dset_id, "mm_num", mm_num);
		rf.close_dataset(mm_dset_id);
	}

	mm_num = md.get_num_ModifiedCamClay();
	if (mm_num)
	{
		ModifiedCamClayStateData *mm_data = new ModifiedCamClayStateData[mm_num];
		mm_id = 0;
		for (ModifiedCamClay *iter = md.first_ModifiedCamClay();
			 iter; iter = md.next_ModifiedCamClay(iter))
		{
			mm_data[mm_id].pcl_id = get_pcl_id<Model_T3D_CHM_s::Particle>(iter->ext_data);
			mm_data[mm_id].from_mm(*iter);
			++mm_id;
		}
		hid_t mcc_dt_id = get_mcc_hdf5_dt_id();
		rf.write_dataset(mm_grp_id, "ModifiedCamClay", mm_num, mm_data, mcc_dt_id);
		H5Tclose(mcc_dt_id);
		delete[] mm_data;
		hid_t mm_dset_id = rf.open_dataset(mm_grp_id, "ModifiedCamClay");
		rf.write_attribute(mm_dset_id, "mm_num", mm_num);
		rf.close_dataset(mm_dset_id);
	}

	rf.close_group(mm_grp_id);
	return 0;
}

int load_material_model_from_hdf5_file(Model_T3D_CHM_s &md, ResultFile_hdf5 &rf, hid_t frame_id)
{
	using namespace MatModel;
	hid_t mm_dset_id;
	size_t mm_num;
	hid_t mm_grp_id = rf.open_group(frame_id, "MaterialModel");

	// linear elasticity
	if (rf.has_dataset(mm_grp_id, "LinearElasticity"))
	{
		// mm_num
		mm_dset_id = rf.open_dataset(mm_grp_id, "LinearElasticity");
		rf.read_attribute(mm_dset_id, "mm_num", mm_num);
		rf.close_dataset(mm_dset_id);
		// get data
		LinearElasticityStateData *mm_data = new LinearElasticityStateData[mm_num];
		hid_t le_dt_id = get_le_hdf5_dt_id();
		rf.read_dataset(
			mm_grp_id,
			"LinearElasticity",
			mm_num,
			mm_data,
			le_dt_id
			);
		H5Tclose(le_dt_id);
		LinearElasticity *mms = md.add_LinearElasticity(mm_num);
		for (size_t mm_id = 0; mm_id < mm_num; ++mm_id)
		{
			LinearElasticityStateData &mmd = mm_data[mm_id];
			LinearElasticity &mm = mms[mm_id];
			mmd.to_mm(mm);
			md.pcls[mmd.pcl_id].set_mat_model(mm);
		}
		delete[] mm_data;
	}

	// modified cam clay
	if (rf.has_dataset(mm_grp_id, "ModifiedCamClay"))
	{
		// mm_num
		mm_dset_id = rf.open_dataset(mm_grp_id, "ModifiedCamClay");
		rf.read_attribute(mm_dset_id, "mm_num", mm_num);
		rf.close_dataset(mm_dset_id);
		// get data
		ModifiedCamClayStateData *mm_data = new ModifiedCamClayStateData[mm_num];
		hid_t mcc_dt_id = get_mcc_hdf5_dt_id();
		rf.read_dataset(
			mm_grp_id,
			"ModifiedCamClay",
			mm_num,
			mm_data,
			mcc_dt_id
			);
		H5Tclose(mcc_dt_id);
		ModifiedCamClay *mms = md.add_ModifiedCamClay(mm_num);
		for (size_t mm_id = 0; mm_id < mm_num; ++mm_id)
		{
			ModifiedCamClayStateData &mmd = mm_data[mm_id];
			ModifiedCamClay &mm = mms[mm_id];
			mmd.to_mm(mm);
			md.pcls[mmd.pcl_id].set_mat_model(mm);
		}
		delete[] mm_data;
	}

	rf.close_group(mm_grp_id);
	return 0;
}


int load_model_from_hdf5_file(Model_T3D_CHM_s &md,
	const char *hdf5_name, const char *th_name, size_t frame_id)
{
	ResultFile_hdf5 rf;
	rf.open(hdf5_name);
	hid_t file_id = rf.get_file_id();
	if (file_id < 0) return -1;

	// model data (mainly bg mesh)
	load_model_data_from_hdf5_file(md, rf);

	// time history output
	hid_t th_grp_id = rf.get_time_history_grp_id();
	hid_t th_id = rf.open_group(th_grp_id, th_name);
	char th_frame_name[30];
	snprintf(th_frame_name, 30, "frame_%zu", frame_id);
	hid_t th_frame_id = rf.open_group(th_id, th_frame_name);
	// particle data
	load_pcl_data_from_hdf5_file(md, rf, th_frame_id);
	// constitutive model data
	load_material_model_from_hdf5_file(md, rf, th_frame_id);
	rf.close_group(th_frame_id);
	rf.close_group(th_id);

	return 0;
}

};