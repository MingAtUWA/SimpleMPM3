#include "Simulations_pcp.h"

#include <cmath>
#include "Step_T2D_ME_s_Geo.h"

Step_T2D_ME_s_Geo::Step_T2D_ME_s_Geo(const char* _name) :
	Step(_name, "Step_T2D_ME_s_Geo", &solve_substep_T2D_ME_s_Geo_avg),
	model(nullptr), damping_ratio(0.0),
	f_ub_ratio_bound(0.0), e_kin_ratio_bound(0.0),
	node_has_a_or_v_bc(nullptr)
{

}

Step_T2D_ME_s_Geo::~Step_T2D_ME_s_Geo() {}

int Step_T2D_ME_s_Geo::init_calculation()
{
	Model_T2D_ME_s &md = *model;

	if (is_first_step) {}

	node_has_a_or_v_bc = new bool[md.node_num];
	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		Node& n = md.nodes[n_id];
		n.has_mp = false;
		node_has_a_or_v_bc[n_id] = false;
	}

	for (size_t e_id = 0; e_id < md.elem_num; ++e_id)
	{
		Element& e = md.elems[e_id];
		e.has_pcl = false;
		e.pcls = nullptr;
	}
	
	for (size_t pcl_id = 0; pcl_id < md.pcl_num; ++pcl_id)
	{
		Particle &pcl = md.pcls[pcl_id];
		pcl.pe = const_cast<Element *>(md.find_in_which_element(pcl));
		if (pcl.pe)
		{
			Element &e = *pcl.pe;
			e.add_pcl(pcl);
			e.has_pcl = true;
			Node& n1 = md.nodes[e.n1];
			n1.has_mp = true;
			Node& n2 = md.nodes[e.n2];
			n2.has_mp = true;
			Node& n3 = md.nodes[e.n3];
			n3.has_mp = true;
			pcl.vol = pcl.m / pcl.density;
		}
	}

	for (size_t a_id = 0; a_id < md.ax_num; ++a_id)
		node_has_a_or_v_bc[md.axs[a_id].node_id] = true;
	for (size_t a_id = 0; a_id < md.ay_num; ++a_id)
		node_has_a_or_v_bc[md.ays[a_id].node_id] = true;
	for (size_t v_id = 0; v_id < md.vx_num; ++v_id)
		node_has_a_or_v_bc[md.vxs[v_id].node_id] = true;
	for (size_t v_id = 0; v_id < md.vy_num; ++v_id)
		node_has_a_or_v_bc[md.vys[v_id].node_id] = true;

	// convergence criteria
	// unbalanced force
	f_ub = 0.0;
	max_f_ub = 0.0;
	init_f_ub = 0.0;
	f_ub_ratio = 1.0;
	// kinematic energy
	e_kin = 0.0;
	e_kin_max = 0.0;
	e_kin_prev = 0.0;
	e_kin_ratio = 1.0;

	return 0;
}

int Step_T2D_ME_s_Geo::finalize_calculation()
{
	Model_T2D_ME_s &md = *model;

	for (size_t pcl_id = 0; pcl_id < md.pcl_num; ++pcl_id)
	{
		Particle &pcl = md.pcls[pcl_id];
		pcl.vx = 0.0;
		pcl.vy = 0.0;
	}

	delete[] node_has_a_or_v_bc;
	node_has_a_or_v_bc = nullptr;

	return 0;
}

