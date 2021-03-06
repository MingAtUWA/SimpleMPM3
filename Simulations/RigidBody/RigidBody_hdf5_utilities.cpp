#include "Simulations_pcp.h"

#include "RigidBody_hdf5_utilities.h"

namespace RigidBody_hdf5_utilities
{
	// rigid circle
	int output_rigid_circle_to_hdf5_file(
		RigidCircle& rc,
		ResultFile_hdf5& rf,
		hid_t rc_grp_id
	)
	{
		if (rc_grp_id < 0)
			return -1;

		rf.write_attribute(rc_grp_id, "radius", rc.get_radius());
		rf.write_attribute(rc_grp_id, "density", rc.get_density());
		rf.write_attribute(rc_grp_id, "rfx", rc.get_rfx());
		rf.write_attribute(rc_grp_id, "rfy", rc.get_rfy());
		rf.write_attribute(rc_grp_id, "rm", rc.get_rm());
		rf.write_attribute(rc_grp_id, "ax", rc.get_ax());
		rf.write_attribute(rc_grp_id, "ay", rc.get_ay());
		rf.write_attribute(rc_grp_id, "a_angle", rc.get_a_ang());
		rf.write_attribute(rc_grp_id, "vx", rc.get_vx());
		rf.write_attribute(rc_grp_id, "vy", rc.get_vy());
		rf.write_attribute(rc_grp_id, "v_angle", rc.get_v_ang());
		rf.write_attribute(rc_grp_id, "x", rc.get_x());
		rf.write_attribute(rc_grp_id, "y", rc.get_y());
		rf.write_attribute(rc_grp_id, "angle", rc.get_ang());

		// boundary conditions
		rf.write_attribute(rc_grp_id, "rfx_bc", rc.get_rfx_bc());
		rf.write_attribute(rc_grp_id, "rfy_bc", rc.get_rfy_bc());
		rf.write_attribute(rc_grp_id, "rm_bc", rc.get_rm_bc());
		if (rc.has_ax_bc())
			rf.write_attribute(rc_grp_id, "ax_bc", rc.get_ax_bc());
		if (rc.has_ay_bc())
			rf.write_attribute(rc_grp_id, "ay_bc", rc.get_ay_bc());
		if (rc.has_a_ang_bc())
			rf.write_attribute(rc_grp_id, "a_ang_bc", rc.get_a_ang_bc());
		if (rc.has_vx_bc())
			rf.write_attribute(rc_grp_id, "vx_bc", rc.get_vx_bc());
		if (rc.has_vy_bc())
			rf.write_attribute(rc_grp_id, "vy_bc", rc.get_vy_bc());
		if (rc.has_v_ang_bc())
			rf.write_attribute(rc_grp_id, "v_ang_bc", rc.get_v_ang());

		return 0;
	}

