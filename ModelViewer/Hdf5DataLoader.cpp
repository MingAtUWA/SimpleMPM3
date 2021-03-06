#include "ModelViewer_pcp.h"

#include <exception>

#include "Hdf5DataLoader.h"

Hdf5DataLoader::Hdf5DataLoader() :
	res_file(nullptr), th_id(-1), pcl_dt_id(-1), pcl_size(0),
	frame_id(std::numeric_limits<size_t>::max()), pcl_num(0) {}

Hdf5DataLoader::~Hdf5DataLoader() { close_res_file(); }

void Hdf5DataLoader::close_res_file()
{
	if (!res_file)
		return;
	ResultFile_hdf5& rf = *res_file;
	if (th_id >= 0)
	{
		rf.close_group(th_id);
		th_id = -1;
	}
	if (pcl_dt_id >= 0)
	{
		H5Tclose(pcl_dt_id);
		pcl_dt_id = -1;
	}
	res_file = nullptr;
}

int Hdf5DataLoader::set_time_history(
	ResultFile_hdf5& rf,
	const char* th_name
	)
{
	char exception_msg[250];
	close_res_file();

	res_file = &rf;

	hid_t th_grp_id = rf.get_time_history_grp_id();
	// check if the time history exists
	if (!rf.has_group(th_grp_id, th_name))
	{
		snprintf(exception_msg, sizeof(exception_msg),
			"hdf5 file has no time history %s.", th_name);
		throw std::exception(exception_msg);
		close_res_file();
		return -1;
	}
	th_id = rf.open_group(th_grp_id, th_name);

	if (!rf.has_group(th_id, "frame_0"))
	{
		snprintf(exception_msg, sizeof(exception_msg),
			"hdf5 file doesn't have frame_0 in time history %s.", th_name);
		throw std::exception(exception_msg);
		close_res_file();
		return -2;
	}
	
	hid_t frame_grp_id = rf.open_group(th_id, "frame_0");
	hid_t pcl_grp_id = rf.open_group(frame_grp_id, "ParticleData");
	hid_t pcl_dset_id = rf.open_dataset(pcl_grp_id, "field");
	// get field data type
	pcl_dt_id = H5Dget_type(pcl_dset_id);
	pcl_size = H5Tget_size(pcl_dt_id);
	rf.close_dataset(pcl_dset_id);
	rf.close_group(pcl_grp_id);
	rf.close_group(frame_grp_id);
	return 0;
}

