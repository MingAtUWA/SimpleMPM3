#ifndef __Hdf5_Data_Loader_h__
#define __Hdf5_Data_Loader_h__

#include <unordered_map>

#include "hdf5.h"
#include "ItemArray.hpp"
#include "ResultFile_hdf5.h"
#include "Model_hdf5_utilities.h"

class Hdf5DataLoader
{
protected:
	ResultFile_hdf5 *res_file;
	hid_t th_id;

	hid_t pcl_dt_id;
	size_t pcl_size;

	size_t frame_id;
	size_t pcl_num;
	MemoryUtils::ItemArray<char> pcl_fld_data_mem;

public:
	Hdf5DataLoader();
	~Hdf5DataLoader();
	void close_res_file();
	
	int set_time_history(ResultFile_hdf5& rf, const char* th_name);

	inline hid_t get_time_history_id() const noexcept { return th_id; }
	inline hid_t get_pcl_data_type() const noexcept { return pcl_dt_id; }
	inline size_t get_pcl_size() const noexcept { return pcl_size; }

	inline size_t get_pcl_num() const noexcept { return pcl_num; }
	inline char* get_pcl_field_data() const noexcept
	{ return pcl_fld_data_mem.get_mem(); }

public:
	enum class MatModelType : unsigned char
	{
		LinearElasticity = 0,
		ModifiedCamClay = 1,
		VonMises = 2,
		Tresca = 3
	};
	struct MatModelPointer
	{
		MatModelType type;
		void *pmat;
		inline MatModelPointer() {}
		inline MatModelPointer(const MatModelPointer &other)
		{
			type = other.type;
			pmat = other.pmat;
		}
	};
	typedef std::unordered_map<size_t, MatModelPointer> MatModelMap;
	typedef Model_hdf5_utilities::LinearElasticityStateData LinearElasticityStateData;
	typedef Model_hdf5_utilities::ModifiedCamClayStateData ModifiedCamClayStateData;
	typedef Model_hdf5_utilities::VonMisesStateData VonMisesStateData;
	typedef Model_hdf5_utilities::TrescaStateData TrescaStateData;
	
	struct MatModelInfo
	{
		size_t mat_data_size;
		size_t s11_off, s22_off, s33_off;
		size_t s12_off, s23_off, s31_off;
	};

protected:
	MatModelMap mat_model_map;
	size_t LinearElasticity_num;
	size_t ModifiedCamClay_num;
	size_t VonMises_num;
	size_t Tresca_num;
	MemoryUtils::ItemArray<LinearElasticityStateData> LinearElasticity_mem;
	MemoryUtils::ItemArray<ModifiedCamClayStateData> ModifiedCamClay_mem;
	MemoryUtils::ItemArray<VonMisesStateData> VonMises_mem;
	MemoryUtils::ItemArray<TrescaStateData> Tresca_mem;
	static const MatModelInfo mat_model_info[];

public:
	inline MatModelMap& get_mat_model_map() { return mat_model_map; }
	inline static const MatModelInfo& get_mat_model_info(MatModelType type)
	{ return mat_model_info[(unsigned char)type]; }

	int load_frame_data(size_t fm_id, bool need_mat_model = false);
};

#endif