	int load_rigid_circle_from_hdf5_file(
		RigidCircle& rc,
		ResultFile_hdf5& rf,
		hid_t rc_grp_id
	)
	{
		if (rc_grp_id < 0)
			return -1;

		double rc_radius, rc_density;
		double rc_rfx, rc_rfy, rc_rm;
		double rc_ax, rc_ay, rc_a_ang;
		double rc_vx, rc_vy, rc_v_ang;
		double rc_x, rc_y, rc_ang;

		rf.read_attribute(rc_grp_id, "radius", rc_radius);
		rf.read_attribute(rc_grp_id, "density", rc_density);
		rf.read_attribute(rc_grp_id, "rfx", rc_rfx);
		rf.read_attribute(rc_grp_id, "rfy", rc_rfy);
		rf.read_attribute(rc_grp_id, "rm", rc_rm);
		rf.read_attribute(rc_grp_id, "ax", rc_ax);
		rf.read_attribute(rc_grp_id, "ay", rc_ay);
		rf.read_attribute(rc_grp_id, "a_angle", rc_a_ang);
		rf.read_attribute(rc_grp_id, "vx", rc_vx);
		rf.read_attribute(rc_grp_id, "vy", rc_vy);
		rf.read_attribute(rc_grp_id, "v_angle", rc_v_ang);
		rf.read_attribute(rc_grp_id, "x", rc_x);
		rf.read_attribute(rc_grp_id, "y", rc_y);
		rf.read_attribute(rc_grp_id, "angle", rc_ang);

		rc.set_init_state(
			rc_radius, rc_density,
			rc_rfx, rc_rfy, rc_rm,
			rc_ax, rc_ay, rc_a_ang,
			rc_vx, rc_vy, rc_v_ang,
			rc_x, rc_y, rc_ang
		);

		// boundary conditions
		double bc_value;
		rf.read_attribute(rc_grp_id, "rfx_bc", bc_value);
		rc.add_rfx_bc(bc_value);
		rf.read_attribute(rc_grp_id, "rfy_bc", bc_value);
		rc.add_rfy_bc(bc_value);
		rf.read_attribute(rc_grp_id, "rm_bc", bc_value);
		rc.add_rm_bc(bc_value);
		if (rf.has_attribute(rc_grp_id, "ax_bc"))
		{
			rf.read_attribute(rc_grp_id, "ax_bc", bc_value);
			rc.set_ax_bc(bc_value);
		}
		if (rf.has_attribute(rc_grp_id, "ay_bc"))
		{
			rf.read_attribute(rc_grp_id, "ay_bc", bc_value);
			rc.set_ay_bc(bc_value);
		}
		if (rf.has_attribute(rc_grp_id, "a_ang_bc"))
		{
			rf.read_attribute(rc_grp_id, "a_ang_bc", bc_value);
			rc.set_a_ang_bc(bc_value);
		}
		if (rf.has_attribute(rc_grp_id, "vx_bc"))
		{
			rf.read_attribute(rc_grp_id, "vx_bc", bc_value);
			rc.set_vx_bc(bc_value);
		}
		if (rf.has_attribute(rc_grp_id, "vy_bc"))
		{
			rf.read_attribute(rc_grp_id, "vy_bc", bc_value);
			rc.set_vy_bc(bc_value);
		}
		if (rf.has_attribute(rc_grp_id, "v_ang_bc"))
		{
			rf.read_attribute(rc_grp_id, "v_ang_bc", bc_value);
			rc.set_v_ang_bc(bc_value);
		}

		return 0;
	}

	// rigid rect
	int output_rigid_rect_to_hdf5_file(
		RigidRect& rr,
		ResultFile_hdf5& rf,
		hid_t rr_grp_id
	)
	{
		if (rr_grp_id < 0)
			return -1;

		rf.write_attribute(rr_grp_id, "hx", rr.get_hx());
		rf.write_attribute(rr_grp_id, "hy", rr.get_hy());
		rf.write_attribute(rr_grp_id, "density", rr.get_density());

		rf.write_attribute(rr_grp_id, "ax", rr.get_ax());
		rf.write_attribute(rr_grp_id, "ay", rr.get_ay());
		rf.write_attribute(rr_grp_id, "a_angle", rr.get_a_ang());
		rf.write_attribute(rr_grp_id, "vx", rr.get_vx());
		rf.write_attribute(rr_grp_id, "vy", rr.get_vy());
		rf.write_attribute(rr_grp_id, "v_angle", rr.get_v_ang());
		rf.write_attribute(rr_grp_id, "x", rr.get_x());
		rf.write_attribute(rr_grp_id, "y", rr.get_y());
		rf.write_attribute(rr_grp_id, "angle", rr.get_ang());

		rf.write_attribute(rr_grp_id, "fx_contact", rr.get_fx_contact());
		rf.write_attribute(rr_grp_id, "fy_contact", rr.get_fy_contact());
		rf.write_attribute(rr_grp_id, "m_contact", rr.get_m_contact());
		rf.write_attribute(rr_grp_id, "fx_external", rr.get_fx_external());
		rf.write_attribute(rr_grp_id, "fy_external", rr.get_fy_external());
		rf.write_attribute(rr_grp_id, "m_external", rr.get_m_external());

		if (rr.has_ax_bc())
			rf.write_attribute(rr_grp_id, "ax_bc", rr.get_ax_bc());
		if (rr.has_ay_bc())
			rf.write_attribute(rr_grp_id, "ay_bc", rr.get_ay_bc());
		if (rr.has_a_ang_bc())
			rf.write_attribute(rr_grp_id, "a_ang_bc", rr.get_a_ang_bc());
		if (rr.has_vx_bc())
			rf.write_attribute(rr_grp_id, "vx_bc", rr.get_vx_bc());
		if (rr.has_vy_bc())
			rf.write_attribute(rr_grp_id, "vy_bc", rr.get_vy_bc());
		if (rr.has_v_ang_bc())
			rf.write_attribute(rr_grp_id, "v_ang_bc", rr.get_v_ang());

		return 0;
	}

