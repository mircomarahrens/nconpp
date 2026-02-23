#!/usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt
from nconpp import LatticeGraph


class PyLatticeGraph(LatticeGraph):
    """Python wrapper around the C++ LatticeGraph (via PyBind).

    Adds predefined lattice configurations and plotting utilities.
    """

    def __init__(self, kind="honeycomb", sites=None, unitcell=None, bcs=None):
        self.kind = kind

        # Use predefined defaults if not provided
        defaults = self._get_predefined(kind)
        sites = sites if sites is not None else defaults["sites"]
        unitcell = unitcell if unitcell is not None else defaults["unitcell"]
        bcs = bcs if bcs is not None else defaults["bcs"]

        # Call the C++ LatticeGraph constructor
        super().__init__(kind, sites, unitcell, bcs)

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

    @property
    def vertex_dict(self):
        """Return {index: coordinate} dict from C++ vertices."""
        return {k: tuple(v.coordinate) for k, v in self.vertices.items()}

    @property
    def edge_dict(self):
        """Return {index: (source, target)} dict from C++ edges."""
        return {k: (e.src, e.dest) for k, e in self.edges.items()}

    @property
    def interior_indices(self):
        """Return set of vertex indices that are not boundary sites."""
        indices = set()
        for k, v in self.vertices.items():
            if not v.boundary:
                indices.add(k)
        return indices

    def plot_graph(self):
        """Plotting the graph of the lattice with the help of rustworkx.

        See https://www.rustworkx.org/ for further details.
        """
        import rustworkx as rx
        from rustworkx.visualization import mpl_draw
        import matplotlib.patches as mpatches

        G = rx.PyDiGraph()

        rx_index_map = {}
        pos = {}
        labels = {}
        node_colors = []

        interior = self.interior_indices
        vdict = self.vertex_dict

        for idx, coord in vdict.items():
            rx_idx = G.add_node(idx)
            rx_index_map[idx] = rx_idx
            if len(coord) == 1:
                pos[rx_idx] = (coord[0], 0)
            elif len(coord) == 2:
                pos[rx_idx] = coord
            else:
                pos[rx_idx] = (coord[0], coord[1])
            labels[rx_idx] = str(idx)

            if idx in interior:
                node_colors.append('yellow')
            else:
                node_colors.append('lightsalmon')

        edict = self.edge_dict
        for bond_idx, (src, tgt) in edict.items():
            G.add_edge(rx_index_map[src], rx_index_map[tgt], bond_idx)

        fig, ax = plt.subplots(1, 1, figsize=(8, 6))

        mpl_draw(G,
                 pos=pos,
                 labels=lambda node: labels[node],
                 node_color=node_colors,
                 node_size=750,
                 edge_color='black',
                 arrows=False,
                 font_size=8,
                 with_labels=True,
                 ax=ax)

        interior_patch = mpatches.Patch(color='yellow', label='Lattice sites')
        boundary_patch = mpatches.Patch(color='lightsalmon', label='Boundary sites')
        ax.legend(handles=[interior_patch, boundary_patch], loc='upper right')

        ax.set_axis_off()
        plt.tight_layout()
        plt.show()

    def __repr__(self):
        return (f"PyLatticeGraph(kind='{self.kind}', "
                f"vertices={len(self.vertices)}, edges={len(self.edges)})")
