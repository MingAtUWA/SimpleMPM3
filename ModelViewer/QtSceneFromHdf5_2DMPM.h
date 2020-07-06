#ifndef __Qt_Scene_From_Hdf5_2D_MPM_h__
#define __Qt_Scene_From_Hdf5_2D_MPM_h__

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

#include "ResultFile_hdf5.h"
#include "QtTriangleMeshGLObject.h"
#include "UniformColorMap_Abaqus.h"
#include "QtMultiColourCircleGLObject.h"

class QtSceneFromHdf5_2DMPM
{
protected:
	QOpenGLFunctions_3_3_Core &gl;

	struct PointData
	{
		GLfloat x, y;
		GLfloat va;
	};

	// hdf5 result file data infos
	ResultFile_hdf5* res_file;
	hid_t th_id;
	hid_t frame_grp_id;
	size_t pcl_num;
	hid_t pcl_dt_id;
	size_t pcl_size;
	size_t pcl_x_off;
	size_t pcl_y_off;
	size_t pcl_vol_off;
	std::string field_name;
	size_t pcl_fld_off;
	hid_t pcl_fld_type;

	QOpenGLShaderProgram shader_plain2D;
	QOpenGLShaderProgram shader_circles;

	bool display_bg_mesh;
	bool display_pcls;

	QtTriangleMeshGLObject bg_mesh_obj;

	UniformColourMap_Abaqus color_map;
	GLuint color_map_texture;
	QtMultiColourCircleGLObject pcls_obj;
	
	bool display_whole_model;
	GLfloat xl, xu, yl, yu, padding_ratio;

	// viewport
	GLint vp_x_pos, vp_y_pos;
	GLsizei vp_x_size, vp_y_size;
	
	void set_viewport(int wd, int ht, GLfloat xlen, GLfloat ylen);
	
	QMatrix4x4 view_mat;

	QVector3D bg_color;
	inline void set_bg_color(GLfloat r, GLfloat g, GLfloat b)
	{
		bg_color[0] = r;
		bg_color[1] = g;
		bg_color[2] = b;
	}

	void clear();

public:
	QtSceneFromHdf5_2DMPM(QOpenGLFunctions_3_3_Core &_gl);
	~QtSceneFromHdf5_2DMPM();
	void close_file();

	inline void set_display_bg_mesh(bool op = true) { display_bg_mesh = op; }
	inline void set_display_pcls(bool op = true) { display_pcls = op; }
	inline void set_display_whole_model() { display_whole_model = true; }
	inline void set_display_range(double _xl, double _xu, double _yl, double _yu)
	{
		display_whole_model = false;
		xl = _xl; xu = _xu; yl = _yl; yu = _yu;
	}

	inline void set_fld_range(double min, double max)
	{ color_map.set_range(min, max); }

	int set_res_file(
		ResultFile_hdf5& rf,
		const char* th_na,
		size_t frame_id,
		const char* field_na
	);

	int initialize(int wd, int ht);
	void draw();
	void resize(int wd, int ht);
};

#endif