int solve_substep_T2D_ME_s_Geo(void *_self)
{
	typedef Model_T2D_ME_s::Node Node;
	typedef Model_T2D_ME_s::Element Element;
	typedef Model_T2D_ME_s::Particle Particle;
	Step_T2D_ME_s_Geo &self = *(Step_T2D_ME_s_Geo *)(_self);
	Model_T2D_ME_s &md = *self.model;

	// init nodes
	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		Node &n = md.nodes[n_id];
		n.m = 0.0;
		n.vx = 0.0;
		n.vy = 0.0;
		n.fx_ext = 0.0;
		n.fy_ext = 0.0;
		n.fx_int = 0.0;
		n.fy_int = 0.0;
		n.de_vol_by_3 = 0.0;
		n.se_pcl_vol = 0.0;
	}

	// init elements
	for (size_t e_id = 0; e_id < md.elem_num; ++e_id)
	{
		Element &e = md.elems[e_id];
		e.mi_pcl_vol = 0.0;
		e.s11 = 0.0;
		e.s22 = 0.0;
		e.s12 = 0.0;
	}

	// init particles
	for (size_t pcl_id = 0; pcl_id < md.pcl_num; ++pcl_id)
	{
		Particle &pcl = md.pcls[pcl_id];
		if (pcl.pe)
		{
			Element &e = *pcl.pe;
			e.s11 += pcl.vol * pcl.s11;
			e.s22 += pcl.vol * pcl.s22;
			e.s12 += pcl.vol * pcl.s12;
			e.mi_pcl_vol += pcl.vol;

			double mvx = pcl.m * pcl.vx;
			double mvy = pcl.m * pcl.vy;
			// node 1
			Node &n1 = md.nodes[e.n1];
			n1.m  += pcl.N1 * pcl.m;
			n1.vx += pcl.N1 * mvx;
			n1.vy += pcl.N1 * mvy;
			// node 2
			Node &n2 = md.nodes[e.n2];
			n2.m  += pcl.N2 * pcl.m;
			n2.vx += pcl.N2 * mvx;
			n2.vy += pcl.N2 * mvy;
			// node 3
			Node &n3 = md.nodes[e.n3];
			n3.m  += pcl.N3 * pcl.m;
			n3.vx += pcl.N3 * mvx;
			n3.vy += pcl.N3 * mvy;
		}
	}

	for (size_t e_id = 0; e_id < md.elem_num; ++e_id)
	{
		Element &e = md.elems[e_id];
		if (e.pcls)
		{
			e.s11 /= e.mi_pcl_vol;
			e.s22 /= e.mi_pcl_vol;
			e.s12 /= e.mi_pcl_vol;
			if (e.mi_pcl_vol > e.area)
				e.mi_pcl_vol = e.area;

			Node &n1 = md.nodes[e.n1];
			Node &n2 = md.nodes[e.n2];
			Node &n3 = md.nodes[e.n3];
			// node 1
			n1.fx_int += (e.dN1_dx * e.s11 + e.dN1_dy * e.s12) * e.mi_pcl_vol;
			n1.fy_int += (e.dN1_dx * e.s12 + e.dN1_dy * e.s22) * e.mi_pcl_vol;
			// node 2
			n2.fx_int += (e.dN2_dx * e.s11 + e.dN2_dy * e.s12) * e.mi_pcl_vol;
			n2.fy_int += (e.dN2_dx * e.s12 + e.dN2_dy * e.s22) * e.mi_pcl_vol;
			// node 3
			n3.fx_int += (e.dN3_dx * e.s11 + e.dN3_dy * e.s12) * e.mi_pcl_vol;
			n3.fy_int += (e.dN3_dx * e.s12 + e.dN3_dy * e.s22) * e.mi_pcl_vol;
		}
	}

	// body force
	double bf_mag;
	for (size_t bf_id = 0; bf_id < md.bfx_num; ++bf_id)
	{
		BodyForceAtPcl &bf = md.bfxs[bf_id];
		Particle &pcl = md.pcls[bf.pcl_id];
		if (pcl.pe)
		{
			Element &e = *pcl.pe;
			bf_mag = pcl.m * bf.bf;
			// node 1
			Node &n1 = md.nodes[e.n1];
			n1.fx_ext += pcl.N1 * bf_mag;
			// node 2
			Node &n2 = md.nodes[e.n2];
			n2.fx_ext += pcl.N2 * bf_mag;
			// node 3
			Node &n3 = md.nodes[e.n3];
			n3.fx_ext += pcl.N3 * bf_mag;
		}
	}
	for (size_t bf_id = 0; bf_id < md.bfy_num; ++bf_id)
	{
		BodyForceAtPcl &bf = md.bfys[bf_id];
		Particle &pcl = md.pcls[bf.pcl_id];
		if (pcl.pe)
		{
			Element &e = *pcl.pe;
			bf_mag = pcl.m * bf.bf;
			// node 1
			Node &n1 = md.nodes[e.n1];
			n1.fy_ext += pcl.N1 * bf_mag;
			// node 2
			Node &n2 = md.nodes[e.n2];
			n2.fy_ext += pcl.N2 * bf_mag;
			// node 3
			Node &n3 = md.nodes[e.n3];
			n3.fy_ext += pcl.N3 * bf_mag;
		}
	}

	// surface force
	for (size_t tf_id = 0; tf_id < md.tx_num; ++tf_id)
	{
		TractionBCAtPcl &tf = md.txs[tf_id];
		Particle &pcl = md.pcls[tf.pcl_id];
		if (pcl.pe)
		{
			Element &e = *pcl.pe;
			// node 1
			Node &n1 = md.nodes[e.n1];
			n1.fx_ext += pcl.N1 * tf.t;
			// node 2
			Node &n2 = md.nodes[e.n2];
			n2.fx_ext += pcl.N2 * tf.t;
			// node 3
			Node &n3 = md.nodes[e.n3];
			n3.fx_ext += pcl.N3 * tf.t;
		}
	}
	for (size_t tf_id = 0; tf_id < md.ty_num; ++tf_id)
	{
		TractionBCAtPcl &tf = md.tys[tf_id];
		Particle &pcl = md.pcls[tf.pcl_id];
		if (pcl.pe)
		{
			Element &e = *pcl.pe;
			// node 1
			Node &n1 = md.nodes[e.n1];
			n1.fy_ext += pcl.N1 * tf.t;
			// node 2
			Node &n2 = md.nodes[e.n2];
			n2.fy_ext += pcl.N2 * tf.t;
			// node 3
			Node &n3 = md.nodes[e.n3];
			n3.fy_ext += pcl.N3 * tf.t;
		}
	}

	// update nodal acceleration of fluid pahse
	double nf;
	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		Node &n = md.nodes[n_id];
		if (n.has_mp)
		{
			nf = n.fx_ext - n.fx_int;
			n.ax = (nf - self.damping_ratio * abs(nf) * get_sign(n.vx)) / n.m;
			nf = n.fy_ext - n.fy_int;
			n.ay = (nf - self.damping_ratio * abs(nf) * get_sign(n.vy)) / n.m;
		}
	}

	// apply acceleration bc
	for (size_t a_id = 0; a_id < md.ax_num; ++a_id)
	{
		Node &n = md.nodes[md.axs[a_id].node_id];
		n.ax = md.axs[a_id].a;
	}
	for (size_t a_id = 0; a_id < md.ay_num; ++a_id)
	{
		Node &n = md.nodes[md.ays[a_id].node_id];
		n.ay = md.ays[a_id].a;
	}

	// update nodal momentum
	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		Node &n = md.nodes[n_id];
		if (n.has_mp)
		{
			n.vx /= n.m;
			n.vx += n.ax * self.dtime;
			n.vy /= n.m;
			n.vy += n.ay * self.dtime;
		}
	}

	// apply velocity bc
	for (size_t v_id = 0; v_id < md.vx_num; ++v_id)
	{
		Node &n = md.nodes[md.vxs[v_id].node_id];
		n.ax = 0.0;
		n.vx = md.vxs[v_id].v;
	}
	for (size_t v_id = 0; v_id < md.vy_num; ++v_id)
	{
		Node &n = md.nodes[md.vys[v_id].node_id];
		n.ay = 0.0;
		n.vy = md.vys[v_id].v;
	}

	// cal unbalanced nodal force and kinetic energy
	double nfx_ub, nfy_ub, nf_ub;
	self.max_f_ub = 0.0;
	self.f_ub = 0.0;
	self.e_kin = 0.0;
	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		Node &n = md.nodes[n_id];
		if (n.has_mp)
		{
			if (!self.node_has_a_or_v_bc[n_id])
			{
				nfx_ub = n.fx_ext - n.fx_int;
				nfy_ub = n.fy_ext - n.fy_int;
				nf_ub = nfx_ub * nfx_ub + nfy_ub * nfy_ub;
				self.f_ub += nf_ub;
				nf_ub = sqrt(nf_ub);
				if (self.max_f_ub < nf_ub)
					self.max_f_ub = nf_ub;
			}
			self.e_kin += n.m * (n.vx * n.vx + n.vy * n.vy);
		}
	}

	if (self.substep_index == 0) // first step
	{
		self.init_f_ub = self.f_ub;
		self.e_kin_max = self.e_kin;
	}
	else // not first step
	{
		if (self.e_kin_max < self.e_kin)
			self.e_kin_max = self.e_kin;
	}

	self.f_ub_ratio = sqrt(self.f_ub / self.init_f_ub);
	self.e_kin_ratio = sqrt(self.e_kin_prev / self.e_kin_max);

	// kinetic damping
	if (self.e_kin < self.e_kin_prev)
	{
		// reset pcl velocity
		for (size_t p_id = 0; p_id < md.pcl_num; ++p_id)
		{
			Particle &pcl = md.pcls[p_id];
			pcl.vx = 0.0;
			pcl.vy = 0.0;
		}
		self.e_kin_prev = 0.0;

		//if (self.e_kin_ratio < self.e_kin_ratio_bound &&
		//	self.f_ub_ratio < self.f_ub_ratio_bound)
		//	return 2;
		return 1;
	}
	
	self.e_kin_prev = self.e_kin;
	
	// update displacement increment of both phases
	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		Node &n = md.nodes[n_id];
		if (n.has_mp)
		{
			n.dux = n.vx * self.dtime;
			n.duy = n.vy * self.dtime;
		}
	}

	// map variables back to particles and update their variables
	double de11, de22, de12, de_vol_by_3;
	for (size_t e_id = 0; e_id < md.elem_num; ++e_id)
	{
		Element& e = md.elems[e_id];
		if (e.pcls)
		{
			Node& n1 = md.nodes[e.n1];
			Node& n2 = md.nodes[e.n2];
			Node& n3 = md.nodes[e.n3];

			// strain increment
			de11 = n1.dux * e.dN1_dx + n2.dux * e.dN2_dx + n3.dux * e.dN3_dx;
			de22 = n1.duy * e.dN1_dy + n2.duy * e.dN2_dy + n3.duy * e.dN3_dy;
			de12 = (n1.dux * e.dN1_dy + n2.dux * e.dN2_dy + n3.dux * e.dN3_dy
				  + n1.duy * e.dN1_dx + n2.duy * e.dN2_dx + n3.duy * e.dN3_dx) * 0.5;
			de_vol_by_3 = (de11 + de22) / 3.0;
			e.dde11 = de11 - de_vol_by_3;
			e.dde22 = de22 - de_vol_by_3;
			e.de12 = de12;
			e.de_vol_by_3 = de_vol_by_3;
		}
	}

	for (size_t pcl_id = 0; pcl_id < md.pcl_num; ++pcl_id)
	{
		Particle& pcl = md.pcls[pcl_id];
		if (pcl.pe)
		{
			Element& e = *pcl.pe;
			double vol_de_vol_by_3 = pcl.vol * e.de_vol_by_3;
			// node 1
			Node& n1 = md.nodes[e.n1];
			n1.de_vol_by_3 += pcl.N1 * vol_de_vol_by_3;
			n1.se_pcl_vol += pcl.N1 * pcl.vol;
			// node 2
			Node& n2 = md.nodes[e.n2];
			n2.de_vol_by_3 += pcl.N2 * vol_de_vol_by_3;
			n2.se_pcl_vol += pcl.N2 * pcl.vol;
			// node 3
			Node& n3 = md.nodes[e.n3];
			n3.de_vol_by_3 += pcl.N3 * vol_de_vol_by_3;
			n3.se_pcl_vol += pcl.N3 * pcl.vol;
		}
	}
	
	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		Node& n = md.nodes[n_id];
		if (n.has_mp)
			n.de_vol_by_3 /= n.se_pcl_vol;
	}
	
	// map variables back to particles and update their variables
	double ds11, ds22, ds12;
	for (size_t pcl_id = 0; pcl_id < md.pcl_num; ++pcl_id)
	{
		Particle &pcl = md.pcls[pcl_id];
		if (pcl.pe)
		{
			Element &e = *pcl.pe;
			Node &n1 = md.nodes[e.n1];
			Node &n2 = md.nodes[e.n2];
			Node &n3 = md.nodes[e.n3];

			// velocity
			pcl.vx += (n1.ax * pcl.N1 + n2.ax * pcl.N2 + n3.ax * pcl.N3) * self.dtime;
			pcl.vy += (n1.ay * pcl.N1 + n2.ay * pcl.N2 + n3.ay * pcl.N3) * self.dtime;

			// strain
			de_vol_by_3 = (n1.de_vol_by_3 + n2.de_vol_by_3 + n3.de_vol_by_3) / 3.0;
			de11 = e.dde11 + de_vol_by_3;
			de22 = e.dde22 + de_vol_by_3;
			de12 = e.de12;

			// stress
			double dstrain[6] = { de11, de22, 0.0, de12, 0.0, 0.0 };
			pcl.mm->integrate(dstrain);
			const double *dstress = pcl.mm->get_dstress();
			pcl.s11 += dstress[0];
			pcl.s22 += dstress[1];
			pcl.s12 += dstress[3];
		}
	}
	
	return 0;
}
