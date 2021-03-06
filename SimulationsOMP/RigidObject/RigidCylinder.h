#ifndef __Rigid_Cylinder_h__
#define __Rigid_Cylinder_h__

#include "Geometry3D.h"
#include "Force3D.h"

class RigidCylinder
{
protected:
	double h, r;
	
	union
	{
		struct { double x, y, z; };
		Point3D centre;
	};

	union
	{
		struct { double vx, vy, vz; };
		Vector3D velocity;
	};

	union
	{
		struct
		{
			double fx_cont, fy_cont, fz_cont;
			double mx_cont, my_cont, mz_cont;
		};
		Force3D cont_force;
	};
	
	double h_div_2, r2;
	Cube lbbox;
	
public:
	explicit RigidCylinder();
	~RigidCylinder();

	inline double get_h() const noexcept { return h; }
	inline double get_r() const noexcept { return r; }
	inline const Point3D &get_centre() const noexcept { return centre; }
	inline const Vector3D &get_velocity() const noexcept { return velocity; }
	inline const Force3D &get_cont_force() const noexcept { return cont_force; }
	Cube get_bbox() const noexcept
	{
		return Cube(lbbox.xl + x, lbbox.xu + x,
					lbbox.yl + y, lbbox.yu + y,
					lbbox.zl + z, lbbox.zu + z);
	}

	void init(double _x, double _y, double _z,
			  double _h, double _r) noexcept;
	void set_vbc(double _vx, double _vy, double _vz) noexcept;
	void set_cont_force(double fx, double fy, double fz,
						double mx, double my, double mz) noexcept;
	void set_cont_force(const Force3D &cf) noexcept;

	inline void reset_cont_force() noexcept
	{
		fx_cont = 0.0; fy_cont = 0.0; fz_cont = 0.0;
		mx_cont = 0.0; my_cont = 0.0; mz_cont = 0.0;
	}

	inline void update_motion(double dt) noexcept
	{
		x += vx * dt;
		y += vy * dt;
		z += vz * dt;
	}
	
	inline void get_global_point(
		const Point3D &lp,
		Point3D &gp
		) const noexcept
	{
		gp.x = lp.x + x;
		gp.y = lp.y + y;
		gp.z = lp.z + z;
	}

	inline void get_local_point(
		const Point3D& gp,
		Point3D& lp
		) const noexcept
	{
		lp.x = gp.x - x;
		lp.y = gp.y - y;
		lp.z = gp.z - z;
	}

	inline void get_global_vector(
		const Vector3D &lv,
		Vector3D &gv
		) const noexcept
	{
		gv.x = lv.x;
		gv.y = lv.y;
		gv.z = lv.z;
	}

	inline void get_local_vector(
		const Vector3D& gv,
		Vector3D& lv
		) const noexcept
	{
		lv.x = gv.x;
		lv.y = gv.y;
		lv.z = gv.z;
	}

	bool detect_collision_with_point(
		double p_x,	double p_y,	double p_z, double p_r,
		double &dist, Vector3D &lnorm, Point3D &lcontpos
		) const noexcept;
};

#endif