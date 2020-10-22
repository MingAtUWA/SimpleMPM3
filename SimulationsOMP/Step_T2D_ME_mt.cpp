#include "SimulationsOMP_pcp.h"

#include <iostream>
#include <fstream>
#include <omp.h>

#include "Step_T2D_ME_mt.h"

#define one_third (1.0f/3.0f)
#define N_min (1.0e-5f)
#define Block_Low(th_id, th_num, data_num) ((th_id)*(data_num)/(th_num))

static std::fstream res_file_t2d_me_mt;

Step_T2D_ME_mt::Step_T2D_ME_mt(const char* _name) : 
	Step_OMP(_name, "Step_T2D_ME_mt", &substep_func_omp_T2D_ME_mt) {}

Step_T2D_ME_mt::~Step_T2D_ME_mt() {}

namespace
{
	inline void divide_task_to_thread(
		uint32_t thread_num,
		uint32_t data_num,
		uint32_t data_range[] // len = thread_num+1
		)
	{
		data_range[0] = 0;
		data_range[thread_num] = data_num;
		register uint32_t i;
		for (i = 1; i < thread_num; ++i)
			data_range[i] = Block_Low(i, thread_num, data_num);
	}
}

int Step_T2D_ME_mt::init_calculation()
{
	res_file_t2d_me_mt.open("me_mt_res.txt", std::ios::out | std::ios::binary);
	
	Model_T2D_ME_mt &md = *(Model_T2D_ME_mt *)model;

	omp_set_num_threads(thread_num);

	uint32_t th_num_1 = thread_num + 1;
	char *mem_range = (char *)task_range_mem.alloc((sizeof(PclRange) + sizeof(uint32_t) * 3) * size_t(th_num_1));
	pcl_range = (PclRange *)mem_range;
	mem_range += sizeof(PclRange) * th_num_1;
	elem_range = (uint32_t *)mem_range;
	mem_range += sizeof(uint32_t) * th_num_1;
	node_elem_range = (uint32_t *)mem_range;
	mem_range += sizeof(uint32_t) * th_num_1;
	node_range = (uint32_t*)mem_range;

	pcl_range[0].id = 0;
	divide_task_to_thread(thread_num, md.elem_num, elem_range);
	divide_task_to_thread(thread_num, md.node_num, node_range);
	node_elem_range[0] = 0;
	uint32_t th_id;
	for (th_id = 0; th_id < thread_num; ++th_id)
		node_elem_range[th_id + 1] = md.node_elem_list[node_range[th_id+1] - 1];

	radix_sort_var_id = 0;
	new_to_prev_pcl_maps[0] = (uint32_t *)radix_sort_var_mem.alloc(sizeof(uint32_t) * (size_t(md.pcl_num) * 4 + 2) + Cache_Alignment * 3);
	new_to_prev_pcl_maps[1] = cache_aligned(new_to_prev_pcl_maps[0] + md.pcl_num);
	pcl_in_elem_arrays[0] = cache_aligned(new_to_prev_pcl_maps[1] + md.pcl_num);
	pcl_in_elem_arrays[1] = cache_aligned(pcl_in_elem_arrays[0] + md.pcl_num + 1);
	pcl_in_elem_arrays[0][md.pcl_num] = md.elem_num;
	pcl_in_elem_arrays[1][md.pcl_num] = md.elem_num;

	elem_count_bin = (uint32_t *)elem_bin_mem.alloc(sizeof(uint32_t) * thread_num * 0x100 * 2);
	elem_sum_bin = elem_count_bin + size_t(thread_num) * 0x100;

	pcl_num = md.pcl_num;
	elem_num = md.elem_num;
	node_num = md.node_num;

	pcl_m = md.pcl_m;
	pcl_bf = md.pcl_bf;
	pcl_t = md.pcl_t;
	pcl_pos = md.pcl_pos;
	pcl_mat_model = md.pcl_mat_model;

	pcl_sorted_var_id = 1;

	PclSortedVarArray &md_pscv0 = md.pcl_sorted_var_array[0];
	PclSortedVarArray &pscv0 = pcl_sorted_var_array[0];
	pscv0.pcl_index = md_pscv0.pcl_index;
	pscv0.pcl_density = md_pscv0.pcl_density;
	pscv0.pcl_disp = md_pscv0.pcl_disp;
	pscv0.pcl_v = md_pscv0.pcl_v;
	pscv0.pcl_N = md_pscv0.pcl_N;
	pscv0.pcl_stress = md_pscv0.pcl_stress;

	PclSortedVarArray& md_pscv1 = md.pcl_sorted_var_array[1];
	PclSortedVarArray& pscv1 = pcl_sorted_var_array[1];
	pscv1.pcl_index = md_pscv1.pcl_index;
	pscv1.pcl_density = md_pscv1.pcl_density;
	pscv1.pcl_disp = md_pscv1.pcl_disp;
	pscv1.pcl_v = md_pscv1.pcl_v;
	pscv1.pcl_N = md_pscv1.pcl_N;
	pscv1.pcl_stress = md_pscv1.pcl_stress;

	elem_node_id = md.elem_node_id;
	elem_area = md.elem_area;
	elem_sf_ab = md.elem_sf_ab;
	elem_sf_c = md.elem_sf_c;

	elem_density = md.elem_density;
	elem_pcl_m = md.elem_pcl_m;
	elem_pcl_vol = md.elem_pcl_vol;
	elem_de = md.elem_de;
	elem_stress = md.elem_stress;
	elem_m_de_vol = md.elem_m_de_vol;

	elem_node_vm = md.elem_node_vm;
	elem_node_force = md.elem_node_force;

	elem_id_array = md.elem_id_array;
	node_elem_id_array = md.node_elem_id_array;
	node_elem_list = md.node_elem_list;
	node_a = md.node_a;
	node_v = md.node_v;
	node_has_vbc = md.node_has_vbc;
	node_am = md.node_am;
	node_de_vol = md.node_de_vol;
	
	for (th_id = 1; th_id < th_num_1; ++th_id)
		pcl_range[th_id].id = Block_Low(th_id, thread_num, pcl_num);
	PclSortedVarArray &psva = md.pcl_sorted_var_array[0];
	uint32_t next_pcl_num = 0;
#pragma omp parallel
	{
		uint32_t my_th_id = omp_get_thread_num();
		
		uint32_t e_id0 = elem_range[my_th_id];
		uint32_t e_id1 = elem_range[my_th_id + 1];
		memset(elem_pcl_m + e_id0, 0, (e_id1 - e_id0) * sizeof(float));
		memset(elem_pcl_vol + e_id0, 0, (e_id1 - e_id0) * sizeof(float));
		memset(elem_stress + e_id0, 0, (e_id1 - e_id0) * sizeof(ElemStress));
		memset(elem_node_vm + e_id0 * 3, 0, (e_id1 - e_id0) * 3 * sizeof(ElemNodeVM));
		memset(elem_node_force + e_id0 * 3, 0, (e_id1 - e_id0) * 3 * sizeof(ElemNodeForce));

		uint32_t sort_var_id = radix_sort_var_id;
		uint32_t* new_to_prev_pcl_map = new_to_prev_pcl_maps[sort_var_id];
		uint32_t* new_to_prev_pcl_map_tmp = new_to_prev_pcl_maps[sort_var_id ^ 1];
		uint32_t* pcl_in_elem_array = pcl_in_elem_arrays[sort_var_id];
		uint32_t* pcl_in_elem_array_tmp = pcl_in_elem_arrays[sort_var_id ^ 1];

		uint32_t p_id0 = pcl_range[my_th_id].id;
		uint32_t p_id1 = pcl_range[my_th_id + 1].id;
		uint32_t pcl_in_mesh_num = 0;
		uint32_t e_id, p_id;
		for (p_id = p_id0; p_id < p_id1; ++p_id)
		{
			PclDisp& p_d = psva.pcl_disp[p_id];
			p_d.ux = 0.0f;
			p_d.uy = 0.0f;

			PclPos& p_p = md.pcl_pos[p_id];
			PclShapeFunc& p_N = psva.pcl_N[p_id];
			e_id = md.find_pcl_in_which_elem(p_p.x, p_p.y, p_N);
			if (e_id != elem_num)
			{
				if (p_N.N1 < N_min)
					p_N.N1 = N_min;
				if (p_N.N2 < N_min)
					p_N.N2 = N_min;
				if (p_N.N3 < N_min)
					p_N.N3 = N_min;
				++pcl_in_mesh_num;
			}
			new_to_prev_pcl_map[p_id] = p_id;
			pcl_in_elem_array[p_id] = e_id;
		}

#pragma omp critical
		next_pcl_num += pcl_in_mesh_num;
		
		uint32_t* my_cbin = elem_count_bin + size_t(my_th_id) * 0x100;
		uint32_t* my_sbin = elem_sum_bin + size_t(my_th_id) * 0x100;
		uint32_t data_digit, bin_id, th_id;
		uint32_t* other_cbin;
		for (uint32_t digit_disp = 0, elem_num_tmp = elem_num;
			 elem_num_tmp; digit_disp += 8, elem_num_tmp >>= 8)
		{
			memset(my_cbin, 0, sizeof(uint32_t) * 0x100);

			for (p_id = p_id0; p_id < p_id1; ++p_id)
			{
				data_digit = (pcl_in_elem_array[p_id] >> digit_disp) & 0xFF;
				++my_cbin[data_digit];
			}

			my_sbin[0] = my_cbin[0];
			for (bin_id = 1; bin_id < 0x100; ++bin_id)
			{
				my_cbin[bin_id] += my_cbin[bin_id - 1];
				my_sbin[bin_id] = my_cbin[bin_id];
			}
#pragma omp barrier

			for (th_id = 0; th_id < my_th_id; ++th_id)
			{
				other_cbin = elem_count_bin + (size_t(th_id) << 8);
				for (bin_id = 0; bin_id < 0x100; ++bin_id)
					my_sbin[bin_id] += other_cbin[bin_id];
			}
			for (th_id = my_th_id + 1; th_id < thread_num; ++th_id)
			{
				other_cbin = elem_count_bin + (size_t(th_id) << 8);
				for (bin_id = 1; bin_id < 0x100; ++bin_id)
					my_sbin[bin_id] += other_cbin[bin_id - 1];
			}

			for (p_id = p_id1; p_id-- > p_id0;)
			{
				data_digit = (pcl_in_elem_array[p_id] >> digit_disp) & 0xFF;
				pcl_in_elem_array_tmp[--my_sbin[data_digit]] = pcl_in_elem_array[p_id];
				new_to_prev_pcl_map_tmp[my_sbin[data_digit]] = new_to_prev_pcl_map[p_id];
			}

			new_to_prev_pcl_map_tmp = new_to_prev_pcl_map;
			pcl_in_elem_array_tmp = pcl_in_elem_array;
			sort_var_id ^= 1;
			new_to_prev_pcl_map = new_to_prev_pcl_maps[sort_var_id];
			pcl_in_elem_array = pcl_in_elem_arrays[sort_var_id];
#pragma omp barrier
		}

		if (my_th_id == 0) // master thread
		{
			pcl_range[thread_num].id = next_pcl_num;
			radix_sort_var_id = sort_var_id;
			pcl_num = next_pcl_num;
		}
		else // non master thread
		{
			p_id = Block_Low(my_th_id, thread_num, next_pcl_num);
			if (p_id < next_pcl_num)
			{
				e_id = pcl_in_elem_array[p_id];
				while (e_id == pcl_in_elem_array[p_id + 1])
					++p_id;
				pcl_range[my_th_id].id = p_id + 1;
			}
			else
			{
				pcl_range[my_th_id].id = next_pcl_num;
			}
		}
	}

	return 0;
}