	int load_rigid_rect_from_hdf5_file(
		RigidRect& rr,
		ResultFile_hdf5& rf,
		hid_t rr_grp_id
	)
	{
		if (rr_grp_id < 0)
			return -1;

		double hx, hy, density;
		double ax, ay, a_ang;
		double vx, vy, v_ang;
		double x, y, ang;
		double fx_con, fy_con, m_con;
		rf.read_attribute(rr_grp_id, "hx", hx);
		rf.read_attribute(rr_grp_id, "hy", hy);
		rf.read_attribute(rr_grp_id, "density", density);
		rf.read_attribute(rr_grp_id, "ax", ax);
		rf.read_attribute(rr_grp_id, "ay", ay);
		rf.read_attribute(rr_grp_id, "a_angle", a_ang);
		rf.read_attribute(rr_grp_id, "vx", vx);
		rf.read_attribute(rr_grp_id, "vy", vy);
		rf.read_attribute(rr_grp_id, "v_angle", v_ang);
		rf.read_attribute(rr_grp_id, "x", x);
		rf.read_attribute(rr_grp_id, "y", y);
		rf.read_attribute(rr_grp_id, "angle", ang);
		rf.read_attribute(rr_grp_id, "fx_contact", fx_con);
		rf.read_attribute(rr_grp_id, "fy_contact", fy_con);
		rf.read_attribute(rr_grp_id, "m_contact", m_con);

		rr.set_init_state(hx, hy, density,
			fx_con, fy_con, m_con,
			ax, ay, a_ang,
			vx, vy, v_ang,
			x, y, ang
		);

		// boundary conditions
		double fx_ext, fy_ext, m_ext;
		double ax_bc, ay_bc, a_ang_bc;
		double vx_bc, vy_bc, v_ang_bc;
		rf.read_attribute(rr_grp_id, "fx_external", fx_ext);
		rr.add_fx_external(fx_ext);
		rf.read_attribute(rr_grp_id, "fy_external", fy_ext);
		rr.add_fy_external(fy_ext);
		rf.read_attribute(rr_grp_id, "m_external", m_ext);
		rr.add_m_external(m_ext);
		if (rf.has_attribute(rr_grp_id, "ax_bc"))
		{
			rf.read_attribute(rr_grp_id, "ax_bc", ax_bc);
			rr.set_ax_bc(ax_bc);
		}
		if (rf.has_attribute(rr_grp_id, "ay_bc"))
		{
			rf.read_attribute(rr_grp_id, "ay_bc", ay_bc);
			rr.set_ay_bc(ay_bc);
		}
		if (rf.has_attribute(rr_grp_id, "a_ang_bc"))
		{
			rf.read_attribute(rr_grp_id, "a_ang_bc", a_ang_bc);
			rr.set_a_ang_bc(a_ang_bc);
		}
		if (rf.has_attribute(rr_grp_id, "vx_bc"))
		{
			rf.read_attribute(rr_grp_id, "vx_bc", vx_bc);
			rr.set_vx_bc(vx_bc);
		}
		if (rf.has_attribute(rr_grp_id, "vy_bc"))
		{
			rf.read_attribute(rr_grp_id, "vy_bc", vy_bc);
			rr.set_vy_bc(vy_bc);
		}
		if (rf.has_attribute(rr_grp_id, "v_ang_bc"))
		{
			rf.read_attribute(rr_grp_id, "v_ang_bc", v_ang_bc);
			rr.set_v_ang_bc(v_ang_bc);
		}

		return 0;
	}

