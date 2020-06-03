#ifndef __Ball_Mesh_Data__
#define __Ball_Mesh_Data__

static const unsigned int ball_node_num = 72;

static const float ball_nodes[] = {
    0.525731, 0.000000, 0.850651, // node 0
    0.525731, 0.000000, -0.850651, // node 1
    -0.525731, 0.000000, 0.850651, // node 2
    -0.525731, 0.000000, -0.850651, // node 3
    0.000000, 0.850651, 0.525731, // node 4
    0.000000, -0.850651, 0.525731, // node 5
    0.000000, 0.850651, -0.525731, // node 6
    0.000000, -0.850651, -0.525731, // node 7
    0.850651, 0.525731, 0.000000, // node 8
    -0.850651, 0.525731, 0.000000, // node 9
    0.850651, -0.525731, 0.000000, // node 10
    -0.850651, -0.525731, 0.000000, // node 11
    0.000000, 1.000000, 0.000000, // node 12
    0.500000, 0.809017, 0.309017, // node 13
    0.500000, 0.809017, -0.309017, // node 14
    -0.500000, 0.809017, 0.309017, // node 15
    0.000000, 1.000000, 0.000000, // node 16
    -0.500000, 0.809017, -0.309017, // node 17
    -0.309017, 0.500000, -0.809017, // node 18
    -0.809017, 0.309017, -0.500000, // node 19
    -0.500000, 0.809017, -0.309017, // node 20
    0.309017, 0.500000, -0.809017, // node 21
    0.000000, 0.000000, -1.000000, // node 22
    -0.309017, 0.500000, -0.809017, // node 23
    0.500000, 0.809017, -0.309017, // node 24
    0.809017, 0.309017, -0.500000, // node 25
    0.309017, 0.500000, -0.809017, // node 26
    1.000000, 0.000000, 0.000000, // node 27
    0.809017, -0.309017, -0.500000, // node 28
    0.809017, 0.309017, -0.500000, // node 29
    0.809017, 0.309017, 0.500000, // node 30
    0.809017, -0.309017, 0.500000, // node 31
    1.000000, 0.000000, 0.000000, // node 32
    0.500000, 0.809017, 0.309017, // node 33
    0.309017, 0.500000, 0.809017, // node 34
    0.809017, 0.309017, 0.500000, // node 35
    -0.309017, 0.500000, 0.809017, // node 36
    0.000000, 0.000000, 1.000000, // node 37
    0.309017, 0.500000, 0.809017, // node 38
    -0.500000, 0.809017, 0.309017, // node 39
    -0.809017, 0.309017, 0.500000, // node 40
    -0.309017, 0.500000, 0.809017, // node 41
    -1.000000, 0.000000, 0.000000, // node 42
    -0.809017, -0.309017, 0.500000, // node 43
    -0.809017, 0.309017, 0.500000, // node 44
    -0.809017, 0.309017, -0.500000, // node 45
    -0.809017, -0.309017, -0.500000, // node 46
    -1.000000, 0.000000, 0.000000, // node 47
    0.000000, 0.000000, -1.000000, // node 48
    0.309017, -0.500000, -0.809017, // node 49
    -0.309017, -0.500000, -0.809017, // node 50
    0.809017, -0.309017, -0.500000, // node 51
    0.500000, -0.809017, -0.309017, // node 52
    0.309017, -0.500000, -0.809017, // node 53
    0.809017, -0.309017, 0.500000, // node 54
    0.309017, -0.500000, 0.809017, // node 55
    0.500000, -0.809017, 0.309017, // node 56
    0.000000, 0.000000, 1.000000, // node 57
    -0.309017, -0.500000, 0.809017, // node 58
    0.309017, -0.500000, 0.809017, // node 59
    -0.809017, -0.309017, 0.500000, // node 60
    -0.500000, -0.809017, 0.309017, // node 61
    -0.309017, -0.500000, 0.809017, // node 62
    -0.309017, -0.500000, -0.809017, // node 63
    -0.500000, -0.809017, -0.309017, // node 64
    -0.809017, -0.309017, -0.500000, // node 65
    -0.500000, -0.809017, 0.309017, // node 66
    -0.500000, -0.809017, -0.309017, // node 67
    0.000000, -1.000000, 0.000000, // node 68
    0.500000, -0.809017, 0.309017, // node 69
    0.000000, -1.000000, 0.000000, // node 70
    0.500000, -0.809017, -0.309017 // node 71
};

static const unsigned int ball_elem_num = 80;

static const unsigned int ball_elems[] = {
    12, 13, 14, // elem 0
    15, 16, 17, // elem 1
    18, 19, 20, // elem 2
    21, 22, 23, // elem 3
    24, 25, 26, // elem 4
    27, 28, 29, // elem 5
    30, 31, 32, // elem 6
    33, 34, 35, // elem 7
    36, 37, 38, // elem 8
    39, 40, 41, // elem 9
    42, 43, 44, // elem 10
    45, 46, 47, // elem 11
    48, 49, 50, // elem 12
    51, 52, 53, // elem 13
    54, 55, 56, // elem 14
    57, 58, 59, // elem 15
    60, 61, 62, // elem 16
    63, 64, 65, // elem 17
    66, 67, 68, // elem 18
    69, 70, 71, // elem 19
    6, 12, 14, // elem 20
    4, 13, 12, // elem 21
    8, 14, 13, // elem 22
    9, 15, 17, // elem 23
    4, 16, 15, // elem 24
    6, 17, 16, // elem 25
    6, 18, 20, // elem 26
    3, 19, 18, // elem 27
    9, 20, 19, // elem 28
    6, 21, 23, // elem 29
    1, 22, 21, // elem 30
    3, 23, 22, // elem 31
    6, 24, 26, // elem 32
    8, 25, 24, // elem 33
    1, 26, 25, // elem 34
    8, 27, 29, // elem 35
    10, 28, 27, // elem 36
    1, 29, 28, // elem 37
    8, 30, 32, // elem 38
    0, 31, 30, // elem 39
    10, 32, 31, // elem 40
    8, 33, 35, // elem 41
    4, 34, 33, // elem 42
    0, 35, 34, // elem 43
    4, 36, 38, // elem 44
    2, 37, 36, // elem 45
    0, 38, 37, // elem 46
    4, 39, 41, // elem 47
    9, 40, 39, // elem 48
    2, 41, 40, // elem 49
    9, 42, 44, // elem 50
    11, 43, 42, // elem 51
    2, 44, 43, // elem 52
    9, 45, 47, // elem 53
    3, 46, 45, // elem 54
    11, 47, 46, // elem 55
    3, 48, 50, // elem 56
    1, 49, 48, // elem 57
    7, 50, 49, // elem 58
    1, 51, 53, // elem 59
    10, 52, 51, // elem 60
    7, 53, 52, // elem 61
    10, 54, 56, // elem 62
    0, 55, 54, // elem 63
    5, 56, 55, // elem 64
    0, 57, 59, // elem 65
    2, 58, 57, // elem 66
    5, 59, 58, // elem 67
    2, 60, 62, // elem 68
    11, 61, 60, // elem 69
    5, 62, 61, // elem 70
    3, 63, 65, // elem 71
    7, 64, 63, // elem 72
    11, 65, 64, // elem 73
    5, 66, 68, // elem 74
    11, 67, 66, // elem 75
    7, 68, 67, // elem 76
    10, 69, 71, // elem 77
    5, 70, 69, // elem 78
    7, 71, 70 // elem 78
};

#endif