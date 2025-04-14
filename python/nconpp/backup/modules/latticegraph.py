import numpy as np

class LATTICEGRAPH:
    """ ========================================================================
        Parent class for lattice construction.
        ========================================================================
        Key             |  Default        | Description
        ----------------+-----------------+-------------------------------------
        name            | chain           | a free name for the lattice
        N               | 2               | number of sites
        Nd              | (2,)            | lattice shape, index dimension
        bcs             | ("pbc",)        | boundary condition per direction
        sl_list         | [(-1,+1,)]      | site legs in unit cell
        Npos_dict       | None            | multi index site positions with
                        |                 |  k:v = site: coordinates
        bond_set        | None            | set for bond connections
        bond_dict       | None            | dict for bond connections with
                        |                 |  k:v = coordinates: 
                        |                 |  {other coordinates}
        vertex_dict     | None            | dict for graph vertices with 
                        |                 | k:v = vertex: site coordinates 
        vertex_set      | None            | vertex_dict.keys()
        edge_dict       | None            | dict for graph edges with
                        |                 | k:v = edge: (vertex1, vertex2)
        edge_set        | None            | edge_dict.values()
        bi_edge_dict    | None            | dict for bidirected graph edges with
                        |                 | k:v = edge: (vertex1, vertex2)
        bi_edge_set     | None            | bi_edge_dict.values()
        ========================================================================
    """

    def __init__(self, 
        name="chain", 
        sites=2, 
        spread=(2,), 
        boundaries=("pbc",), 
        sitelegs=[(-1,+1)]):
        self.name = name         # lattice name, e.g. triangle, square, honeycomb, chain, cube, whatever you want. It is just a name.
        self.N = sites           # number of sites
        self.Nd = spread         # max possible spatial expansiveness
        self.bcs = boundaries    # boundary condition
        self.sl_list = sitelegs  # legs of sites in unit cell
        self.Npos_dict = None    # dict of site coordinates
        self.bond_set = None     # set for bond connections
        self.bond_dict = None    # dict for bond connections
        self.vertex_dict = None  # dict for graph vertices
        self.vertex_set = None   # set of vertices
        self.edge_dict = None    # dict graph edges
        self.edge_set = None     # set of edges
        self.bi_edge_dict = None # dict of bidirectional edges
        self.bi_edge_set = None  # set of bidirectional edges


    def init_lattice(self):
        self._init_Npos_dict()
        self.bond_set = set()
        self.bond_dict = dict((self._unravel(mi, self.Nd), set()) for mi in range(self.N))
        temp_bond_list = []
        for i in range(self.N):
            for l in self.sl_list[i%len(self.sl_list)]:
                ml = np.multiply(np.sign(l),self._unravel(abs(l), self.Nd))
                mi = self._unravel(i, self.Nd)
                mj = np.add(mi,ml)
                for bc in self.bcs:
                    if bc == 'pbc':
                        ind = self.bcs.index(bc)
                        if mj[ind] > self.Nd[ind]:
                            mj[ind] = 0
                        if mj[ind] < 0:
                            mj[ind] = self.Nd[ind]
                mj = tuple(mj)
                temp_bond_list.append([mi,mj])
                self.bond_dict[mi].add(mj)
        self.bond_set = set(tuple(sorted(l)) for l in temp_bond_list)
    
    def init_graph(self):
        # prepare temp
        temp_list = []; temp_dict = {}
        for b in self.bond_set:
            for i in b:
                temp_list.append(i)
        temp_list = list(set(temp_list))
        for ind, j in enumerate(temp_list):
            temp_dict[j] = ind
        # init edge dict
        self.edge_dict = {}
        for ind, b in enumerate(self.bond_set):
            i = b[0]; j = b[1]
            self.edge_dict[ind] = (temp_dict[i], temp_dict[j])
        # init vertex dict
        self.vertex_dict = {}; 
        for ind, j in enumerate(temp_list):
            self.vertex_dict[ind] = j
        # init bidirections
        self.bi_edge_dict = {}
        for k in self.edge_dict:
            cedge = list(self.edge_dict[k]) 
            cedge[0], cedge[1] = cedge[1], cedge[0] 
            self.bi_edge_dict[k+len(self.edge_dict)] = tuple(cedge)
        self.bi_edge_dict.update(self.edge_dict)
        # init sets
        self.bi_edge_set = self.bi_edge_dict.values()
        self.vertex_set = set(self.vertex_dict.keys())
        self.edge_set = set(self.edge_dict.values())


    def _init_Npos_dict(self):
        self.Npos_dict = []
        for i in range(self.N):
            mi = self._unravel(i, self.Nd)
            self.Npos_dict.append(mi)
        return self.Npos_dict

    def _ravel(self, index, index_dims):
        """ Wrapper function for numpy.ravel_multi_index(). """
        # (i,j) -> k
        return np.ravel_multi_index(index, index_dims)
    
    def _unravel(self, index, index_dims):
        """ Wrapper function for numpy.unravel(). """
        # (i,j) <- k
        return np.unravel_index(index, index_dims)