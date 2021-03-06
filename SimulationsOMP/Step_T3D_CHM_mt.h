#ifndef __Step_T3D_CHM_mt_h__
#define __Step_T3D_CHM_mt_h__

#include "CacheAlignedMem.h"
#include "Step_OMP.h"
#include "Model_T3D_CHM_mt.h"
#include "RigidObject/Force2D.h"

class Model_T3D_CHM_mt;
class Step_T3D_CHM_mt;
namespace Model_T3D_CHM_mt_hdf5_utilities
{
	int load_model_from_hdf5_file(Model_T3D_CHM_mt& md, Step_T3D_CHM_mt& step,
		const char* hdf5_name, const char* th_name, size_t frame_id);
}

int substep_func_omp_T3D_CHM_mt(void* _self, size_t my_th_id,
	double dt, double cur_time, size_t substp_id);
int substep_func_omp_T3D_CHM_mt2(void* _self, size_t my_th_id,
	double dt, double cur_time, size_t substp_id);

class Step_T3D_CHM_mt : public Step_OMP
{
protected:
	typedef Model_T3D_CHM_mt::ShapeFunc ShapeFunc;
	typedef Model_T3D_CHM_mt::DShapeFuncABC DShapeFuncAB;
	typedef Model_T3D_CHM_mt::DShapeFuncD DShapeFuncC;
	typedef Model_T3D_CHM_mt::Force Force;
	typedef Model_T3D_CHM_mt::Position Position;
	typedef Model_T3D_CHM_mt::Displacement Displacement;
	typedef Model_T3D_CHM_mt::Velocity Velocity;
	typedef Model_T3D_CHM_mt::Acceleration Acceleration;
	typedef Model_T3D_CHM_mt::Stress Stress;
	typedef Model_T3D_CHM_mt::Strain Strain;
	typedef Model_T3D_CHM_mt::StrainInc StrainInc;
	typedef Model_T3D_CHM_mt::SortedPclVarArrays SortedPclVarArrays;
	typedef Model_T3D_CHM_mt::ElemNodeIndex ElemNodeIndex;
	typedef Model_T3D_CHM_mt::ElemNodeVM ElemNodeVM;
	typedef Model_T3D_CHM_mt::NodeHasVBC NodeHasVBC;

	double* pcl_m_s; // ori_pcl_num
	double* pcl_density_s; // ori_pcl_num
	double* pcl_vol_s; // ori_pcl_num
	Force* pcl_bf_s; // ori_pcl_num
	Force* pcl_bf_f; // ori_pcl_num
	Force* pcl_t; // ori_pcl_num
	Position* pcl_pos; // ori_pcl_num
	double* pcl_vol; // ori_pcl_num
	MatModel::MaterialModel** pcl_mat_model; // ori_pcl_num

	SortedPclVarArrays sorted_pcl_var_arrays[2];

	ElemNodeIndex *elem_node_id;
	DShapeFuncAB *elem_N_abc;
	DShapeFuncC *elem_N_d;
	double* elem_vol;

	double* elem_density_f; // elem_num
	double* elem_pcl_n; // elem_num
	double* elem_pcl_m_s; // elem_num
	double* elem_pcl_m_f; // elem_num
	StrainInc *elem_de; // elem_num
	double *elem_p; // elem_num
	double *elem_n2_miu_div_k_vol;
	Force *elem_seep_force;
	double* elem_m_de_vol_s; // elem_num
	double* elem_m_de_vol_f; // elem_num

	// element-node data
	ElemNodeVM* elem_node_vm_s; // elem_num * 3
	ElemNodeVM* elem_node_vm_f; // elem_num * 3
	Force* elem_node_force_s; // elem_num * 3
	Force* elem_node_force_f; // elem_num * 3

	Acceleration* node_a_s; // node_num
	Acceleration* node_a_f; // node_num
	Velocity* node_v_s; // node_num
	Velocity* node_v_f; // node_num
	NodeHasVBC* node_has_vbc_s; // node_num
	NodeHasVBC* node_has_vbc_f; // node_num
	double* node_am_s; // node_num
	double* node_am_f; // node_num
	double* node_de_vol_s; // node_num
	double* node_de_vol_f; // node_num

	size_t elem_num, node_num;
	double k, miu, Kf;

	RigidCylinder *prcy;
	RigidObjectByT3DMesh *prm;
	ContactModel3D *pcm_s, *pcm_f;
	
	size_t* contact_substep_id_s;
	Position* prev_contact_pos_s;
	Force* prev_contact_tan_force_s;
	size_t* contact_substep_id_f;
	Position* prev_contact_pos_f;
	Force* prev_contact_tan_force_f;

#ifdef _DEBUG
	size_t prev_valid_pcl_num_tmp;
#endif
	size_t prev_valid_pcl_num, valid_pcl_num;
	size_t valid_elem_num;
	Force3D cf_tmp;

	size_t* elem_count_bin;
	size_t* elem_sum_bin;

	size_t* prev_pcl_ids[2]; // (pcl_num + 2) * 2
	size_t* pcl_in_elems[2]; // pcl_num * 2
	size_t* valid_elem_id; // elem_num
	size_t* node_has_elems[2]; // (elem_num * 4 + 2) * 2
	size_t* node_elem_pairs[2]; // elem_num * 4 * 2
	
	union ThreadData
	{
		struct
		{
			size_t sorted_pcl_var_id;
			size_t sorted_pcl_in_elem_id;
			double max_pcl_vol;
		};
		char padding[Cache_Alignment * 2];
	};
	ThreadData* thread_datas;
	
	CacheAlignedMem thread_mem;
	CacheAlignedMem cal_mem;
	
	int apply_rigid_cylinder(
		size_t p_id0, size_t p_id1,
		const size_t* pcl_in_elem,
		const SortedPclVarArrays& cur_spva,
		Force3D& rc_cf,
		size_t substp_id,
		ThreadData& thd) noexcept;
	
	int apply_t3d_rigid_mesh(
		size_t p_id0, size_t p_id1,
		const size_t *pcl_in_elem,
		const SortedPclVarArrays& cur_spva,
		Force3D& rc_cf,
		size_t substp_id,
		ThreadData& thd) noexcept;

	ParticleVariablesGetter pv_place_holder;

public:
	int init_calculation() override;
	friend int substep_func_omp_T3D_CHM_mt(void* _self,
		size_t my_th_id, double dt, double cur_time, size_t substp_id);
	friend int substep_func_omp_T3D_CHM_mt2(void* _self,
		size_t my_th_id, double dt, double cur_time, size_t substp_id);
	int finalize_calculation() override;

public:
	Step_T3D_CHM_mt(const char* _name);
	~Step_T3D_CHM_mt();

	inline size_t get_pcl_num() const noexcept { return prev_valid_pcl_num; }
	inline size_t get_sorted_pcl_var_id() const noexcept { return thread_datas[0].sorted_pcl_var_id; }
	inline size_t* get_prev_pcl_id() const noexcept { return prev_pcl_ids[thread_datas[0].sorted_pcl_in_elem_id]; }
	inline size_t* get_pcl_in_elem() const noexcept { return pcl_in_elems[thread_datas[0].sorted_pcl_in_elem_id]; }

	friend int Model_T3D_CHM_mt_hdf5_utilities::load_model_from_hdf5_file(
		Model_T3D_CHM_mt &md, Step_T3D_CHM_mt &step, const char *hdf5_name,
		const char* th_name, size_t frame_id);
};

#endif