#ifndef __Qt_Multi_Color_Ball_GL_Object_h__
#define __Qt_Multi_Color_Ball_GL_Object_h__

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

#include "ItemArray.hpp"

class QtMultiColorBallGLObject
{
protected:
	QOpenGLFunctions_3_3_Core& gl;

	struct PointData
	{
		GLuint type;
		GLfloat x, y, z;
		GLfloat radius;
		GLfloat fld;
	};
	MemoryUtils::ItemArray<PointData> pt_data_mem;

	GLuint vao, vbo_cs, veo_cs, vbo_pts;
	size_t c_elem_node_num;
	size_t pt_num;

	void clear();

	int init_ball_data();
	int init_gl_buffer(PointData *pds, size_t pd_num);
	int update_gl_buffer(PointData *pds, size_t pd_num);

public:
	QtMultiColorBallGLObject(QOpenGLFunctions_3_3_Core& _gl);
	~QtMultiColorBallGLObject();

	template <typename FieldDataType>
	int init(
		char *pcls_data, size_t pcl_size, size_t pcl_num,
		size_t x_off, size_t y_off,
		size_t vol_off, float radius_scale,
		size_t fld_off
		);

	template <typename FieldDataType>
	int update(
		char* pcls_data, size_t pcl_size, size_t pcl_num,
		size_t x_off, size_t y_off,
		size_t vol_off, float radius_scale,
		size_t fld_off
		);

	void draw(QOpenGLShaderProgram& shader);
};

template <typename FieldDataType>
int QtMultiColorBallGLObject::init(
	char* pcls_data, size_t pcl_size, size_t pcl_num,
	size_t x_off, size_t y_off,
	size_t vol_off, float radius_scale,
	size_t fld_off)
{
	GLfloat pcl_vol;
	char* pcl_data = pcls_data;
	pt_data_mem.reserve(pcl_num);
	PointData* pt_datas = pt_data_mem.get_mem();
	for (size_t pcl_id = 0; pcl_id < pcl_num; ++pcl_id)
	{
		PointData& pd = pt_datas[pcl_id];
		pd.type = 1; // multi color
		pd.x = GLfloat(*(double *)(pcl_data + x_off));
		pd.y = GLfloat(*(double *)(pcl_data + y_off));
		pcl_vol = GLfloat(*(double *)(pcl_data + vol_off));
		pd.radius = sqrt(pcl_vol / 3.14159265359f) * radius_scale;
		pd.fld = GLfloat(*(FieldDataType*)(pcl_data + fld_off));
		pcl_data += pcl_size;
	}
	int res = init_gl_buffer(pt_datas, pcl_num);
	return res;
}

template <typename FieldDataType>
int QtMultiColorBallGLObject::update(
	char* pcls_data, size_t pcl_size, size_t pcl_num,
	size_t x_off, size_t y_off,
	size_t vol_off, float radius_scale,
	size_t fld_off
	)
{
	GLfloat pcl_vol;
	char* pcl_data = pcls_data;
	pt_data_mem.reserve(pcl_num);
	PointData* pt_datas = pt_data_mem.get_mem();
	for (size_t pcl_id = 0; pcl_id < pcl_num; ++pcl_id)
	{
		PointData& pd = pt_datas[pcl_id];
		pd.type = 1; // multi color
		pd.x = GLfloat(*(double*)(pcl_data + x_off));
		pd.y = GLfloat(*(double*)(pcl_data + y_off));
		pcl_vol = GLfloat(*(double*)(pcl_data + vol_off));
		pd.radius = sqrt(pcl_vol / 3.14159265359f) * radius_scale;
		pd.fld = GLfloat(*(FieldDataType*)(pcl_data + fld_off));
		pcl_data += pcl_size;
	}
	int res = update_gl_buffer(pt_datas, pcl_num);
	return res;
}

#endif