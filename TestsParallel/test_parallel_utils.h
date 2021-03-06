#ifndef __Test_Parallel_Utils_h__
#define __Test_Parallel_Utils_h__

#include "ItemArray.hpp"
#include "Geometry2D.h"
#include "Geometry3D.h"

typedef MemoryUtils::ItemArray<size_t> IndexArray;
typedef MemoryUtils::ItemArray<Point2D> Point2DArray;
typedef MemoryUtils::ItemArray<Point3D> Point3DArray;

// ======================= 2D =========================
template <typename Model, typename PclPos = typename Model::Position>
void find_2d_pcls(Model& md, IndexArray& pt_array,
	Rect& range, bool need_reset_array = true)
{
	if (need_reset_array)
		pt_array.reset();
	const size_t pcl_num = md.get_pcl_num();
	const PclPos *pcl_pos = md.get_pcl_pos();
	for (size_t p_id = 0; p_id < pcl_num; ++p_id)
	{
		const PclPos& pp = pcl_pos[p_id];
		if (pp.x >= range.xl && pp.x <= range.xu &&
			pp.y >= range.yl && pp.y <= range.yu)
			pt_array.add(p_id);
	}
}

template <typename Model, typename NodePos = typename Model::Position>
void find_2d_nodes_on_x_line(Model& md, IndexArray& id_array,
	float x, bool need_reset_array = true, float tol = 1.0e-3)
{
	if (need_reset_array)
		id_array.reset();
	tol = abs(x) < 1.0 ? tol : abs(x) * tol;
	const float xl = x - tol;
	const float xu = x + tol;
	const size_t node_num = md.get_node_num();
	const NodePos* node_pos = md.get_node_pos();
	for (size_t n_id = 0; n_id < node_num; ++n_id)
	{
		const NodePos& np = node_pos[n_id];
		if (np.x > xl && np.x < xu)
			id_array.add(n_id);
	}
}

template <typename Model, typename NodePos = typename Model::Position>
void find_2d_nodes_on_y_line(Model& md, IndexArray& id_array,
	float y, bool need_reset_array = true, float tol = 1.0e-3)
{
	if (need_reset_array)
		id_array.reset();
	tol = abs(y) < 1.0 ? tol : abs(y) * tol;
	const float yl = y - tol;
	const float yu = y + tol;
	const size_t node_num = md.get_node_num();
	const NodePos* node_pos = md.get_node_pos();
	for (size_t n_id = 0; n_id < node_num; ++n_id)
	{
		const NodePos& np = node_pos[n_id];
		if (np.y > yl && np.y < yu)
			id_array.add(n_id);
	}
}

// ======================= 3D =========================
template <typename Model, typename PclPos = typename Model::Position>
void find_3d_pcls(Model& md, IndexArray& pt_array,
	Cube& range, bool need_reset_array = true)
{
	if (need_reset_array)
		pt_array.reset();

	size_t pcl_num = md.get_pcl_num();
	const PclPos *pcl_pos = md.get_pcl_pos();
	for (size_t p_id = 0; p_id < pcl_num; ++p_id)
	{
		const PclPos &pp = pcl_pos[p_id];
		if (pp.x >= range.xl && pp.x <= range.xu &&
			pp.y >= range.yl && pp.y <= range.yu &&
			pp.z >= range.zl && pp.z <= range.zu)
			pt_array.add(p_id);
	}
}

template <typename Model, typename NodePos = typename Model::Position>
void find_3d_nodes_on_x_plane(Model& md, IndexArray& id_array,
	double x, bool need_reset_array = true, double tol = 1.0e-3)
{
	if (need_reset_array)
		id_array.reset();
	const size_t node_num = md.get_node_num();
	const NodePos* node_pos = md.get_node_pos();
	tol = abs(x) < 1.0 ? tol : abs(x) * tol;
	const double xl = x - tol;
	const double xu = x + tol;
	for (size_t n_id = 0; n_id < node_num; ++n_id)
	{
		const NodePos &n = node_pos[n_id];
		if (n.x > xl && n.x < xu)
			id_array.add(n_id);
	}
}

template <typename Model, typename NodePos = typename Model::Position>
void find_3d_nodes_on_y_plane(Model& md, IndexArray& id_array,
	double y, bool need_reset_array = true, double tol = 1.0e-3)
{
	if (need_reset_array)
		id_array.reset();
	const size_t node_num = md.get_node_num();
	const NodePos *node_pos = md.get_node_pos();
	tol = abs(y) < 1.0 ? tol : abs(y) * tol;
	const double yl = y - tol;
	const double yu = y + tol;
	for (size_t n_id = 0; n_id < node_num; ++n_id)
	{
		const NodePos &n = node_pos[n_id];
		if (n.y > yl && n.y < yu)
			id_array.add(n_id);
	}
}

template <typename Model, typename NodePos = typename Model::Position>
void find_3d_nodes_on_z_plane(Model& md, IndexArray& id_array,
	double z, bool need_reset_array = true, double tol = 1.0e-3)
{
	if (need_reset_array)
		id_array.reset();
	const size_t node_num = md.get_node_num();
	const NodePos *node_pos = md.get_node_pos();
	tol = abs(z) < 1.0 ? tol : abs(z) * tol;
	const double zl = z - tol;
	const double zu = z + tol;
	for (size_t n_id = 0; n_id < node_num; ++n_id)
	{
		const NodePos &n = node_pos[n_id];
		if (n.z > zl && n.z < zu)
			id_array.add(n_id);
	}
}

inline void print_array(
	const size_t* data,
	size_t num,
	size_t data_per_line = 20)
{
	for (size_t i = 0; i < num; ++i)
	{
		std::cout << data[i] << ", ";
		if (i % data_per_line == (data_per_line - 1))
			std::cout << "\n";
	}
	if ((num - 1) % data_per_line != (data_per_line - 1))
		std::cout << "\n";
	std::cout << "\n";
}

#endif