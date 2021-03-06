import math
import numpy as np
import matplotlib.pyplot as plt

class CircleMesh:
    def __init__(self):
        # solid circle
        self.node_num = 0
        self.nodes = None
        self.elem_num = 0
        self.elems = None
        # circle edge
        self.edge_node_num = 0
        self.edge_nodes = None
        self.edge_num = 0
        self.edges = None
    
    def gen_mesh(self, inv_num):
        # nodes
        self.node_num = inv_num + 1
        self.nodes = np.zeros([self.node_num, 2], dtype = np.float32)
        node0 = self.nodes[0]
        node0[0] = 0.0
        node0[1] = 0.0
        dangle = 2.0 * math.pi / float(inv_num)
        for n_id in range(1, self.node_num):
            node = self.nodes[n_id]
            ang = float(n_id-1) * dangle
            node[0] = math.cos(ang)
            node[1] = math.sin(ang)
        # elems
        self.elem_num = inv_num
        self.elems = np.zeros([self.elem_num, 3], dtype = np.uint32)
        for e_id in range(self.elem_num):
            elem = self.elems[e_id]
            elem[0] = 0
            elem[1] = e_id + 1
            elem[2] = e_id + 2
        self.elems[self.elem_num-1][2] = 1
    
    def gen_hexagon(self):
        self.gen_mesh(6)
    
    def subdivide(self, other):
        pass # to be finished ...
        # self.node_num = other.node_num + other.edge_num
        # self.nodes = np.zeros([self.node_num, 2], dtype=np.float32)
        # self.elem_num = other.elem_num * 3
        # self.elems = np.zeros([self.elem_num, 3], dtype=np.uint32)
    
    def write_elems_to_cfile(self, filename):
        with open(filename, "w") as cfile:
            cfile.write("""#ifndef __Circle_Mesh_Data__
#define __Circle_Mesh_Data__

// Generated by CircleMesh.py

static const unsigned int circle_node_num = %d;

static const float circle_nodes[] = {
""" % self.node_num)
            
            for n_id in range(self.node_num-1):
                node = self.nodes[n_id]
                cfile.write("    %ff, %ff, // node %d\n" % (node[0], node[1], n_id))
            node = self.nodes[self.node_num-1]
            cfile.write("    %ff, %ff // node %d\n};" % (node[0], node[1], self.node_num-1))
            
            cfile.write("""

static const unsigned int circle_elem_num = %d;

static const unsigned int circle_elems[] = {
""" % self.elem_num)

            for e_id in range(self.elem_num-1):
                elem = self.elems[e_id]
                cfile.write("    %d, %d, %d, // elem %d\n" % (elem[0], elem[1], elem[2], e_id))
            elem = self.elems[self.elem_num-1]
            cfile.write("    %d, %d, %d // elem %d\n};" % (elem[0], elem[1], elem[2], self.elem_num-1))

            cfile.write("""

#endif""")
    
    def display(self):
        fig = plt.figure()
        plot1 = fig.subplots(1, 1)
        plot1.set_xlabel('x')
        plot1.set_ylabel('y')
        x = np.zeros(self.node_num)
        y = np.zeros(self.node_num)
        for n_id in range(self.node_num):
            node = self.nodes[n_id]
            x[n_id] = node[0]
            y[n_id] = node[1]
        plot1.plot(x, y)
        plt.show()

        
    def gen_edge(self, inv_num):
        # nodes
        self.edge_node_num = inv_num
        self.edge_nodes = np.zeros([self.edge_node_num, 2], dtype = np.float32)
        dangle = 2.0 * math.pi / float(inv_num)
        for n_id in range(self.edge_node_num):
            node = self.edge_nodes[n_id]
            ang = float(n_id) * dangle
            node[0] = math.cos(ang)
            node[1] = math.sin(ang)
        # elems
        self.edge_num = inv_num
        self.edges = np.zeros([self.edge_num, 2], dtype = np.uint32)
        for e_id in range(self.edge_num):
            edge = self.edges[e_id]
            edge[0] = e_id
            edge[1] = e_id + 1
        self.edges[self.edge_num-1][1] = 0
    
    def write_edges_to_cfile(self, filename):
        with open(filename, "w") as cfile:
            cfile.write("""#ifndef __Circle_Edges_Data__
#define __Circle_Edges_Data__

// Generated by CircleMesh.py

static const unsigned int circle_node_num = %d;

static const float circle_nodes[] = {
""" % self.edge_node_num)
            
            for n_id in range(self.edge_node_num-1):
                node = self.edge_nodes[n_id]
                cfile.write("    %ff, %ff, // node %d\n" % (node[0], node[1], n_id))
            node = self.edge_nodes[self.edge_node_num-1]
            cfile.write("    %ff, %ff // node %d\n};" % (node[0], node[1], self.edge_node_num-1))
            
            cfile.write("""

static const unsigned int circle_edge_num = %d;

static const unsigned int circle_edges[] = {
""" % self.edge_num)

            for e_id in range(self.edge_num-1):
                edge = self.edges[e_id]
                cfile.write("    %d, %d, // edge %d\n" % (edge[0], edge[1], e_id))
            edge = self.edges[self.edge_num-1]
            cfile.write("    %d, %d // edge %d\n};" % (edge[0], edge[1], self.edge_num-1))

            cfile.write("""

#endif""")

if __name__ == "__main__":
    circle0 = CircleMesh()
    #circle0.gen_mesh(60)
    #circle0.gen_hexagon()
    #circle0.display()
    # circle0.write_elems_to_cfile("circle_mesh_data.h")
    circle0.gen_edge(360)
    circle0.write_edges_to_cfile("circle_edges_data.h")