int Hdf5DataLoader::load_frame_data(
	size_t fm_id,
	bool need_mat_model
	)
{
	if (frame_id == fm_id)
		return 0;

	int res = 0;
	ResultFile_hdf5& rf = *res_file;
	char frame_name[50];
	snprintf(frame_name, sizeof(frame_name), "frame_%zu", fm_id);
	hid_t frame_grp_id = rf.open_group(th_id, frame_name);
	hid_t pcl_data_id = rf.open_group(frame_grp_id, "ParticleData");

	// pcl num
	res = rf.read_attribute(pcl_data_id, "pcl_num", pcl_num);
	if (res)
		goto exit;

	if (pcl_num)
	{
		// pcl data
		pcl_fld_data_mem.reserve(pcl_size * pcl_num);
		char* pcls_data = pcl_fld_data_mem.get_mem();
		res = rf.read_dataset(pcl_data_id, "field", pcl_num, (void*)pcls_data, pcl_dt_id);
		if (res)
			goto exit;
	}

	if (need_mat_model && rf.has_group(frame_grp_id, "MaterialModel"))
	{
		mat_model_map.clear();
		MatModelPointer mm_pt;

		hid_t mat_model_id = rf.open_group(frame_grp_id, "MaterialModel");

		if (rf.has_dataset(mat_model_id, "LinearElasticity"))
		{
			hid_t le_grp = rf.open_dataset(mat_model_id, "LinearElasticity");
			hid_t le_dt = Model_hdf5_utilities::get_le_hdf5_dt_id();
			rf.read_attribute(mat_model_id, "LinearElasticity_num", LinearElasticity_num);
			LinearElasticity_mem.reserve(LinearElasticity_num);
			LinearElasticityStateData *le_mem = LinearElasticity_mem.get_mem();
			rf.read_dataset(
				mat_model_id,
				"LinearElasticity",
				LinearElasticity_num,
				le_mem,
				le_dt
				);
			mm_pt.type = MatModelType::LinearElasticity;
			for (size_t mm_id = 0; mm_id < LinearElasticity_num; ++mm_id)
			{
				LinearElasticityStateData& mm = le_mem[mm_id];
				mm_pt.pmat = &mm;
				mat_model_map.emplace(mm.id, mm_pt);
			}
			H5Tclose(le_dt);
			rf.close_dataset(le_grp);
		}

		if (rf.has_dataset(mat_model_id, "ModifiedCamClay"))
		{
			hid_t mcc_grp = rf.open_dataset(mat_model_id, "ModifiedCamClay");
			hid_t mcc_dt = Model_hdf5_utilities::get_mcc_hdf5_dt_id();
			rf.read_attribute(mat_model_id, "ModifiedCamClay_num", ModifiedCamClay_num);
			ModifiedCamClay_mem.reserve(ModifiedCamClay_num);
			ModifiedCamClayStateData* mcc_mem = ModifiedCamClay_mem.get_mem();
			rf.read_dataset(
				mat_model_id,
				"ModifiedCamClay",
				ModifiedCamClay_num,
				mcc_mem,
				mcc_dt
				);
			mm_pt.type = MatModelType::ModifiedCamClay;
			for (size_t mm_id = 0; mm_id < ModifiedCamClay_num; ++mm_id)
			{
				ModifiedCamClayStateData &mm = mcc_mem[mm_id];
				mm_pt.pmat = &mm;
				mat_model_map.emplace(mm.id, mm_pt);
			}
			H5Tclose(mcc_dt);
			rf.close_dataset(mcc_grp);
		}

		if (rf.has_dataset(mat_model_id, "VonMises"))
		{
			hid_t vm_grp = rf.open_dataset(mat_model_id, "VonMises");
			hid_t vm_dt = Model_hdf5_utilities::get_von_mises_hdf5_dt_id();
			rf.read_attribute(mat_model_id, "VonMises_num", VonMises_num);
			VonMises_mem.reserve(VonMises_num);
			VonMisesStateData* vm_mem = VonMises_mem.get_mem();
			rf.read_dataset(
				mat_model_id,
				"VonMises",
				VonMises_num,
				vm_mem,
				vm_dt
				);
			mm_pt.type = MatModelType::VonMises;
			for (size_t mm_id = 0; mm_id < VonMises_num; ++mm_id)
			{
				VonMisesStateData &mm = vm_mem[mm_id];
				mm_pt.pmat = &mm;
				mat_model_map.emplace(mm.id, mm_pt);
			}
			H5Tclose(vm_dt);
			rf.close_dataset(vm_grp);
		}

		if (rf.has_dataset(mat_model_id, "Tresca"))
		{
			hid_t tc_grp = rf.open_dataset(mat_model_id, "Tresca");
			hid_t tc_dt = Model_hdf5_utilities::get_tresca_hdf5_dt_id();
			rf.read_attribute(mat_model_id, "Tresca_num", Tresca_num);
			Tresca_mem.reserve(Tresca_num);
			TrescaStateData* tc_mem = Tresca_mem.get_mem();
			rf.read_dataset(
				mat_model_id,
				"Tresca",
				Tresca_num,
				tc_mem,
				tc_dt
				);
			mm_pt.type = MatModelType::Tresca;
			for (size_t mm_id = 0; mm_id < Tresca_num; ++mm_id)
			{
				TrescaStateData &mm = tc_mem[mm_id];
				mm_pt.pmat = &mm;
				mat_model_map.emplace(mm.id, mm_pt);
			}
			H5Tclose(tc_dt);
			rf.close_dataset(tc_grp);
		}

		rf.close_group(mat_model_id);
	}

	frame_id = fm_id;
exit:
	rf.close_group(pcl_data_id);
	rf.close_group(frame_grp_id);
	return 0;
}

using Model_hdf5_utilities::LinearElasticityStateData;
using Model_hdf5_utilities::ModifiedCamClayStateData;
using Model_hdf5_utilities::VonMisesStateData;
using Model_hdf5_utilities::TrescaStateData;

const Hdf5DataLoader::MatModelInfo
Hdf5DataLoader::mat_model_info[] = {
	{
		sizeof(LinearElasticityStateData),
		offsetof(LinearElasticityStateData, s11),
		offsetof(LinearElasticityStateData, s22),
		offsetof(LinearElasticityStateData, s33),
		offsetof(LinearElasticityStateData, s12),
		offsetof(LinearElasticityStateData, s23),
		offsetof(LinearElasticityStateData, s31)
	},
	{
		sizeof(ModifiedCamClayStateData),
		offsetof(ModifiedCamClayStateData, s11),
		offsetof(ModifiedCamClayStateData, s22),
		offsetof(ModifiedCamClayStateData, s33),
		offsetof(ModifiedCamClayStateData, s12),
		offsetof(ModifiedCamClayStateData, s23),
		offsetof(ModifiedCamClayStateData, s31)
	},
	{
		sizeof(VonMisesStateData),
		offsetof(VonMisesStateData, s11),
		offsetof(VonMisesStateData, s22),
		offsetof(VonMisesStateData, s33),
		offsetof(VonMisesStateData, s12),
		offsetof(VonMisesStateData, s23),
		offsetof(VonMisesStateData, s31)
	},
	{
		sizeof(TrescaStateData),
		offsetof(TrescaStateData, s11),
		offsetof(TrescaStateData, s22),
		offsetof(TrescaStateData, s33),
		offsetof(TrescaStateData, s12),
		offsetof(TrescaStateData, s23),
		offsetof(TrescaStateData, s31)
	}
};
