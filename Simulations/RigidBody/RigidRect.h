#ifndef __Rigid_Rect_h__
#define __Rigid_Rect_h__

#include <cmath>

#include "Geometry2D.h"

class RigidRect;

struct RigidRectForce
{
protected:
	friend class RigidRect;
	double fx, fy, m;

public:
	inline void reset_rf() noexcept { fx = 0.0; fy = 0.0; m = 0.0; }
	inline void add_rf(
		double _x,   double _y,
		double _fx,  double _fy,
		double cen_x, double cen_y
		) noexcept
	{
		fx += _fx; fy += _fy;
		m += (_x - cen_x) * _fy - (_y - cen_y) * _fx;
	}
	inline void combine(const RigidRectForce &other) noexcept
	{
		fx += other.fx;
		fy += other.fy;
		m += other.m;
	}
};

// for t-bar penetration and pipe embedment
class RigidRect
{
protected:
	double hx, hy;
	double density;

	union { double ax; unsigned long long ax_ull; };
	union { double ay; unsigned long long ay_ull; };
	union { double a_ang; unsigned long long a_ang_ull; };
	union { double vx; unsigned long long vx_ull; };
	union { double vy; unsigned long long vy_ull; };
	union { double v_ang; unsigned long long v_ang_ull; };
	double x, y, ang; // position and angle

	union { double ax_bc; unsigned long long ax_bc_ull; };
	union { double ay_bc; unsigned long long ay_bc_ull; };
	union { double a_ang_bc; unsigned long long a_ang_bc_ull; };
	unsigned long long ax_bc_mask, ay_bc_mask, a_ang_bc_mask;
	union { double vx_bc; unsigned long long vx_bc_ull; };
	union { double vy_bc; unsigned long long vy_bc_ull; };
	union { double v_ang_bc; unsigned long long v_ang_bc_ull; };
	unsigned long long vx_bc_mask, vy_bc_mask, v_ang_bc_mask;

	double fx_ext, fy_ext, m_ext;
	double fx_con, fy_con, m_con;
	
	double m, moi; // moment of inertia
	inline void init_cal_var()
	{
		m = hx * hy * density;
		moi = m * (hx * hx + hy * hy) / 12.0;
	}

	const static Vector2D edge_normal[4];

public:
	RigidRect() : hx(0.0), hy(0.0), density(1.0),
		ax(0.0), ay(0.0), a_ang(0.0),
		vx(0.0), vy(0.0), v_ang(0.0),
		x(0.0), y(0.0), ang(0.0),
		ax_bc(0.0), ay_bc(0.0), a_ang_bc(0.0),
		vx_bc(0.0), vy_bc(0.0), v_ang_bc(0.0),
		ax_bc_mask(0), ay_bc_mask(0), a_ang_bc_mask(0),
		vx_bc_mask(0), vy_bc_mask(0), v_ang_bc_mask(0),
		fx_ext(0.0), fy_ext(0.0), m_ext(0.0),
		fx_con(0.0), fy_con(0.0), m_con(0.0)
	{ init_cal_var(); }
	~RigidRect() {}

	inline double get_hx() const noexcept { return hx; }
	inline double get_hy() const noexcept { return hy; }
	
	inline double get_density() const noexcept { return density; }
	inline double get_m() const noexcept { return m; }
	inline double get_moi() const noexcept { return moi; }

	inline double get_ax() const noexcept { return ax; }
	inline double get_ay() const noexcept { return ay; }
	inline double get_a_ang() const noexcept { return a_ang; }
	inline double get_vx() const noexcept { return vx; }
	inline double get_vy() const noexcept { return vy; }
	inline double get_v_ang() const noexcept { return v_ang; }
	inline double get_x() const noexcept { return x; }
	inline double get_y() const noexcept { return y; }
	inline double get_ang() const noexcept { return ang; }

	inline double get_fx_external() const noexcept { return fx_ext; }
	inline double get_fy_external() const noexcept { return fy_ext; }
	inline double get_m_external() const noexcept { return m_ext; }
	
	inline double get_fx_contact() const noexcept { return fx_con; }
	inline double get_fy_contact() const noexcept { return fy_con; }
	inline double get_m_contact() const noexcept { return m_con; }

	inline double get_ax_bc() const noexcept { return ax_bc; }
	inline double get_ay_bc() const noexcept { return ay_bc; }
	inline double get_a_ang_bc() const noexcept { return a_ang_bc; }
	inline double get_vx_bc() const noexcept { return vx_bc; }
	inline double get_vy_bc() const noexcept { return vy_bc; }
	inline double get_v_ang_bc() const noexcept { return v_ang_bc; }

	inline bool has_ax_bc() const noexcept { return ax_bc_mask != 0; }
	inline bool has_ay_bc() const noexcept { return ay_bc_mask != 0; }
	inline bool has_a_ang_bc() const noexcept { return a_ang_bc_mask != 0; }
	inline bool has_vx_bc() const noexcept { return vx_bc_mask != 0; }
	inline bool has_vy_bc() const noexcept { return vy_bc_mask != 0; }
	inline bool has_v_ang_bc() const noexcept { return v_ang_bc_mask != 0; }
	
