#include "Simulations_pcp.h"

#include "ParticleGenerator3D.hpp"
#include "Model_T3D_ME_s.h"

Model_T3D_ME_s::Model_T3D_ME_s() :
	pcl_num(0), pcls(nullptr),
	bfx_num(0), bfxs(nullptr), 
	bfy_num(0), bfys(nullptr),
	bfz_num(0), bfzs(nullptr),
	tx_num(0), txs(nullptr),
	ty_num(0), tys(nullptr),
	tz_num(0), tzs(nullptr),
	ax_num(0), axs(nullptr),
	ay_num(0), ays(nullptr),
	az_num(0), azs(nullptr),
	vx_num(0), vxs(nullptr),
	vy_num(0), vys(nullptr),
	vz_num(0), vzs(nullptr),
	rb_is_init(false),
	bg_grid_num(0), bg_grids(nullptr),
	Kn_cont(0.0), Kt_cont(0.0), miu_cont(0.0)
{
	db_file.open("t3d_s_model.txt", std::ios::binary | std::ios::out);
}

Model_T3D_ME_s::~Model_T3D_ME_s()
{
	clear_pcls();
	
	clear_bfxs();
	clear_bfys();
	clear_bfzs();
	clear_txs();
	clear_tys();
	clear_tzs();
	clear_axs();
	clear_ays();
	clear_azs();
	clear_vxs();
	clear_vys();
	clear_vzs();
}

void Model_T3D_ME_s::init_mesh_shape_funcs()
{
	double inv_vol_6;
	for (size_t e_id = 0; e_id < elem_num; ++e_id)
	{
		Element &e = elems[e_id];
		Node &n1 = nodes[e.n1];
		Node &n2 = nodes[e.n2];
		Node &n3 = nodes[e.n3];
		Node &n4 = nodes[e.n4];
		inv_vol_6 = 1.0 / (6.0 * e.vol);
		// N1
		e.a1 = ((n4.y - n2.y) * (n3.z - n2.z) - (n3.y - n2.y) * (n4.z - n2.z)) * inv_vol_6;
		e.b1 = ((n4.z - n2.z) * (n3.x - n2.x) - (n3.z - n2.z) * (n4.x - n2.x)) * inv_vol_6;
		e.c1 = ((n4.x - n2.x) * (n3.y - n2.y) - (n3.x - n2.x) * (n4.y - n2.y)) * inv_vol_6;
		e.coef1 = e.a1 * n2.x + e.b1 * n2.y + e.c1 * n2.z;
		// N2
		e.a2 = ((n4.y - n3.y) * (n1.z - n3.z) - (n1.y - n3.y) * (n4.z - n3.z)) * inv_vol_6;
		e.b2 = ((n4.z - n3.z) * (n1.x - n3.x) - (n1.z - n3.z) * (n4.x - n3.x)) * inv_vol_6;
		e.c2 = ((n4.x - n3.x) * (n1.y - n3.y) - (n1.x - n3.x) * (n4.y - n3.y)) * inv_vol_6;
		e.coef2 = e.a2 * n3.x + e.b2 * n3.y + e.c2 * n3.z;
		// N3
		e.a3 = ((n2.y - n4.y) * (n1.z - n4.z) - (n1.y - n4.y) * (n2.z - n4.z)) * inv_vol_6;
		e.b3 = ((n2.z - n4.z) * (n1.x - n4.x) - (n1.z - n4.z) * (n2.x - n4.x)) * inv_vol_6;
		e.c3 = ((n2.x - n4.x) * (n1.y - n4.y) - (n1.x - n4.x) * (n2.y - n4.y)) * inv_vol_6;
		e.coef3 = e.a3 * n4.x + e.b3 * n4.y + e.c3 * n4.z;
		// N4
		e.a4 = ((n2.y - n1.y) * (n3.z - n1.z) - (n3.y - n1.y) * (n2.z - n1.z)) * inv_vol_6;
		e.b4 = ((n2.z - n1.z) * (n3.x - n1.x) - (n3.z - n1.z) * (n2.x - n1.x)) * inv_vol_6;
		e.c4 = ((n2.x - n1.x) * (n3.y - n1.y) - (n3.x - n1.x) * (n2.y - n1.y)) * inv_vol_6;
		e.coef4 = e.a4 * n1.x + e.b4 * n1.y + e.c4 * n1.z;
		// derivatives
		e.dN1_dx = e.a1;
		e.dN1_dy = e.b1;
		e.dN1_dz = e.c1;
		e.dN2_dx = e.a2;
		e.dN2_dy = e.b2;
		e.dN2_dz = e.c2;
		e.dN3_dx = e.a3;
		e.dN3_dy = e.b3;
		e.dN3_dz = e.c3;
		e.dN4_dx = e.a4;
		e.dN4_dy = e.b4;
		e.dN4_dz = e.c4;
	}

	//db_file << std::fixed << std::left << std::setprecision(8);
	//for (size_t e_id = 0; e_id < elem_num; ++e_id)
	//{
	//	Element& e = elems[e_id];
	//	db_file << e_id << ", " << e.vol << ":\n"
	//		<< e.a1 << ", " << e.b1 << ", " << e.c1 << ", " << e.coef1 << ",\n"
	//		<< e.a2 << ", " << e.b2 << ", " << e.c2 << ", " << e.coef2 << ",\n"
	//		<< e.a3 << ", " << e.b3 << ", " << e.c3 << ", " << e.coef3 << ",\n"
	//		<< e.a4 << ", " << e.b4 << ", " << e.c4 << ", " << e.coef4 << ",\n";
	//}
}