	// only output state info, no mesh info
	int output_rigid_tetrahedron_mesh_state_to_hdf5_file(
		RigidTetrahedronMesh& rtm,
		ResultFile_hdf5& rf,
		hid_t rtm_grp_id
	)
	{
		rf.write_attribute(rtm_grp_id, "density", rtm.get_density());

		rf.write_attribute(rtm_grp_id, "ax", rtm.get_ax());
		rf.write_attribute(rtm_grp_id, "ay", rtm.get_ay());
		rf.write_attribute(rtm_grp_id, "az", rtm.get_az());
		rf.write_attribute(rtm_grp_id, "vx", rtm.get_vx());
		rf.write_attribute(rtm_grp_id, "vy", rtm.get_vy());
		rf.write_attribute(rtm_grp_id, "vz", rtm.get_vz());
		rf.write_attribute(rtm_grp_id, "x", rtm.get_x());
		rf.write_attribute(rtm_grp_id, "y", rtm.get_y());
		rf.write_attribute(rtm_grp_id, "z", rtm.get_z());

		rf.write_attribute(rtm_grp_id, "fx_ext", rtm.get_fx_ext());
		rf.write_attribute(rtm_grp_id, "fy_ext", rtm.get_fy_ext());
		rf.write_attribute(rtm_grp_id, "fz_ext", rtm.get_fz_ext());
		rf.write_attribute(rtm_grp_id, "fx_contact", rtm.get_fx_contact());
		rf.write_attribute(rtm_grp_id, "fy_contact", rtm.get_fy_contact());
		rf.write_attribute(rtm_grp_id, "fz_contact", rtm.get_fz_contact());

		rf.write_attribute(rtm_grp_id, "ax_ang", rtm.get_ax_ang());
		rf.write_attribute(rtm_grp_id, "ay_ang", rtm.get_ay_ang());
		rf.write_attribute(rtm_grp_id, "az_ang", rtm.get_az_ang());
		rf.write_attribute(rtm_grp_id, "vx_ang", rtm.get_vx_ang());
		rf.write_attribute(rtm_grp_id, "vy_ang", rtm.get_vy_ang());
		rf.write_attribute(rtm_grp_id, "vz_ang", rtm.get_vz_ang());
		rf.write_attribute(rtm_grp_id, "x_ang", rtm.get_x_ang());
		rf.write_attribute(rtm_grp_id, "y_ang", rtm.get_y_ang());
		rf.write_attribute(rtm_grp_id, "z_ang", rtm.get_z_ang());

		rf.write_attribute(rtm_grp_id, "mx_ext", rtm.get_mx_ext());
		rf.write_attribute(rtm_grp_id, "my_ext", rtm.get_my_ext());
		rf.write_attribute(rtm_grp_id, "mz_ext", rtm.get_mz_ext());
		rf.write_attribute(rtm_grp_id, "mx_contact", rtm.get_mx_contact());
		rf.write_attribute(rtm_grp_id, "my_contact", rtm.get_my_contact());
		rf.write_attribute(rtm_grp_id, "mz_contact", rtm.get_mz_contact());

		if (rtm.has_ax_bc())
			rf.write_attribute(rtm_grp_id, "ax_bc", rtm.get_ax_bc());
		if (rtm.has_ay_bc())
			rf.write_attribute(rtm_grp_id, "ay_bc", rtm.get_ay_bc());
		if (rtm.has_az_bc())
			rf.write_attribute(rtm_grp_id, "az_bc", rtm.get_az_bc());

		if (rtm.has_vx_bc())
			rf.write_attribute(rtm_grp_id, "vx_bc", rtm.get_vx_bc());
		if (rtm.has_vy_bc())
			rf.write_attribute(rtm_grp_id, "vy_bc", rtm.get_vy_bc());
		if (rtm.has_vz_bc())
			rf.write_attribute(rtm_grp_id, "vz_bc", rtm.get_vz_bc());

		if (rtm.has_ax_ang_bc())
			rf.write_attribute(rtm_grp_id, "ax_ang_bc", rtm.get_ax_ang_bc());
		if (rtm.has_ay_ang_bc())
			rf.write_attribute(rtm_grp_id, "ay_ang_bc", rtm.get_ay_ang_bc());
		if (rtm.has_az_ang_bc())
			rf.write_attribute(rtm_grp_id, "az_ang_bc", rtm.get_az_ang_bc());

		if (rtm.has_vx_ang_bc())
			rf.write_attribute(rtm_grp_id, "vx_ang_bc", rtm.get_vx_ang_bc());
		if (rtm.has_vy_ang_bc())
			rf.write_attribute(rtm_grp_id, "vy_ang_bc", rtm.get_vy_ang_bc());
		if (rtm.has_vz_ang_bc())
			rf.write_attribute(rtm_grp_id, "vz_ang_bc", rtm.get_vz_ang_bc());

		return 0;
	}