	inline void set_ax_bc(double _a) { ax_bc_mask = ~ax_bc_mask; ax_bc = _a; }
	inline void set_ay_bc(double _a) { ay_bc_mask = ~ay_bc_mask; ay_bc = _a; }
	inline void set_a_ang_bc(double _a) { a_ang_bc_mask = ~a_ang_bc_mask; a_ang_bc = _a; }

	inline void set_vx_bc(double _v) { vx_bc_mask = ~vx_bc_mask; vx_bc = _v; }
	inline void set_vy_bc(double _v) { vy_bc_mask = ~vy_bc_mask; vy_bc = _v; }
	inline void set_v_ang_bc(double _v) { v_ang_bc_mask = ~v_ang_bc_mask; v_ang_bc = _v; }
	inline void set_v_bc(double _vx, double _vy, double _v_ang)
	{ set_vx_bc(_vx); set_vy_bc(_vy); set_v_ang_bc(_v_ang); }

	inline void add_fx_external(double _f) { fx_ext += _f; }
	inline void add_fy_external(double _f) { fy_ext += _f; }
	inline void add_m_external(double _f) { m_ext += _f; }
	inline void add_f_external(double _x, double _y, double _fx, double _fy)
	{
		fx_ext += _fx;
		fy_ext += _fy;
		m_ext += (_x - x) * _fy - (_y - y) * _fx;
	}

	void init(double _x, double _y, double _hx, double _hy, double _density = 1.0);

	void set_init_state(double _hx, double _hy, double _density,
		double _fx_cont, double _fy_cont, double _m_cont,
		double _ax, double _ay, double _a_ang,
		double _vx, double _vy, double _v_ang,
		double _x, double _y, double _ang);

public: // helper function for calculation
	inline void get_bbox(Rect &box, double exp_size = 0.0) const noexcept
	{
		double hhx = hx * 0.5;
		double hhy = hy * 0.5;
		box.xl = x - hhx - exp_size;
		box.xu = x + hhx + exp_size;
		box.yl = y - hhy - exp_size;
		box.yu = y + hhy + exp_size;
	}

	inline void reset_f_contact() noexcept
	{ fx_con = 0.0; fy_con = 0.0; m_con = 0.0; }

	inline void add_f_contact(
		double _x, double _y,
		double fx, double fy
		) noexcept
	{
		fx_con += fx; fy_con += fy;
		m_con += (_x - x) * fy - (_y - y) * fx;
	}

	// return true if point(x, y) collides with circle
	// also calculate overlapping distance and norm direction at that point
	// return false if point is outside circle
	inline bool detect_collision_with_point(
		double _x, double _y, double pcl_vol,
		double& overlap_dist, double& norm_x, double& norm_y
		)
	{
		double pcl_radius = sqrt(pcl_vol) * 0.5;
		Rect bbox;
		get_bbox(bbox, pcl_radius);
		if (!bbox.is_in_box(_x, _y))
			return false;

		unsigned char closest_edge_id;
		double dist_tmp;
		// edge 0
		overlap_dist = _x - bbox.xl;
		closest_edge_id = 0;
		// edge 1
		dist_tmp = bbox.xu - _x;
		if (dist_tmp < overlap_dist)
			closest_edge_id = 1;
		// edge 2
		dist_tmp = _y - bbox.yl;
		if (dist_tmp < overlap_dist)
			closest_edge_id = 2;
		// edge 3
		dist_tmp = bbox.yu - _y;
		if (dist_tmp < overlap_dist)
			closest_edge_id = 3;
		const Vector2D& en = edge_normal[closest_edge_id];
		norm_x = en.x;
		norm_y = en.y;
		return true;
	}

	// update rigid circle motion and position
	void update_motion(double dt)
	{
		// update a
		ax = (fx_con + fx_ext) / m;
		ay = (fy_con + fy_ext) / m;
		a_ang = (m_con + m_ext) / moi;
		// apply abc
		ax = (ax_ull & ~ax_bc_mask) | (ax_bc_ull & ax_bc_mask);
		ay = (ay_ull & ~ay_bc_mask) | (ay_bc_ull & ay_bc_mask);
		a_ang = (a_ang_ull & ~a_ang_bc_mask) | (a_ang_bc_ull & a_ang_bc_mask);
		// update velocity
		vx += ax * dt;
		vy += ay * dt;
		v_ang += a_ang * dt;
		// apply vbc
		vx = (vx_ull & ~vx_bc_mask) | (vx_bc_ull & vx_bc_mask);
		vy = (vy_ull & ~vy_bc_mask) | (vy_bc_ull & vy_bc_mask);
		v_ang = (v_ang_ull & ~v_ang_bc_mask) | (v_ang_bc_ull & v_ang_bc_mask);
		// update position
		x += vx * dt;
		y += vy * dt;
		ang += v_ang * dt;
	}
};

#endif