int Model_T3D_ME_s::init_mesh(
	double *node_coords, size_t n_num,
	size_t *elem_n_ids, size_t e_num
	)
{
	int res = BgMesh::init_mesh(node_coords, n_num, elem_n_ids, e_num);
	if (res < 0)
		return res;
	init_mesh_shape_funcs();
	return 0;
}

int Model_T3D_ME_s::load_mesh_from_hdf5(const char *file_name)
{
	int res = BgMesh::load_mesh_from_hdf5(file_name);
	if (res < 0)
		return res;
	init_mesh_shape_funcs();
	return 0;
}

int Model_T3D_ME_s::init_search_grid(double _hx, double _hy, double _hz)
{
	int res = search_bg_grid.init(*this, _hx, _hy, _hz);
	bg_grid_id_box.xl_id = 0;
	bg_grid_id_box.xu_id = search_bg_grid.get_x_num();
	bg_grid_id_box.yl_id = 0;
	bg_grid_id_box.yu_id = search_bg_grid.get_y_num();
	bg_grid_id_box.zl_id = 0;
	bg_grid_id_box.zu_id = search_bg_grid.get_z_num();
	bg_grids = search_bg_grid.get_grids();
	bg_grid_num = search_bg_grid.get_num();

	//db_file << search_bg_grid.get_xl() << ", "
	//	<< search_bg_grid.get_xu() << ", "
	//	<< search_bg_grid.get_yl() << ", "
	//	<< search_bg_grid.get_yu() << ", "
	//	<< search_bg_grid.get_zl() << ", "
	//	<< search_bg_grid.get_zu() << ", "
	//	<< search_bg_grid.get_x_num() << ", "
	//	<< search_bg_grid.get_y_num() << ", "
	//	<< search_bg_grid.get_z_num() << ",\n";

	//for (size_t g_id = 0; g_id < bg_grid_num; g_id++)
	//{
	//	db_file << g_id << ": ";
	//	Grid& g = bg_grids[g_id];
	//	for (auto pe = g.pelems; pe; pe = pe->next)
	//		db_file << pe->e->id << ", ";
	//	db_file << "\n";
	//}

	return res;
}

int Model_T3D_ME_s::init_pcls(size_t num, double m, double density)
{
	clear_pcls();
	if (num == 0)
		return -1;
	
	alloc_pcls(num);
	for (size_t pcl_id = 0; pcl_id < pcl_num; ++pcl_id)
	{
		Particle &pcl = pcls[pcl_id];
		pcl.id = pcl_id;
		pcl.m = m;
		pcl.density = density;
		pcl.vx = 0.0;
		pcl.vy = 0.0;
		pcl.vz = 0.0;
		pcl.e11 = 0.0;
		pcl.e22 = 0.0;
		pcl.e33 = 0.0;
		pcl.e12 = 0.0;
		pcl.e23 = 0.0;
		pcl.e31 = 0.0;
		pcl.s11 = 0.0;
		pcl.s22 = 0.0;
		pcl.s33 = 0.0;
		pcl.s12 = 0.0;
		pcl.s23 = 0.0;
		pcl.s31 = 0.0;
	}
	return 0;
}

int Model_T3D_ME_s::init_pcls(ParticleGenerator3D<Model_T3D_ME_s> &pg, double density)
{
	int res = init_pcls(pg.get_num(), density, density);
	if (res)
		return res;

	ParticleGenerator3D<Model_T3D_ME_s>::Particle *p_iter = pg.first();
	for (size_t pcl_id = 0; pcl_id < pcl_num; ++pcl_id)
	{
		Particle &pcl = pcls[pcl_id];
		pcl.x = p_iter->x;
		pcl.y = p_iter->y;
		pcl.z = p_iter->z;
		pcl.m *= p_iter->vol;
		p_iter = pg.next(p_iter);
	}
	return 0;
}

void Model_T3D_ME_s::alloc_pcls(size_t num)
{
	pcl_num = num;
	pcls = new Particle[pcl_num];
}

void Model_T3D_ME_s::clear_pcls()
{
	if (pcls)
	{
		delete[] pcls;
		pcls = nullptr;
	}
	pcl_num = 0;
}

int Model_T3D_ME_s::init_rb(
	double density,
	const char* file_name,
	double dx,
	double dy,
	double dz,
	double dx_ang,
	double dy_ang,
	double dz_ang
	)
{
	int res = rb.init(density, file_name, dx, dy, dz, dx_ang, dy_ang, dz_ang);
	if (res)
		return res;
	rb_is_init = true;
	return 0;
}