	int load_rigid_tetrahedron_mesh_state_from_hdf5_file(
		RigidTetrahedronMesh& rtm,
		ResultFile_hdf5& rf,
		hid_t rtm_grp_id
	)
	{
		if (rtm_grp_id < 0)
			return -1;

		double rtm_density;
		double rtm_fx_contact, rtm_fy_contact, rtm_fz_contact;
		double rtm_ax, rtm_ay, rtm_az;
		double rtm_vx, rtm_vy, rtm_vz;
		double rtm_x, rtm_y, rtm_z;
		double rtm_mx_contact, rtm_my_contact, rtm_mz_contact;
		double rtm_ax_ang, rtm_ay_ang, rtm_az_ang;
		double rtm_vx_ang, rtm_vy_ang, rtm_vz_ang;
		double rtm_x_ang, rtm_y_ang, rtm_z_ang;

		rf.read_attribute(rtm_grp_id, "density", rtm_density);

		rf.read_attribute(rtm_grp_id, "ax", rtm_ax);
		rf.read_attribute(rtm_grp_id, "ay", rtm_ay);
		rf.read_attribute(rtm_grp_id, "az", rtm_az);
		rf.read_attribute(rtm_grp_id, "vx", rtm_vx);
		rf.read_attribute(rtm_grp_id, "vy", rtm_vy);
		rf.read_attribute(rtm_grp_id, "vz", rtm_vz);
		rf.read_attribute(rtm_grp_id, "x", rtm_x);
		rf.read_attribute(rtm_grp_id, "y", rtm_y);
		rf.read_attribute(rtm_grp_id, "z", rtm_z);
		rf.read_attribute(rtm_grp_id, "fx_contact", rtm_fx_contact);
		rf.read_attribute(rtm_grp_id, "fy_contact", rtm_fy_contact);
		rf.read_attribute(rtm_grp_id, "fz_contact", rtm_fz_contact);

		rf.read_attribute(rtm_grp_id, "ax_ang", rtm_ax_ang);
		rf.read_attribute(rtm_grp_id, "ay_ang", rtm_ay_ang);
		rf.read_attribute(rtm_grp_id, "az_ang", rtm_az_ang);
		rf.read_attribute(rtm_grp_id, "vx_ang", rtm_vx_ang);
		rf.read_attribute(rtm_grp_id, "vy_ang", rtm_vy_ang);
		rf.read_attribute(rtm_grp_id, "vz_ang", rtm_vz_ang);
		rf.read_attribute(rtm_grp_id, "x_ang", rtm_x_ang);
		rf.read_attribute(rtm_grp_id, "y_ang", rtm_y_ang);
		rf.read_attribute(rtm_grp_id, "z_ang", rtm_z_ang);
		rf.read_attribute(rtm_grp_id, "mx_contact", rtm_mx_contact);
		rf.read_attribute(rtm_grp_id, "my_contact", rtm_my_contact);
		rf.read_attribute(rtm_grp_id, "mz_contact", rtm_mz_contact);

		rtm.set_init_state(
			rtm_density,
			rtm_fx_contact, rtm_fy_contact, rtm_fz_contact,
			rtm_ax, rtm_ay, rtm_az,
			rtm_vx, rtm_vy, rtm_vz,
			rtm_x, rtm_y, rtm_z,
			rtm_mx_contact, rtm_my_contact, rtm_mz_contact,
			rtm_ax_ang, rtm_ay_ang, rtm_az_ang,
			rtm_vx_ang, rtm_vy_ang, rtm_vz_ang,
			rtm_x_ang, rtm_y_ang, rtm_z_ang
		);

		double bc_value;
		rf.read_attribute(rtm_grp_id, "fx_ext", bc_value);
		rtm.add_fx_ext(bc_value);
		rf.read_attribute(rtm_grp_id, "fy_ext", bc_value);
		rtm.add_fy_ext(bc_value);
		rf.read_attribute(rtm_grp_id, "fz_ext", bc_value);
		rtm.add_fz_ext(bc_value);
		if (rf.has_attribute(rtm_grp_id, "ax_bc"))
		{
			rf.read_attribute(rtm_grp_id, "ax_bc", bc_value);
			rtm.set_ax_bc(bc_value);
		}
		if (rf.has_attribute(rtm_grp_id, "ay_bc"))
		{
			rf.read_attribute(rtm_grp_id, "ay_bc", bc_value);
			rtm.set_ay_bc(bc_value);
		}
		if (rf.has_attribute(rtm_grp_id, "az_bc"))
		{
			rf.read_attribute(rtm_grp_id, "az_bc", bc_value);
			rtm.set_az_bc(bc_value);
		}
		if (rf.has_attribute(rtm_grp_id, "vx_bc"))
		{
			rf.read_attribute(rtm_grp_id, "vx_bc", bc_value);
			rtm.set_vx_bc(bc_value);
		}
		if (rf.has_attribute(rtm_grp_id, "vy_bc"))
		{
			rf.read_attribute(rtm_grp_id, "vy_bc", bc_value);
			rtm.set_vy_bc(bc_value);
		}
		if (rf.has_attribute(rtm_grp_id, "vz_bc"))
		{
			rf.read_attribute(rtm_grp_id, "vz_bc", bc_value);
			rtm.set_vz_bc(bc_value);
		}

		rf.read_attribute(rtm_grp_id, "mx_ext", bc_value);
		rtm.add_mx_ext(bc_value);
		rf.read_attribute(rtm_grp_id, "my_ext", bc_value);
		rtm.add_my_ext(bc_value);
		rf.read_attribute(rtm_grp_id, "mz_ext", bc_value);
		rtm.add_mz_ext(bc_value);
		if (rf.has_attribute(rtm_grp_id, "ax_ang_bc"))
		{
			rf.read_attribute(rtm_grp_id, "ax_ang_bc", bc_value);
			rtm.set_ax_bc(bc_value);
		}
		if (rf.has_attribute(rtm_grp_id, "ay_ang_bc"))
		{
			rf.read_attribute(rtm_grp_id, "ay_ang_bc", bc_value);
			rtm.set_ay_bc(bc_value);
		}
		if (rf.has_attribute(rtm_grp_id, "az_ang_bc"))
		{
			rf.read_attribute(rtm_grp_id, "az_ang_bc", bc_value);
			rtm.set_az_bc(bc_value);
		}
		if (rf.has_attribute(rtm_grp_id, "vx_ang_bc"))
		{
			rf.read_attribute(rtm_grp_id, "vx_ang_bc", bc_value);
			rtm.set_vx_bc(bc_value);
		}
		if (rf.has_attribute(rtm_grp_id, "vy_ang_bc"))
		{
			rf.read_attribute(rtm_grp_id, "vy_ang_bc", bc_value);
			rtm.set_vy_bc(bc_value);
		}
		if (rf.has_attribute(rtm_grp_id, "vz_ang_bc"))
		{
			rf.read_attribute(rtm_grp_id, "vz_ang_bc", bc_value);
			rtm.set_vz_bc(bc_value);
		}

		return 0;
	}

