#!/usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt


class LatticeGraph:
    """ A class for graphs of lattices.

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

    def __init__(self, kind="honeycomb", sites=None, unitcell=None, bcs=None):
        self.kind = kind

        # Use predefined defaults if unitcell/sites/bcs not provided
        defaults = self._get_predefined(kind)
        self.sites = sites if sites is not None else defaults["sites"]
        self.unitcell = unitcell if unitcell is not None else defaults["unitcell"]
        self.boundary_conditions = bcs if bcs is not None else defaults["bcs"]
        self.sites_total = np.prod(self.sites)
        self.sites_position = tuple()
        self.bonds = tuple()
        self.vertex_dict = dict()
        self.edge_dict = dict()

        self._init_lattice()
        self._init_graph()

    @staticmethod
    def _get_predefined(kind):
        predefined = {
            "chain": {
                "sites": (4,),
                "unitcell": [((-1,), (+1,))],
                "bcs": ("obc",),
            },
            "square": {
                "sites": (4, 4),
                "unitcell": [((0, +1), (+1, 0), (0, -1), (-1, 0))],
                "bcs": ("obc", "obc"),
            },
            "triangle": {
                "sites": (4, 4),
                "unitcell": [((0, +1), (+1, 0), (+1, +1), (0, -1), (-1, 0), (-1, -1))],
                "bcs": ("obc", "obc"),
            },
            "honeycomb": {
                "sites": (4, 4),
                "unitcell": [((0, -1), (0, +1), (+1, 0)), ((-1, 0), (0, -1), (0, +1))],
                "bcs": ("obc", "obc"),
            },
            "cube": {
                "sites": (2, 2, 2),
                "unitcell": [((+1, 0, 0), (0, +1, 0), (0, 0, +1), (-1, 0, 0), (0, -1, 0), (0, 0, -1))],
                "bcs": ("obc", "obc", "obc"),
            },
        }
        if kind not in predefined:
            raise ValueError(f"Unknown lattice kind '{kind}'. Available: {list(predefined.keys())}")
        return predefined[kind]

    def _init_lattice(self):
        """ Function for initializing the site indices for a lattice.
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
                            target[ind] = self.sites[ind] - 1
                bonds_dict[origin].add(tuple(target))
        self.sites_position = tuple(Npos.values())
        self.bonds = list()
        for origin, v in bonds_dict.items():
            for target in v:
                self.bonds.append((origin, target))
        self.bonds = tuple(self.bonds)

    def _init_graph(self, bonds=None, Npos=None):
        """ Function for initializing the vertices and edges for a graph.
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

    def plot_graph(self, show_boundary=False):
        """ Plotting the graph of the lattice with the help of rustworkx, please
            see https://www.rustworkx.org/ for further details.
        """
        import rustworkx as rx
        from rustworkx.visualization import mpl_draw

        G = rx.PyDiGraph()

        rx_index_map = {}
        pos = {}
        labels = {}
        node_colors = []

        # Determine which vertex indices are interior (original lattice) sites
        interior_indices = set(range(self.sites_total))

        for idx, coord in self.vertex_dict.items():
            rx_idx = G.add_node(idx)
            rx_index_map[idx] = rx_idx
            if len(coord) == 1:
                pos[rx_idx] = (coord[0], 0)
            elif len(coord) == 2:
                pos[rx_idx] = coord
            else:
                pos[rx_idx] = (coord[0], coord[1])
            labels[rx_idx] = str(idx)

            # Color interior sites yellow, boundary sites light red
            if idx in interior_indices:
                node_colors.append('yellow')
            else:
                node_colors.append('lightsalmon')

        for bond_idx, (src, tgt) in self.edge_dict.items():
            G.add_edge(rx_index_map[src], rx_index_map[tgt], bond_idx)

        fig, ax = plt.subplots(1, 1, figsize=(8, 6))

        mpl_draw(G,
                 pos=pos,
                 labels=lambda node: labels[node],
                 node_color=node_colors,
                 node_size=750,
                 edge_color='black',
                 arrows=True,
                 font_size=8,
                 with_labels=True,
                 ax=ax)

        # Add a legend
        import matplotlib.patches as mpatches
        interior_patch = mpatches.Patch(color='yellow', label='Lattice sites')
        boundary_patch = mpatches.Patch(color='lightsalmon', label='Boundary sites')
        ax.legend(handles=[interior_patch, boundary_patch], loc='upper right')

        ax.set_axis_off()
        plt.tight_layout()
        plt.show()
