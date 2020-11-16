#ifndef __Rigid_Cone_h__
#define __Rigid_Cone_h__

#include "Geometry3D.h"
#include "ContactForce3D.h"

class RigidCone
{
protected:
	double r, h_tip, h_shaft;

	union
	{
		Point3D centre;
		struct { double x, y, z; };
	};

	double vx, vy, vz;

	union
	{
		ContactForce3D cont_force;
		struct
		{
			double fx_cont, fy_cont, fz_cont;
			double mx_cont, my_cont, mz_cont;
		};
	};

	Cube lbbox;
	double ht_div_r, r_div_ht, r2_div_ht;
	double sqrt_one_ht2_div_r2;
	Vector3D res_norms[3];

public:
	explicit RigidCone();
	~RigidCone();

	const Point3D& get_centre() const noexcept { return centre; }
	const ContactForce3D& get_cont_force() const noexcept { return cont_force; }

	void init(double _x, double _y, double _z, double _r,
		double _tip_h, double _shaft_h) noexcept;
	
	void set_vbc(double _vx, double _vy, double _vz) noexcept;

	inline void reset_f_cont() noexcept
	{
		fx_cont = 0.0;
		fy_cont = 0.0;
		fz_cont = 0.0;
		mx_cont = 0.0;
		my_cont = 0.0;
		mz_cont = 0.0;
	}

	inline void combine_f_cont(const ContactForce3D& other) noexcept
	{
		fx_cont += other.fx;
		fy_cont += other.fy;
		fz_cont += other.fz;
		mx_cont += other.mx;
		my_cont += other.my;
		mz_cont += other.mz;
	}

	bool detect_collision_with_point(
		double p_x,	double p_y, double p_z, double p_vol,
		double& dist, Vector3D& lnorm, Point3D& lcontpos
		) noexcept;

	inline void update_motion(double dt) noexcept
	{
		x += vx * dt;
		y += vy * dt;
		z += vz * dt;
	}
};

#endif