	int output_rigid_tetrahedron_mesh_to_hdf5_file(
		RigidTetrahedronMesh& rtm,
		ResultFile_hdf5& rf,
		hid_t rtm_grp_id
	)
	{
		if (rtm_grp_id < 0)
			return -1;

		int res;
		size_t node_num = rtm.get_node_num();
		rf.write_attribute(rtm_grp_id, "node_num", node_num);
		RigidTetrahedronMesh::Node* nodes = rtm.get_nodes();
		RigidTehMeshNodeData* node_data = new RigidTehMeshNodeData[node_num];
		for (size_t n_id = 0; n_id < node_num; ++n_id)
		{
			RigidTehMeshNodeData& nd = node_data[n_id];
			RigidTetrahedronMesh::Node& n = nodes[n_id];
			nd.from_node(n);
		}
		hid_t node_dt_id = get_rigid_teh_mesh_node_dt_id();
		res = rf.write_dataset(
			rtm_grp_id,
			"NodeData",
			node_num,
			node_data,
			node_dt_id
		);
		H5Tclose(node_dt_id);
		delete[] node_data;

		size_t elem_num = rtm.get_elem_num();
		rf.write_attribute(rtm_grp_id, "elem_num", elem_num);
		RigidTetrahedronMesh::Element* elems = rtm.get_elems();
		RigidTehMeshElemData* elem_data = new RigidTehMeshElemData[elem_num];
		for (size_t e_id = 0; e_id < elem_num; ++e_id)
		{
			RigidTehMeshElemData& ed = elem_data[e_id];
			RigidTetrahedronMesh::Element& e = elems[e_id];
			ed.from_elem(e);
		}
		hid_t elem_dt_id = get_rigid_teh_mesh_elem_dt_id();
		res = rf.write_dataset(
			rtm_grp_id,
			"ElementData",
			elem_num,
			elem_data,
			elem_dt_id
		);
		H5Tclose(elem_dt_id);
		delete[] elem_data;

		//size_t bface_num = rtm.get_bface_num();
		//rf.write_attribute(rtm_grp_id, "bface_num", bface_num);
		//RigidTetrahedronMesh::Face* bfaces = rtm.get_bfaces();
		//RigidTehMeshFaceData* bface_data = new RigidTehMeshFaceData[bface_num];
		//for (size_t f_id = 0; f_id < bface_num; ++f_id)
		//{
		//	RigidTehMeshFaceData &fd = bface_data[f_id];
		//	RigidTetrahedronMesh::Face &f = bfaces[f_id];
		//	fd.from_face(f);
		//}
		//hid_t face_dt_id = get_rigid_teh_mesh_face_dt_id();
		//res = rf.write_dataset(
		//	rtm_grp_id,
		//	"BoundaryFaceData",
		//	bface_num,
		//	bface_data,
		//	face_dt_id
		//);
		//H5Tclose(face_dt_id);
		//delete[] bface_data;

		return 0;
	}

