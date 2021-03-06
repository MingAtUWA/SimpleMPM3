#include "ModelViewer_pcp.h"

#include "QtMulticolorCircleGLObject.h"

QtMultiColorCircleGLObject::
	QtMultiColorCircleGLObject(QOpenGLFunctions_3_3_Core& _gl) :
	gl(_gl), vao(0), vbo_cs(0), veo_cs(0), vbo_pts(0),
    c_elem_node_num(0), pt_num(0) {}

QtMultiColorCircleGLObject::~QtMultiColorCircleGLObject() { clear(); }

void QtMultiColorCircleGLObject::clear()
{
    if (veo_cs)
    {
        gl.glDeleteBuffers(1, &veo_cs);
        veo_cs = 0;
    }
    if (vbo_cs)
    {
        gl.glDeleteBuffers(1, &vbo_cs);
        vbo_cs = 0;
    }
    if (vbo_pts)
    {
        gl.glDeleteBuffers(1, &vbo_pts);
        vbo_pts = 0;
    }
    if (vao)
    {
        gl.glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
}

int QtMultiColorCircleGLObject::init(
    size_t pcl_num,
    double* pcl_x_data,
    double* pcl_y_data,
    double* pcl_vol_data,
    double* pcl_fld_data,
    float radius_scale
    )
{
    pt_data_mem.reserve(pcl_num);
    PointData* pt_data = pt_data_mem.get_mem();
    for (size_t pcl_id = 0; pcl_id < pcl_num; ++pcl_id)
    {
        PointData& pd = pt_data[pcl_id];
        pd.type = 1; // multi color
        pd.x = GLfloat(pcl_x_data[pcl_id]);
        pd.y = GLfloat(pcl_y_data[pcl_id]);
        pd.radius = GLfloat(sqrt(pcl_vol_data[pcl_id]) * 0.5) * radius_scale;
        pd.fld = GLfloat(pcl_fld_data[pcl_id]);
    }
    int res = init_gl_buffer(pt_data, pcl_num);
    return res;
}

int QtMultiColorCircleGLObject::update(
    size_t pcl_num,
    double* pcl_x_data,
    double* pcl_y_data,
    double* pcl_vol_data,
    double* pcl_fld_data,
    float radius_scale
    )
{
    pt_data_mem.reserve(pcl_num);
    PointData* pt_data = pt_data_mem.get_mem();
    for (size_t pcl_id = 0; pcl_id < pcl_num; ++pcl_id)
    {
        PointData& pd = pt_data[pcl_id];
        pd.type = 1; // multi color
        pd.x = GLfloat(pcl_x_data[pcl_id]);
        pd.y = GLfloat(pcl_y_data[pcl_id]);
        pd.radius = GLfloat(sqrt(pcl_vol_data[pcl_id]) * 0.5) * radius_scale;
        pd.fld = GLfloat(pcl_fld_data[pcl_id]);
    }
    int res = update_gl_buffer(pt_data, pcl_num);
    return res;
}

int QtMultiColorCircleGLObject::init(
    size_t pcl_num,
    float *pcl_x_data,
    float *pcl_y_data,
    float *pcl_vol_data,
    float *pcl_fld_data,
    float radius_scale
    )
{
    pt_data_mem.reserve(pcl_num);
    PointData* pt_data = pt_data_mem.get_mem();
    for (size_t pcl_id = 0; pcl_id < pcl_num; ++pcl_id)
    {
        PointData& pd = pt_data[pcl_id];
        pd.type = 1; // multi color
        pd.x = GLfloat(pcl_x_data[pcl_id]);
        pd.y = GLfloat(pcl_y_data[pcl_id]);
        pd.radius = GLfloat(sqrt(pcl_vol_data[pcl_id]) * 0.5) * radius_scale;
        pd.fld = GLfloat(pcl_fld_data[pcl_id]);
    }
    int res = init_gl_buffer(pt_data, pcl_num);
    return res;
}

int QtMultiColorCircleGLObject::update(
    size_t pcl_num,
    float *pcl_x_data,
    float *pcl_y_data,
    float *pcl_vol_data,
    float *pcl_fld_data,
    float radius_scale
)
{
    pt_data_mem.reserve(pcl_num);
    PointData* pt_data = pt_data_mem.get_mem();
    for (size_t pcl_id = 0; pcl_id < pcl_num; ++pcl_id)
    {
        PointData& pd = pt_data[pcl_id];
        pd.type = 1; // multi color
        pd.x = GLfloat(pcl_x_data[pcl_id]);
        pd.y = GLfloat(pcl_y_data[pcl_id]);
        pd.radius = GLfloat(sqrt(pcl_vol_data[pcl_id]) * 0.5) * radius_scale;
        pd.fld = GLfloat(pcl_fld_data[pcl_id]);
    }
    int res = update_gl_buffer(pt_data, pcl_num);
    return res;
}

void QtMultiColorCircleGLObject::draw(QOpenGLShaderProgram& shader)
{
    (void)shader;

    gl.glBindVertexArray(vao);
    gl.glDrawElementsInstanced(
        GL_TRIANGLES,
        c_elem_node_num,
        GL_UNSIGNED_INT,
        (GLvoid*)0,
        pt_num
    );
}

int QtMultiColorCircleGLObject::init_gl_buffer(
    PointData* pds,
    size_t pd_num
    )
{
    clear();

    pt_num = pd_num;

    gl.glGenVertexArrays(1, &vao);
    if (vao == 0)
        return -1;
    gl.glBindVertexArray(vao);
    
    int res = init_circle_data();
    if (res)
        return res;

    gl.glGenBuffers(1, &vbo_pts);
    if (vbo_pts == 0)
        return -1;
    gl.glBindBuffer(GL_ARRAY_BUFFER, vbo_pts);
    gl.glBufferData(GL_ARRAY_BUFFER,
        pd_num * sizeof(PointData),
        pds,
        GL_STREAM_DRAW
        );

    // pt_type
    gl.glVertexAttribIPointer(1,
        1, GL_UNSIGNED_INT,
        sizeof(PointData),
        (GLvoid*)offsetof(PointData, type)
        );
    gl.glEnableVertexAttribArray(1);
    gl.glVertexAttribDivisor(1, 1);
    // pt_pos
    gl.glVertexAttribPointer(2,
        2, GL_FLOAT, GL_FALSE,
        sizeof(PointData),
        (GLvoid*)offsetof(PointData, x)
        );
    gl.glEnableVertexAttribArray(2);
    gl.glVertexAttribDivisor(2, 1);
    // pt_radius
    gl.glVertexAttribPointer(3,
        1, GL_FLOAT, GL_FALSE,
        sizeof(PointData),
        (GLvoid*)offsetof(PointData, radius)
        );
    gl.glEnableVertexAttribArray(3);
    gl.glVertexAttribDivisor(3, 1);
    // point value
    gl.glVertexAttribPointer(4,
        1, GL_FLOAT, GL_FALSE,
        sizeof(PointData),
        (GLvoid*)offsetof(PointData, fld)
        );
    gl.glEnableVertexAttribArray(4);
    gl.glVertexAttribDivisor(4, 1);

    return 0;
}

int QtMultiColorCircleGLObject::update_gl_buffer(
    PointData* pds,
    size_t pd_num
    )
{
    if (!vao || !vbo_pts)
        return -1;

    gl.glBindVertexArray(vao);

    gl.glBindBuffer(GL_ARRAY_BUFFER, vbo_pts);
    gl.glBufferSubData(GL_ARRAY_BUFFER,
        0,
        pd_num * sizeof(PointData),
        (GLvoid *)pds
        );

    return 0;
}

static const GLfloat circle_nodes[] = {
    0.000000f, 0.000000f, // node 0
    1.000000f, 0.000000f, // node 1
    0.994522f, 0.104528f, // node 2
    0.978148f, 0.207912f, // node 3
    0.951057f, 0.309017f, // node 4
    0.913545f, 0.406737f, // node 5
    0.866025f, 0.500000f, // node 6
    0.809017f, 0.587785f, // node 7
    0.743145f, 0.669131f, // node 8
    0.669131f, 0.743145f, // node 9
    0.587785f, 0.809017f, // node 10
    0.500000f, 0.866025f, // node 11
    0.406737f, 0.913545f, // node 12
    0.309017f, 0.951057f, // node 13
    0.207912f, 0.978148f, // node 14
    0.104528f, 0.994522f, // node 15
    0.000000f, 1.000000f, // node 16
    -0.104528f, 0.994522f, // node 17
    -0.207912f, 0.978148f, // node 18
    -0.309017f, 0.951057f, // node 19
    -0.406737f, 0.913545f, // node 20
    -0.500000f, 0.866025f, // node 21
    -0.587785f, 0.809017f, // node 22
    -0.669131f, 0.743145f, // node 23
    -0.743145f, 0.669131f, // node 24
    -0.809017f, 0.587785f, // node 25
    -0.866025f, 0.500000f, // node 26
    -0.913545f, 0.406737f, // node 27
    -0.951057f, 0.309017f, // node 28
    -0.978148f, 0.207912f, // node 29
    -0.994522f, 0.104528f, // node 30
    -1.000000f, 0.000000f, // node 31
    -0.994522f, -0.104528f, // node 32
    -0.978148f, -0.207912f, // node 33
    -0.951057f, -0.309017f, // node 34
    -0.913545f, -0.406737f, // node 35
    -0.866025f, -0.500000f, // node 36
    -0.809017f, -0.587785f, // node 37
    -0.743145f, -0.669131f, // node 38
    -0.669131f, -0.743145f, // node 39
    -0.587785f, -0.809017f, // node 40
    -0.500000f, -0.866025f, // node 41
    -0.406737f, -0.913545f, // node 42
    -0.309017f, -0.951057f, // node 43
    -0.207912f, -0.978148f, // node 44
    -0.104528f, -0.994522f, // node 45
    -0.000000f, -1.000000f, // node 46
    0.104528f, -0.994522f, // node 47
    0.207912f, -0.978148f, // node 48
    0.309017f, -0.951057f, // node 49
    0.406737f, -0.913545f, // node 50
    0.500000f, -0.866025f, // node 51
    0.587785f, -0.809017f, // node 52
    0.669131f, -0.743145f, // node 53
    0.743145f, -0.669131f, // node 54
    0.809017f, -0.587785f, // node 55
    0.866025f, -0.500000f, // node 56
    0.913545f, -0.406737f, // node 57
    0.951057f, -0.309017f, // node 58
    0.978148f, -0.207912f, // node 59
    0.994522f, -0.104528f // node 60
};

static const GLuint circle_elems[] = {
    0, 1, 2, // elem 0
    0, 2, 3, // elem 1
    0, 3, 4, // elem 2
    0, 4, 5, // elem 3
    0, 5, 6, // elem 4
    0, 6, 7, // elem 5
    0, 7, 8, // elem 6
    0, 8, 9, // elem 7
    0, 9, 10, // elem 8
    0, 10, 11, // elem 9
    0, 11, 12, // elem 10
    0, 12, 13, // elem 11
    0, 13, 14, // elem 12
    0, 14, 15, // elem 13
    0, 15, 16, // elem 14
    0, 16, 17, // elem 15
    0, 17, 18, // elem 16
    0, 18, 19, // elem 17
    0, 19, 20, // elem 18
    0, 20, 21, // elem 19
    0, 21, 22, // elem 20
    0, 22, 23, // elem 21
    0, 23, 24, // elem 22
    0, 24, 25, // elem 23
    0, 25, 26, // elem 24
    0, 26, 27, // elem 25
    0, 27, 28, // elem 26
    0, 28, 29, // elem 27
    0, 29, 30, // elem 28
    0, 30, 31, // elem 29
    0, 31, 32, // elem 30
    0, 32, 33, // elem 31
    0, 33, 34, // elem 32
    0, 34, 35, // elem 33
    0, 35, 36, // elem 34
    0, 36, 37, // elem 35
    0, 37, 38, // elem 36
    0, 38, 39, // elem 37
    0, 39, 40, // elem 38
    0, 40, 41, // elem 39
    0, 41, 42, // elem 40
    0, 42, 43, // elem 41
    0, 43, 44, // elem 42
    0, 44, 45, // elem 43
    0, 45, 46, // elem 44
    0, 46, 47, // elem 45
    0, 47, 48, // elem 46
    0, 48, 49, // elem 47
    0, 49, 50, // elem 48
    0, 50, 51, // elem 49
    0, 51, 52, // elem 50
    0, 52, 53, // elem 51
    0, 53, 54, // elem 52
    0, 54, 55, // elem 53
    0, 55, 56, // elem 54
    0, 56, 57, // elem 55
    0, 57, 58, // elem 56
    0, 58, 59, // elem 57
    0, 59, 60, // elem 58
    0, 60, 1 // elem 59
};

int QtMultiColorCircleGLObject::init_circle_data()
{
    c_elem_node_num = sizeof(circle_elems) / sizeof(circle_elems[0]);
    
    gl.glGenBuffers(1, &vbo_cs);
    if (vbo_cs == 0)
        return -1;
    gl.glBindBuffer(GL_ARRAY_BUFFER, vbo_cs);
    gl.glBufferData(GL_ARRAY_BUFFER,
        sizeof(circle_nodes),
        circle_nodes,
        GL_STATIC_DRAW
        );

    gl.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    gl.glEnableVertexAttribArray(0);

    gl.glGenBuffers(1, &veo_cs);
    if (veo_cs == 0)
        return -1;
    gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veo_cs);
    gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(circle_elems),
        circle_elems,
        GL_STATIC_DRAW
        );

    return 0;
}