int Step_T2D_ME_mt::finalize_calculation()
{
	Model_T2D_ME_mt &md = *(Model_T2D_ME_mt *)model;
	md.pcl_num = pcl_num;
	return 0;
}

int substep_func_omp_T2D_ME_mt(
	void* _self,
	uint32_t my_th_id,
	float dt,
	float cur_time,
	uint32_t substp_id
	)
{
	typedef Model_T2D_ME_mt::PclBodyForce PclBodyForce;
	typedef Model_T2D_ME_mt::PclTraction PclTraction;
	typedef Model_T2D_ME_mt::PclPos PclPos;
	typedef Model_T2D_ME_mt::PclSortedVarArray PclSortedVarArray;
	typedef Model_T2D_ME_mt::PclDisp PclDisp;
	typedef Model_T2D_ME_mt::PclV PclV;
	typedef Model_T2D_ME_mt::PclShapeFunc PclShapeFunc;
	typedef Model_T2D_ME_mt::PclStress PclStress;
	typedef Model_T2D_ME_mt::ElemNodeIndex ElemNodeIndex;
	typedef Model_T2D_ME_mt::ElemShapeFuncAB ElemShapeFuncAB;
	typedef Model_T2D_ME_mt::ElemShapeFuncC ElemShapeFuncC;
	typedef Model_T2D_ME_mt::ElemStrainInc ElemStrainInc;
	typedef Model_T2D_ME_mt::ElemStress ElemStress;
	typedef Model_T2D_ME_mt::ElemNodeVM ElemNodeVM;
	typedef Model_T2D_ME_mt::ElemNodeForce ElemNodeForce;
	typedef Model_T2D_ME_mt::NodeA NodeA;
	typedef Model_T2D_ME_mt::NodeV NodeV;
	typedef Model_T2D_ME_mt::NodeHasVBC NodeHasVBC;
	typedef Model_T2D_ME_mt::NodePos NodePos;

	Step_T2D_ME_mt& self = *(Step_T2D_ME_mt*)(_self);
	Model_T2D_ME_mt& md = *(Model_T2D_ME_mt*)(self.model);

	PclSortedVarArray& pscv0 = self.pcl_sorted_var_array[self.pcl_sorted_var_id];
	uint32_t* pcl_index0 = pscv0.pcl_index;
	float *pcl_density0 = pscv0.pcl_density;
	PclDisp *pcl_disp0 = pscv0.pcl_disp;
	PclV *pcl_v0 = pscv0.pcl_v;
	PclShapeFunc* pcl_N0 = pscv0.pcl_N;
	PclStress* pcl_stress0 = pscv0.pcl_stress;

	PclSortedVarArray& pscv1 = self.pcl_sorted_var_array[self.pcl_sorted_var_id ^ 1];
	uint32_t* pcl_index1 = pscv1.pcl_index;
	float* pcl_density1 = pscv1.pcl_density;
	PclDisp* pcl_disp1 = pscv1.pcl_disp;
	PclV *pcl_v1 = pscv1.pcl_v;
	PclShapeFunc *pcl_N1 = pscv1.pcl_N;
	PclStress *pcl_stress1 = pscv1.pcl_stress;

	uint32_t sort_var_id = self.radix_sort_var_id;
	uint32_t* new_to_prev_pcl_map = self.new_to_prev_pcl_maps[sort_var_id];
	uint32_t* new_to_prev_pcl_map_tmp = self.new_to_prev_pcl_maps[sort_var_id ^ 1];
	uint32_t* pcl_in_elem_array = self.pcl_in_elem_arrays[sort_var_id];
	uint32_t* pcl_in_elem_array_tmp = self.pcl_in_elem_arrays[sort_var_id ^ 1];

	uint32_t p_id0 = self.pcl_range[my_th_id].id;
	uint32_t p_id1 = self.pcl_range[my_th_id + 1].id;
	uint32_t p_id, e_id;
	uint32_t prev_pcl_id, ori_pcl_id;
	float p_m, p_vol, p_N_m;
	float one_third_bfx, one_third_bfy;
	for (p_id = p_id0; p_id < p_id1; ++p_id)
	{
		prev_pcl_id = new_to_prev_pcl_map[p_id];

		ori_pcl_id = pcl_index1[prev_pcl_id];
		pcl_index0[p_id] = ori_pcl_id;

		e_id = pcl_in_elem_array[p_id];

		// map pcl mass
		p_m = self.pcl_m[ori_pcl_id];
		self.elem_pcl_m[e_id] += p_m;

		// map pcl volume
		p_vol = p_m / pcl_density1[prev_pcl_id];
		self.elem_pcl_vol[e_id] += p_vol;

		// map stress
		PclStress& p_s1 = pcl_stress1[prev_pcl_id];
		PclStress& p_s0 = pcl_stress0[p_id];
		p_s0.s11 = p_s1.s11;
		p_s0.s22 = p_s1.s22;
		p_s0.s12 = p_s1.s12;
		ElemStress & e_s = self.elem_stress[e_id];
		e_s.s11 += p_s0.s11 * p_vol;
		e_s.s22 += p_s0.s22 * p_vol;
		e_s.s12 += p_s0.s12 * p_vol;

		// map velocity
		PclShapeFunc& p_N1 = pcl_N1[prev_pcl_id];
		PclShapeFunc& p_N0 = pcl_N0[p_id];
		p_N0.N1 = p_N1.N1;
		p_N0.N2 = p_N1.N2;
		p_N0.N3 = p_N1.N3;
		PclV& p_v1 = pcl_v1[prev_pcl_id];
		PclV& p_v0 = pcl_v0[p_id];
		p_v0.vx = p_v1.vx;
		p_v0.vy = p_v1.vy;
		ElemNodeVM& en_vm1 = self.elem_node_vm[e_id * 3];
		p_N_m = p_N0.N1 * p_m;
		en_vm1.vm += p_N_m;
		en_vm1.vmx += p_N_m * p_v0.vx;
		en_vm1.vmy += p_N_m * p_v0.vy;
		ElemNodeVM& en_vm2 = self.elem_node_vm[e_id * 3 + 1];
		p_N_m = p_N0.N2 * p_m;
		en_vm2.vm += p_N_m;
		en_vm2.vmx += p_N_m * p_v0.vx;
		en_vm2.vmy += p_N_m * p_v0.vy;
		ElemNodeVM& en_vm3 = self.elem_node_vm[e_id * 3 + 2];
		p_N_m = p_N0.N3 * p_m;
		en_vm3.vm += p_N_m;
		en_vm3.vmx += p_N_m * p_v0.vx;
		en_vm3.vmy += p_N_m * p_v0.vy;

		// External load
		PclBodyForce& p_bf = self.pcl_bf[ori_pcl_id];
		one_third_bfx = one_third * p_bf.bfx;
		one_third_bfy = one_third * p_bf.bfy;
		PclTraction& p_t = self.pcl_t[ori_pcl_id];
		ElemNodeForce& en_f1 = self.elem_node_force[e_id * 3];
		en_f1.fx += one_third_bfx + p_N0.N1 * p_t.tx;
		en_f1.fy += one_third_bfy + p_N0.N1 * p_t.ty;
		ElemNodeForce& en_f2 = self.elem_node_force[e_id * 3 + 1];
		en_f2.fx += one_third_bfx + p_N0.N2 * p_t.tx;
		en_f2.fy += one_third_bfy + p_N0.N2 * p_t.ty;
		ElemNodeForce& en_f3 = self.elem_node_force[e_id * 3 + 2];
		en_f3.fx += one_third_bfx + p_N0.N3 * p_t.tx;
		en_f3.fy += one_third_bfy + p_N0.N3 * p_t.ty;
	}
#pragma omp barrier

	uint32_t e_id0 = self.elem_range[my_th_id];
	uint32_t e_id1 = self.elem_range[my_th_id + 1];
	float e_pcl_vol;
	for (e_id = e_id0; e_id < e_id1; ++e_id)
	{
		if (self.elem_pcl_vol[e_id] != 0.0f)
		{
			e_pcl_vol = self.elem_pcl_vol[e_id];
			self.elem_density[e_id] = self.elem_pcl_m[e_id] / e_pcl_vol;

			ElemStress& e_s = self.elem_stress[e_id];
			e_s.s11 /= e_pcl_vol;
			e_s.s22 /= e_pcl_vol;
			e_s.s12 /= e_pcl_vol;
			if (e_pcl_vol > self.elem_area[e_id])
				e_pcl_vol = self.elem_area[e_id];
			ElemShapeFuncAB& e_sf = self.elem_sf_ab[e_id];
			ElemNodeForce& en_f1 = self.elem_node_force[e_id * 3];
			en_f1.fx -= (e_sf.dN1_dx * e_s.s11 + e_sf.dN1_dy * e_s.s12) * e_pcl_vol;
			en_f1.fy -= (e_sf.dN1_dx * e_s.s12 + e_sf.dN1_dy * e_s.s22) * e_pcl_vol;
			ElemNodeForce& en_f2 = self.elem_node_force[e_id * 3 + 1];
			en_f2.fx -= (e_sf.dN2_dx * e_s.s11 + e_sf.dN2_dy * e_s.s12) * e_pcl_vol;
			en_f2.fy -= (e_sf.dN2_dx * e_s.s12 + e_sf.dN2_dy * e_s.s22) * e_pcl_vol;
			ElemNodeForce& en_f3 = self.elem_node_force[e_id * 3 + 2];
			en_f3.fx -= (e_sf.dN3_dx * e_s.s11 + e_sf.dN3_dy * e_s.s12) * e_pcl_vol;
			en_f3.fy -= (e_sf.dN3_dx * e_s.s12 + e_sf.dN3_dy * e_s.s22) * e_pcl_vol;
		}
	}
#pragma omp barrier

	// update node variables
	uint32_t n_id0 = self.node_range[my_th_id];
	uint32_t n_id1 = self.node_range[my_th_id + 1];
	uint32_t ne_id = self.node_elem_range[my_th_id];
	uint32_t n_id, ne_id1, node_var_id;
	for (n_id = n_id0; n_id < n_id1; ++n_id)
	{
		float n_am = 0.0f;
		float n_fx = 0.0f;
		float n_fy = 0.0f;
		float n_vm = 0.0f;
		float n_vmx = 0.0f;
		float n_vmy = 0.0f;
		ne_id1 = self.node_elem_list[n_id];
		for (; ne_id < ne_id1; ++ne_id)
		{
			n_am += self.elem_pcl_m[self.elem_id_array[ne_id]];
			node_var_id = self.node_elem_id_array[ne_id];
			ElemNodeForce& nf = self.elem_node_force[node_var_id];
			n_fx += nf.fx;
			n_fy += nf.fy;
			ElemNodeVM& nvm = self.elem_node_vm[node_var_id];
			n_vm += nvm.vm;
			n_vmx += nvm.vmx;
			n_vmy += nvm.vmy;
		}
		NodeA &node_a = self.node_a[n_id];
		if (n_am != 0.0f)
		{
			n_am *= one_third;
			self.node_am[n_id] = n_am;
			node_a.ax = n_fx / n_am;
			node_a.ay = n_fy / n_am;
		}
		if (n_vm != 0.0f)
		{
			NodeV& node_v = self.node_v[n_id];
			node_v.vx = n_vmx / n_vm + node_a.ax * dt;
			node_v.vy = n_vmy / n_vm + node_a.ay * dt;
			NodeHasVBC& node_has_vbc = self.node_has_vbc[n_id];
			node_v.vx_ui &= uint32_t(node_has_vbc.has_vx_bc) + UINT32_MAX;
			node_v.vy_ui &= uint32_t(node_has_vbc.has_vy_bc) + UINT32_MAX;
		}
	}
#pragma omp barrier

	// cal element strain and strain enhancement
	float e_de_vol;
	for (e_id = e_id0; e_id < e_id1; ++e_id)
	{
		if (self.elem_pcl_vol[e_id] != 0.0f)
		{
			ElemNodeIndex& e_n_id = self.elem_node_id[e_id];
			ElemShapeFuncAB& e_sf = self.elem_sf_ab[e_id];
			ElemStrainInc& e_de = self.elem_de[e_id];
			NodeV& n_v1 = self.node_v[e_n_id.n1];
			NodeV& n_v2 = self.node_v[e_n_id.n2];
			NodeV& n_v3 = self.node_v[e_n_id.n3];
			e_de.de11 = (e_sf.dN1_dx * n_v1.vx + e_sf.dN2_dx * n_v2.vx + e_sf.dN3_dx * n_v3.vx) * dt;
			e_de.de22 = (e_sf.dN1_dy * n_v1.vy + e_sf.dN2_dy * n_v2.vy + e_sf.dN3_dy * n_v3.vy) * dt;
			e_de.de12 = (e_sf.dN1_dx * n_v1.vy + e_sf.dN2_dx * n_v2.vy + e_sf.dN3_dx * n_v3.vy
					   + e_sf.dN1_dy * n_v1.vx + e_sf.dN2_dy * n_v2.vx + e_sf.dN3_dy * n_v3.vx) * dt * 0.5f;
			e_de_vol = e_de.de11 + e_de.de22;
			self.elem_m_de_vol[e_id] = self.elem_pcl_m[e_id] * e_de_vol;
			e_de_vol *= one_third;
			e_de.de11 -= e_de_vol;
			e_de.de22 -= e_de_vol;
		}
	}
#pragma omp barrier

	float n_am_de_vol;
	ne_id = self.node_elem_range[my_th_id];
	for (n_id = n_id0; n_id < n_id1; ++n_id)
	{
		if (self.node_am[n_id] != 0.0f)
		{
			ne_id1 = self.node_elem_list[n_id];
			n_am_de_vol = 0.0f;
			for (; ne_id < ne_id1; ++ne_id)
				n_am_de_vol += self.elem_m_de_vol[self.elem_id_array[ne_id]];
			self.node_de_vol[n_id] = n_am_de_vol * one_third / self.node_am[n_id];
		}
		else
		{
			ne_id = self.node_elem_list[n_id];
			self.node_de_vol[n_id] = 0.0f;
		}
	}
#pragma omp barrier

	for (e_id = e_id0; e_id < e_id1; ++e_id)
	{
		if (self.elem_pcl_vol[e_id] != 0.0f)
		{
			ElemNodeIndex& e_n_id = self.elem_node_id[e_id];
			e_de_vol = one_third * 
				 (self.node_de_vol[e_n_id.n1]
				+ self.node_de_vol[e_n_id.n2]
				+ self.node_de_vol[e_n_id.n3]);
			self.elem_density[e_id] /= (1.0f + e_de_vol);
			e_de_vol *= one_third;
			ElemStrainInc& e_de = self.elem_de[e_id];
			e_de.de11 += e_de_vol;
			e_de.de22 += e_de_vol;
		}
	}

#pragma omp master
	{
		self.new_pcl_num = 0;
	}
#pragma omp barrier

	// update particle variables
	float pcl_x, pcl_y;
	double dstrain[6] = { 0.0 };
	uint32_t pcl_in_mesh_num = 0;
	for (p_id = p_id0; p_id < p_id1; ++p_id)
	{
		e_id = pcl_in_elem_array[p_id];

		// update density
		pcl_density0[p_id] = self.elem_density[e_id];

		// update stress
		ori_pcl_id = pcl_index0[p_id];
		ElemStrainInc& e_de = self.elem_de[e_id];
		dstrain[0] = double(e_de.de11);
		dstrain[1] = double(e_de.de22);
		dstrain[3] = double(e_de.de12);
		MatModel::MaterialModel &pcl_mm = *self.pcl_mat_model[ori_pcl_id];
		int32_t mm_res = pcl_mm.integrate(dstrain);
		const double *dstress = pcl_mm.get_dstress();
		PclStress& p_s0 = pcl_stress0[p_id];
		p_s0.s11 += float(dstress[0]);
		p_s0.s22 += float(dstress[1]);
		p_s0.s12 += float(dstress[3]);

		ElemNodeIndex& e_n_id = self.elem_node_id[e_id];
		PclShapeFunc& p_N = pcl_N0[p_id];
		
		// update velocity
		NodeA& n_a1 = self.node_a[e_n_id.n1];
		NodeA& n_a2 = self.node_a[e_n_id.n2];
		NodeA& n_a3 = self.node_a[e_n_id.n3];
		PclV& p_v0 = pcl_v0[p_id];
		p_v0.vx += (p_N.N1 * n_a1.ax + p_N.N2 * n_a2.ax + p_N.N3 * n_a3.ax) * dt;
		p_v0.vy += (p_N.N1 * n_a1.ay + p_N.N2 * n_a2.ay + p_N.N3 * n_a3.ay) * dt;
		
		// update displacement
		NodeV& n_v1 = self.node_v[e_n_id.n1];
		NodeV& n_v2 = self.node_v[e_n_id.n2];
		NodeV& n_v3 = self.node_v[e_n_id.n3];
		PclDisp& p_d1 = pcl_disp1[new_to_prev_pcl_map[p_id]];
		PclDisp& p_d0 = pcl_disp0[p_id];
		p_d0.ux = p_d1.ux + (p_N.N1 * n_v1.vx + p_N.N2 * n_v2.vx + p_N.N3 * n_v3.vx) * dt;
		p_d0.uy = p_d1.uy + (p_N.N1 * n_v1.vy + p_N.N2 * n_v2.vy + p_N.N3 * n_v3.vy) * dt;
		
		// update location (in which element)
		PclPos& p_p = self.pcl_pos[ori_pcl_id];
		pcl_x = p_p.x + p_d0.ux;
		pcl_y = p_p.y + p_d0.uy;
		if (!md.is_in_element(pcl_x, pcl_y, e_id, p_N))
			e_id = md.find_pcl_in_which_elem(pcl_x, pcl_y, p_N);
		if (e_id != self.elem_num)
		{
			if (p_N.N1 < N_min)
				p_N.N1 = N_min;
			if (p_N.N2 < N_min)
				p_N.N2 = N_min;
			if (p_N.N3 < N_min)
				p_N.N3 = N_min;
			++pcl_in_mesh_num;
		}
		new_to_prev_pcl_map[p_id] = p_id;
		pcl_in_elem_array[p_id] = e_id;
	}

#pragma omp critical
	{
		self.new_pcl_num += pcl_in_mesh_num;
	}
#pragma omp barrier

	// reset element variables
	e_id0 = self.elem_range[my_th_id];
	e_id1 = self.elem_range[my_th_id + 1];
	memset(self.elem_pcl_m + e_id0, 0, (e_id1 - e_id0) * sizeof(float));
	memset(self.elem_pcl_vol + e_id0, 0, (e_id1 - e_id0) * sizeof(float));
	memset(self.elem_stress + e_id0, 0, (e_id1 - e_id0) * sizeof(ElemStress));
	memset(self.elem_node_vm + e_id0 * 3, 0, (e_id1 - e_id0) * 3 * sizeof(ElemNodeVM));
	memset(self.elem_node_force + e_id0 * 3, 0, (e_id1 - e_id0) * 3 * sizeof(ElemNodeForce));
	
	// sort particle variables
	uint32_t* my_cbin = self.elem_count_bin + size_t(my_th_id) * 0x100;
	uint32_t* my_sbin = self.elem_sum_bin + size_t(my_th_id) * 0x100;
	uint32_t data_digit, bin_id, th_id;
	uint32_t* other_cbin;
	for (uint32_t digit_disp = 0, elem_num_tmp = self.elem_num;
		 elem_num_tmp; digit_disp += 8, elem_num_tmp >>= 8)
	{
		memset(my_cbin, 0, 0x100 * sizeof(uint32_t));

		for (p_id = p_id0; p_id < p_id1; ++p_id)
		{
			data_digit = (pcl_in_elem_array[p_id] >> digit_disp) & 0xFF;
			++my_cbin[data_digit];
		}

		my_sbin[0] = my_cbin[0];
		for (bin_id = 1; bin_id < 0x100; ++bin_id)
		{
			my_cbin[bin_id] += my_cbin[bin_id - 1];
			my_sbin[bin_id] = my_cbin[bin_id];
		}
#pragma omp barrier

		for (th_id = 0; th_id < my_th_id; ++th_id)
		{
			other_cbin = self.elem_count_bin + size_t(th_id) * 0x100;
			for (bin_id = 0; bin_id < 0x100; ++bin_id)
				my_sbin[bin_id] += other_cbin[bin_id];
		}
		for (th_id = my_th_id + 1; th_id < self.thread_num; ++th_id)
		{
			other_cbin = self.elem_count_bin + size_t(th_id) * 0x100;
			for (bin_id = 1; bin_id < 0x100; ++bin_id)
				my_sbin[bin_id] += other_cbin[bin_id - 1];
		}

		for (p_id = p_id1; p_id-- > p_id0;)
		{
			data_digit = (pcl_in_elem_array[p_id] >> digit_disp) & 0xFF;
			pcl_in_elem_array_tmp[--my_sbin[data_digit]] = pcl_in_elem_array[p_id];
			new_to_prev_pcl_map_tmp[my_sbin[data_digit]] = new_to_prev_pcl_map[p_id];
		}

		new_to_prev_pcl_map_tmp = new_to_prev_pcl_map;
		pcl_in_elem_array_tmp = pcl_in_elem_array;
		sort_var_id ^= 1;
		new_to_prev_pcl_map = self.new_to_prev_pcl_maps[sort_var_id];
		pcl_in_elem_array = self.pcl_in_elem_arrays[sort_var_id];
#pragma omp barrier
	}
	
	if (my_th_id == 0)
	{
		self.pcl_range[self.thread_num].id = self.new_pcl_num;
		self.radix_sort_var_id = sort_var_id;
		self.pcl_sorted_var_id ^= 1;
		self.pcl_num = self.new_pcl_num;
		
		if (self.new_pcl_num)
			self.continue_calculation();
		else
			self.exit_calculation();
	}
	else
	{
		p_id = Block_Low(my_th_id, self.thread_num, self.new_pcl_num);
		if (p_id < self.new_pcl_num)
		{
			e_id = pcl_in_elem_array[p_id];
			while (e_id == pcl_in_elem_array[p_id + 1])
				++p_id;
			self.pcl_range[my_th_id].id = p_id + 1;
		}
		else
		{
			self.pcl_range[my_th_id].id = self.new_pcl_num;
		}
	}

#pragma omp barrier
	return 0;
}