	int load_rigid_tetrahedron_mesh_from_hdf5_file(
		RigidTetrahedronMesh& rtm,
		ResultFile_hdf5& rf,
		hid_t rtm_grp_id
	)
	{
		if (rtm_grp_id < 0)
			return -1;

		int res;
		size_t node_num;
		rf.read_attribute(rtm_grp_id, "node_num", node_num);
		RigidTehMeshNodeData* node_data = new RigidTehMeshNodeData[node_num];
		hid_t node_dt_id = get_rigid_teh_mesh_node_dt_id();
		res = rf.read_dataset(
			rtm_grp_id,
			"NodeData",
			node_num,
			node_data,
			node_dt_id
		);
		H5Tclose(node_dt_id);
		RigidTetrahedronMesh::Node* nodes = rtm.alloc_nodes(node_num);
		for (size_t n_id = 0; n_id < node_num; ++n_id)
		{
			RigidTehMeshNodeData& nd = node_data[n_id];
			RigidTetrahedronMesh::Node& n = nodes[n_id];
			nd.to_node(n);
		}
		delete[] node_data;

		size_t elem_num;
		rf.read_attribute(rtm_grp_id, "elem_num", elem_num);
		RigidTehMeshElemData* elem_data = new RigidTehMeshElemData[elem_num];
		hid_t elem_dt_id = get_rigid_teh_mesh_elem_dt_id();
		res = rf.read_dataset(
			rtm_grp_id,
			"ElementData",
			elem_num,
			elem_data,
			elem_dt_id
		);
		H5Tclose(elem_dt_id);
		RigidTetrahedronMesh::Element* elems = rtm.alloc_elements(elem_num);
		for (size_t e_id = 0; e_id < elem_num; ++e_id)
		{
			RigidTehMeshElemData& ed = elem_data[e_id];
			RigidTetrahedronMesh::Element& e = elems[e_id];
			ed.to_elem(e);
		}
		delete[] elem_data;

		rtm.init_mesh_properties_after_loading();

		//size_t bface_num;
		//rf.read_attribute(rtm_grp_id, "bface_num", bface_num);
		//RigidTehMeshFaceData* bface_data = new RigidTehMeshFaceData[bface_num];
		//hid_t face_dt_id = get_rigid_teh_mesh_face_dt_id();
		//res = rf.read_dataset(
		//	rtm_grp_id,
		//	"BoundaryFaceData",
		//	bface_num,
		//	bface_data,
		//	face_dt_id
		//);
		//H5Tclose(face_dt_id);
		//RigidTetrahedronMesh::Face* bfaces = rtm.alloc_bfaces(bface_num);
		//for (size_t f_id = 0; f_id < bface_num; ++f_id)
		//{
		//	RigidTehMeshFaceData& fd = bface_data[f_id];
		//	RigidTetrahedronMesh::Face& f = bfaces[f_id];
		//	fd.to_face(f);
		//}
		//delete[] bface_data;

		return 0;
	}
}
