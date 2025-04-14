#!/usr/bin/python3

import networkx as nx

import numpy as np
import matplotlib.pyplot as plt


class LatticeGraph:
    """A class for graphs of lattices.

    This can be seen as a mixture of lattices and graphs, and therefore we
    have a mixture in the nomenclature of both fields.

    We construct lattices as graphs to achieve a solid framework where each
    constituent has a fixed value. We enumerate sites, bonds, boundary
    conditions (lattice) and setting up vertices and edges (graph) for them.

    The default values are describing a 4x4 honeycomb latticegraph with
    open boundary conditions in both spatial direction:

                    (0, 5)  (1, 5)  (2, 5)  (3, 5)
                      |       |       |       |
                    (0, 4)--(1, 4)  (2, 4)--(3, 4)
                      |       |       |       |
           (-1, 3)--(0, 3)  (1, 3)--(2, 3)  (3, 3)--(4, 3)
                      |       |       |       |
                    (0, 2)--(1, 2)  (2, 2)--(3, 2)
                      |       |       |       |
           (-1, 2)--(0, 1)  (1, 1)--(2, 1)  (3, 1)--(4, 1)
                      |       |       |       |
        y           (0, 0)--(1, 0)  (2, 0)--(3, 0)
        ^             |       |       |       |
        |           (0,-1)  (1,-1)  (2,-1)  (3,-1)
        +-->x

    Args:
        sites (tuple): see Attributes
        unitcell (list): see Attributes
        bcs (tuple): boundary_conditions, see Attributes

    Attributes:
        sites (tuple): the number of sites spatial distributed, e.g. for d=2
            we can have (Nx, Ny), where Nx, Ny are integers.
        sites_position (tuple): each site has a fix index in this tuple and
            each element to this index is a point in a coordinate system.
        sites_total (tuple): the total number of sites.
        boundary_conditions (tuple): the boundary condition per spatial
            direction.
        unitcell (list): a list of tuples. This list set up the bonds of our
            graph. Therefore, each entry corresponds to a site in a
            "unit cell" and each element in this entries is again a tuple
            which represents an additive factor.
            Any site in the lattice is connected to a site in this list.
            We call the site of interest the origin and the site connected
            to the origin the target. We can reach the target sites by
            adding the elements to the origin.
            This pairs of (origin, target) are forming the bonds.
        bonds (tuple): pairs of (origin, target) resulted from the
            "unit cell". Again each entry of this tuple is on a fixed index
            position and can therefore be used as enumeration for the bonds.
        vertex_dict:
        edge_dict:

    Methods:
        _init_lattice, _init_graph: see docstrings in methods for details.

    Todo:
        * implement transformation to bravais lattice
    """

    def __init__(
        self,
        sites=(4, 4),
        unitcell=[((0, -1), (0, +1), (+1, 0)), ((-1, 0), (0, -1), (0, +1))],
        bcs=("obc", "obc"),
    ):
        self.sites = sites
        self.sites_position = tuple()
        self.sites_total = np.prod(sites)
        self.boundary_conditions = bcs
        self.unitcell = unitcell
        self.bonds = tuple()
        self.vertex_dict = dict()
        self.edge_dict = dict()

        self._init_lattice()
        self._init_graph()

    def _init_lattice(self):
        """Function for initializing the site indices for a lattice.
        This function writes the attributes sites_position and bonds.
        """
        Npos = {}
        bonds_dict = {}
        for i in range(self.sites_total):  # i = 0, 1, 2, ...
            # e.g. d=2: ni = (0,0), ...
            origin = np.unravel_index(i, self.sites)
            Npos[i] = origin  # {0: (0,0), 1: (0,1), ...}
            bonds_dict[origin] = set()
            for uc_site in self.unitcell[sum(origin) % len(self.unitcell)]:
                target = np.add(origin, uc_site)
                for ind, bc in enumerate(self.boundary_conditions):
                    if bc == "pbc":
                        if target[ind] >= self.sites[ind]:
                            target[ind] = 0
                        if target[ind] < 0:
                            target[ind] = self.sites[ind]
                bonds_dict[origin].add(tuple(target))
        self.sites_position = tuple(Npos.values())
        self.bonds = list()
        for origin, v in bonds_dict.items():
            for target in v:
                self.bonds.append((origin, target))
        self.bonds = tuple(self.bonds)

    def _init_graph(self, bonds=None, Npos=None):
        """Function for initializing the vertices and edges for a graph.
        This function writes the attributes vertex_dict and edge_dict.
        """
        if bonds is not None:
            self.bonds = bonds
        if Npos is not None:
            self.sites_position = Npos
        self.vertex_dict = dict(var for var in enumerate(self.sites_position))
        vertex_reverse = {v: k for k, v in self.vertex_dict.items()}
        self.edge_dict = {}
        edge_count = 0
        for bond in self.bonds:
            for site in bond:
                if site not in self.vertex_dict.values():
                    key = max(self.vertex_dict.keys()) + 1
                    self.vertex_dict[key] = site
                    vertex_reverse[site] = key
            edge = tuple((vertex_reverse[bond[0]], vertex_reverse[bond[1]]))
            if edge not in self.edge_dict.values():
                self.edge_dict[edge_count] = edge
                edge_count += 1

    def _predefined_latticegraph(self, lattice):
        if lattice == "chain":
            self.dim = 1
            if self.N is None:
                self.N = (2,)
            self.ucl_list = [(-1), (+1)]
        elif lattice == "square":
            self.dim = 2
            if self.N is None:
                self.N = (
                    2,
                    2,
                )
            self.ucl_list = [(0, +1), (+1, 0), (0, -1), (-1, 0)]
        elif lattice == "triangle":
            self.dim = 2
            if self.N is None:
                self.N = (
                    2,
                    2,
                )
            self.ucl_list = [(0, +1), (+1, 0), (+1, +1), (0, -1), (-1, 0), (-1, -1)]
        elif lattice == "honeycomb":
            self.dim = 2
            if self.N is None:
                self.N = (
                    2,
                    2,
                )
            self.ucl_list = [((0, -1), (0, +1), (+1, 0)), ((-1, 0), (0, -1), (0, +1))]
        elif lattice == "cube":
            self.dim = 3
            if self.N is None:
                self.N = (
                    2,
                    2,
                    2,
                )
            self.ucl_list = [
                (+1, 0, 0),
                (0, +1, 0),
                (0, 0, +1),
                (-1, 0, 0),
                (0, -1, 0),
                (0, 0, -1),
            ]
        elif lattice == "kagome":
            raise Exception("Not implemented yet.")
        elif lattice == "archimedean":
            # TODO we should implement all the lattices in archimedean notation
            raise Exception("Not implemented yet.")
        else:
            self.predefined = False

    def plot_graph(self, show_boundary=False):
        """Plotting the graph of the lattice with the help of networkx, please
        see https://networkx.github.io/ for further details.
        """
        G = nx.DiGraph(directed=True)
        # pos=nx.spring_layout(G, pos=self.vertex_dict.copy())
        pos = self.vertex_dict.copy()

        # nodes
        nodes = list(self.vertex_dict.keys())
        G.add_nodes_from(nodes)
        nx.draw_networkx_nodes(
            G, pos, nodelist=nodes, node_color="y", node_size=750, alpha=1.0
        )

        # edges
        edges = list(self.edge_dict.values())
        G.add_edges_from(edges)
        nx.draw_networkx_edges(
            G,
            pos,
            node_size=750,
            edgelist=edges,
            width=1,
            alpha=1.0,
            arrows=True,
            arrowstyle="->",
            arrowsize=10,
            edge_color="black",
        )

        # labels
        labels = {}
        for label in self.vertex_dict:
            labels[label] = str(self.vertex_dict[label])
        nx.draw_networkx_labels(G, pos, labels, font_size=8)

        plt.axis("off")
        plt.show()

    # # TODO
    # def plot_bravais(self, vertex_dict, edge_dict, b=1.0):
    #     a = np.sqrt(3)*b
    #     bravais_dict = {}
    #     j = 0
    #     a1 = np.multiply(a, np.array([1.0, 0.0]))
    #     a2 = np.multiply(a, np.array([0.5, 0.5*np.sqrt(3)]))
    #     for i in vertex_dict.items():
    #         i1 = i[1][0]
    #         i2 = i[1][1]
    #         bravais_dict[j] = tuple(
    #             np.add(np.multiply(i1, a1), np.multiply(i2, a2)))
    #         j += 1

    #     B = nx.Graph()
    #     pos = bravais_dict.copy()

    #     # nodes
    #     nodes = list(bravais_dict.keys())
    #     B.add_nodes_from(nodes)
    #     nx.draw_networkx_nodes(B, pos,
    #                            nodelist=nodes,
    #                            node_color='y',
    #                            node_size=750,
    #                            alpha=1)

    #     # edges
    #     edges = list(edge_dict.values())
    #     nx.draw_networkx_edges(B, pos,
    #                            edgelist=edges,
    #                            width=1,
    #                            alpha=1,
    #                            edge_color='b')

    #     # edges
    #     labels = {}
    #     for label in bravais_dict:
    #         labels[label] = str(label)
    #     nx.draw_networkx_labels(B, pos,
    #                             labels,
    #                             font_size=8)

    #     plt.axis("off")
    #     plt.show()
