#include "SimulationsOMP_pcp.h"

#include "RoughContact3D.h"

RoughContact3D::RoughContact3D() :
	Kn_cont(0.0), Kt_cont(0.0) {}

RoughContact3D::~RoughContact3D() {}

void RoughContact3D::cal_contact_force(
	size_t substp_id,
	double dist,
	const Vector3D& norm,
	const Point3D& cont_pos,
	double pcl_len,
	ParticleVariablesGetter& pv_getter,
	size_t& cont_substp_id,
	Point3D& prev_cont_pos,
	Vector3D& prev_cont_tan_force,
	Vector3D& cont_force)
{
	const double pcl_area = pcl_len * pcl_len;
	// normal force
	// allow overlapping of 0.1 pcl_len
	// reduce oscillation
	dist -= 0.1 * pcl_len;
	double f_cont = dist > 0.0 ? Kn_cont * pcl_area * dist : 0.0;
	cont_force.x = f_cont * norm.x;
	cont_force.y = f_cont * norm.y;
	cont_force.z = f_cont * norm.z;
	// tangential force
	if (cont_substp_id != substp_id)
	{
		if (dist > 0.0)
		{
			// not in contacct
			// record contact position
			prev_cont_pos.x = cont_pos.x;
			prev_cont_pos.y = cont_pos.y;
			prev_cont_pos.z = cont_pos.z;
			cont_substp_id = substp_id + 1;
		}
	}
	else
	{
		// previously in contactt
		// add tangential force to contact force
		const double K_p_area = Kt_cont * pcl_area;
		cont_force.x -= (cont_pos.x - prev_cont_pos.x) * K_p_area;
		cont_force.y -= (cont_pos.y - prev_cont_pos.y) * K_p_area;
		cont_force.z -= (cont_pos.z - prev_cont_pos.z) * K_p_area;
		cont_substp_id = substp_id + 1;
	}
}
