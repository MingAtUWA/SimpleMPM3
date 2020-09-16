#include "Simulations_pcp.h"

#include <cmath>
#include "MaterialModel.h"
#include "Step_T2D_ME_s.h"

#define one_third (1.0/3.0)

int solve_substep_T2D_ME_s_avg(void *_self)
{
	typedef Model_T2D_ME_s::Element Element;
	typedef Model_T2D_ME_s::Node Node;
	typedef Model_T2D_ME_s::Particle Particle;

	Step_T2D_ME_s &self = *(Step_T2D_ME_s *)(_self);
	Model_T2D_ME_s &md = *self.model;

	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		Node &n = md.nodes[n_id];
		n.has_mp = false;
		n.am = 0.0;
		n.vm = 0.0;
		n.vx = 0.0;
		n.vy = 0.0;
		n.fx_ext = 0.0;
		n.fy_ext = 0.0;
		n.fx_int = 0.0;
		n.fy_int = 0.0;
		n.de_vol_by_3 = 0.0;
	}

	for (size_t e_id = 0; e_id < md.elem_num; ++e_id)
	{
		Element &e = md.elems[e_id];
		e.pcls = nullptr;
		e.mi_pcl_vol = 0.0;
		e.s11 = 0.0;
		e.s22 = 0.0;
		e.s12 = 0.0;
		e.pcl_m = 0.0;
	}

	for (size_t pcl_id = 0; pcl_id < md.pcl_num; ++pcl_id)
	{
		Particle &pcl = md.pcls[pcl_id];
		if (pcl.pe)
		{
			if (!(pcl.pe = md.find_in_which_element(pcl)))
				continue;
			pcl.pe->add_pcl(pcl);

			Element &e = *pcl.pe;
			pcl.vol = pcl.m / pcl.density;
			e.mi_pcl_vol += pcl.vol;
			e.s11 += pcl.s11 * pcl.vol;
			e.s22 += pcl.s22 * pcl.vol;
			e.s12 += pcl.s12 * pcl.vol;

			e.pcl_m += pcl.m;

			double mvx = pcl.m * pcl.vx;
			double mvy = pcl.m * pcl.vy;
			// node 1
			Node &n1 = md.nodes[e.n1];
			n1.has_mp = true;
			n1.vm += pcl.N1 * pcl.m;
			n1.vx += pcl.N1 * mvx;
			n1.vy += pcl.N1 * mvy;
			// node 2
			Node &n2 = md.nodes[e.n2];
			n2.has_mp = true;
			n2.vm += pcl.N2 * pcl.m;
			n2.vx += pcl.N2 * mvx;
			n2.vy += pcl.N2 * mvy;
			// node 3
			Node &n3 = md.nodes[e.n3];
			n3.has_mp = true;
			n3.vm += pcl.N3 * pcl.m;
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

			double n_pcl_m = one_third * e.pcl_m;
			n1.am += n_pcl_m;
			n2.am += n_pcl_m;
			n3.am += n_pcl_m;

			n1.fx_int += (e.dN1_dx * e.s11 + e.dN1_dy * e.s12) * e.mi_pcl_vol;
			n1.fy_int += (e.dN1_dx * e.s12 + e.dN1_dy * e.s22) * e.mi_pcl_vol;
			n2.fx_int += (e.dN2_dx * e.s11 + e.dN2_dy * e.s12) * e.mi_pcl_vol;
			n2.fy_int += (e.dN2_dx * e.s12 + e.dN2_dy * e.s22) * e.mi_pcl_vol;
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
			bf_mag = one_third * pcl.m * bf.bf;
			Node &n1 = md.nodes[e.n1];
			Node &n2 = md.nodes[e.n2];
			Node &n3 = md.nodes[e.n3];
			n1.fx_ext += bf_mag;
			n2.fx_ext += bf_mag;
			n3.fx_ext += bf_mag;
		}
	}
	for (size_t bf_id = 0; bf_id < md.bfy_num; ++bf_id)
	{
		BodyForceAtPcl &bf = md.bfys[bf_id];
		Particle &pcl = md.pcls[bf.pcl_id];
		if (pcl.pe)
		{
			Element &e = *pcl.pe;
			bf_mag = one_third * pcl.m * bf.bf;
			Node &n1 = md.nodes[e.n1];
			Node &n2 = md.nodes[e.n2];
			Node &n3 = md.nodes[e.n3];
			n1.fy_ext += bf_mag;
			n2.fy_ext += bf_mag;
			n3.fy_ext += bf_mag;
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
			Node &n1 = md.nodes[e.n1];
			Node& n2 = md.nodes[e.n2];
			Node& n3 = md.nodes[e.n3];
			n1.fx_ext += pcl.N1 * tf.t;
			n2.fx_ext += pcl.N2 * tf.t;
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
			Node &n1 = md.nodes[e.n1];
			Node &n2 = md.nodes[e.n2];
			Node &n3 = md.nodes[e.n3];
			n1.fy_ext += pcl.N1 * tf.t;
			n2.fy_ext += pcl.N2 * tf.t;
			n3.fy_ext += pcl.N3 * tf.t;
		}
	}

	// update node acceleration
	double nf;
	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		Node &n = md.nodes[n_id];
		if (n.has_mp)
		{
			nf = n.fx_ext - n.fx_int;
			n.ax = (nf - self.damping_ratio * abs(nf) * get_sign(n.vx)) / n.am;
			nf = n.fy_ext - n.fy_int;
			n.ay = (nf - self.damping_ratio * abs(nf) * get_sign(n.vy)) / n.am;
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

	// update nodal velocity
	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		Node &n = md.nodes[n_id];
		if (n.has_mp)
		{
			n.vx /= n.vm;
			n.vx += n.ax * self.dtime;
			n.vy /= n.vm;
			n.vy += n.ay * self.dtime;
		}
	}

	// contact with rigid circle
	if (md.rigid_circle_is_init)
		md.apply_rigid_circle(self.dtime);

	if (md.rigid_rect_is_init)
		md.apply_rigid_rect(self.dtime);

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

	double de11, de22, de12, de_vol_by_3;
	for (size_t e_id = 0; e_id < md.elem_num; ++e_id)
	{
		Element &e = md.elems[e_id];
		if (e.pcls)
		{
			Node &n1 = md.nodes[e.n1];
			Node &n2 = md.nodes[e.n2];
			Node &n3 = md.nodes[e.n3];
			
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

	// strain enhancement
	for (size_t e_id = 0; e_id < md.elem_num; ++e_id)
	{
		Element& e = md.elems[e_id];
		if (e.pcls)
		{
			double vol_de_vol_by_3 = one_third * e.de_vol_by_3 * e.pcl_m;
			Node &n1 = md.nodes[e.n1];
			Node& n2 = md.nodes[e.n2];
			Node& n3 = md.nodes[e.n3];
			n1.de_vol_by_3 += vol_de_vol_by_3;
			n2.de_vol_by_3 += vol_de_vol_by_3;
			n3.de_vol_by_3 += vol_de_vol_by_3;
		}
	}

	for (size_t n_id = 0; n_id < md.node_num; ++n_id)
	{
		Node &n = md.nodes[n_id];
		if (n.has_mp)
			n.de_vol_by_3 /= n.am;
	}

	for (size_t e_id = 0; e_id < md.elem_num; ++e_id)
	{
		Element& e = md.elems[e_id];
		Node& n1 = md.nodes[e.n1];
		Node& n2 = md.nodes[e.n2];
		Node& n3 = md.nodes[e.n3];
		if (e.pcls)
			e.de_vol_by_3 = (n1.de_vol_by_3 + n2.de_vol_by_3 + n3.de_vol_by_3) * one_third;
	}

	double ds11, ds22, ds12;
	for (size_t pcl_id = 0; pcl_id < md.pcl_num; ++pcl_id)
	{
		Particle &pcl = md.pcls[pcl_id];
		if (pcl.pe)
		{
			Element& e = *pcl.pe;
			Node &n1 = md.nodes[e.n1];
			Node &n2 = md.nodes[e.n2];
			Node &n3 = md.nodes[e.n3];

			// velocity
			pcl.vx += (n1.ax * pcl.N1 + n2.ax * pcl.N2 + n3.ax * pcl.N3) * self.dtime;
			pcl.vy += (n1.ay * pcl.N1 + n2.ay * pcl.N2 + n3.ay * pcl.N3) * self.dtime;

			// displacement
			pcl.ux += n1.dux * pcl.N1 + n2.dux * pcl.N2 + n3.dux * pcl.N3;
			pcl.uy += n1.duy * pcl.N1 + n2.duy * pcl.N2 + n3.duy * pcl.N3;

			// update position
			pcl.x = pcl.x_ori + pcl.ux;
			pcl.y = pcl.y_ori + pcl.uy;

			// strain
			de_vol_by_3 = e.de_vol_by_3;
			de11 = e.dde11 + de_vol_by_3;
			de22 = e.dde22 + de_vol_by_3;
			de12 = e.de12;
			pcl.e11 += de11;
			pcl.e22 += de22;
			pcl.e12 += de12;

			double dstrain[6] = { de11, de22, 0.0, de12, 0.0, 0.0 };
			pcl.mm->integrate(dstrain);
			const double *dstress = pcl.mm->get_dstress();
			pcl.s11 += dstress[0];
			pcl.s22 += dstress[1];
			pcl.s12 += dstress[3];

			pcl.density /= 1.0 + (de_vol_by_3 * 3.0);
		}
	}
	
	return 0;
}