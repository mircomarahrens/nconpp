import numpy as np

class LATTICE():
    """
    Parent class for lattice construction by using a bidirectional graph.

    From https://en.wikipedia.org/wiki/Lattice_graph:
    "A lattice [...] is a graph whose drawing, embedded in some Euclidean space
     R^n, forms a regular tiling."

    Attributes:
        name (str):
            a free name for the lattice
        Ntot (int):
            number of sites
        N (tuple(int,)):
            "lattice shape", spatial lattice distribution
        bcs (tuple(str,)):
            boundary condition per direction
        ucl_list (list(int,)):
            periodic site legs aka "unit cell"
        Npos_dict (dict(int : (int,),)):
            multi index site positions with
            k:v = site: coordinates,
        bond_dict (dict((int,):(int,))):
            dict for bond connections with
            k:v = coordinates: {other coordinates}
        vertex_dict:
            dict for graph vertices with
            k:v = vertex: site coordinates
        edge_dict:
            dict for graph edges with
            k:v = edge: (vertex1, vertex2)
        bi_edge_dict:
            dict for bidirected graph edges with
            k:v = edge: (vertex1, vertex2)

    Methods:
        init_lattice():

    """

    def __init__(self):
        self.lattice = "chain"
        self.lattice_cases = \
            {"chain", "square", "honeycomb", "triangle"}
        self.Ntot = 2
        self.bcs = ("pbc",)
        self.ucl_list = [(+1,)]
        self.dim = 1
        self.N = (2,)
        self.Npos_dict = dict()
        self.bond_dict = dict()
        self.vertex_dict = dict()
        self.edge_dict = dict()
        self.bi_edge_dict = None
        self.init_lattice()



    def init_lattice(self):
        for i in range(self.Ntot):
            ni = np.unravel_index(i, N)
            self.bond_dict[ni] = set()
            self.Npos_dict[i] = ni
            for l in ucl_list[i%len(ucl_list)]:
                nj = np.add(ni, l)
                for ind, bc in enumerate(self.bcs):
                    if bc == "pbc":
                        if nj[ind] >= N[ind]:
                            nj[ind] = 0
                        if nj[ind] < 0:
                            nj[ind] = N[ind]
                self.bond_dict[ni].add(tuple(nj))

    def init_graph(self):
        self.vertex_dict = self.Npos_dict
        for be in self.bond_dict.values():
            for e in be:
                if e not in self.vertex_dict.values():
                    vertex_dict[max(vertex_dict.keys())+1] = e
################################################################################

def init_lattice(N = (2,3), uc_list = [((0,-1), (0,+1), (+1,0)), ((-1,0), (0,-1), (0,+1))], bcs = ("obc", "ob
    ...: c")):
    ...:     Ntot = np.prod(N)
    ...:     bond_dict = {}
    ...:     Npos_dict = {}
    ...:     for i in range(Ntot):
    ...:         ni = np.unravel_index(i, N)
    ...:         bond_dict[ni] = set()
    ...:         Npos_dict[i] = ni
    ...:         for l in uc_list[i%len(uc_list)]:
    ...:             nj = np.add(ni, l)
    ...:             for ind, bc in enumerate(bcs):
    ...:                 if bc == "pbc":
    ...:                     if nj[ind] >= N[ind]:
    ...:                         nj[ind] = 0
    ...:                     if nj[ind] < 0:
    ...:                         nj[ind] = N[ind]
    ...:             bond_dict[ni].add(tuple(nj))
    ...:     return bond_dict, Npos_dict

        # prep temps
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



    def init_lattice(self):
        """ Initilize the lattice structure.
        """
        # check if a predefined lattice structure wants to be used
        self._predefined_lattice()
        # abbreviations
        N = self.N; Ntot = self.sites; ucl_list = self.ucl_list
        bcs = self.boundary_conditions
        # initilize site positions
        self.Npos_dict = dict((i, np.unravel_index(i, self.N)) \
            for i in range(self.Ntot))
        Npos_dict = self.sitepos_dict;
        # initilize class attributes
        self.bond_set = set(); temp_list = []
        # initilize key:values of bond_dict, where keys are the current site
        # coordinate and values are all sites connected to this
        # 1. init keys
        self.bond_dict = dict((self._unravel(mi, Nd), set()) for mi in range(N))
        # add sites for boundary conditions if needed
        for ind,bc in enumerate(bcs):
            if bc == 'obc':

                self.bond_dict[""]
        # 2. construct the lattice structure, aka bond_list
        for i in range(N):
            for l in ucl_list[i%len(ucl_list)]:
################################################################################
                ml = np.multiply(np.sign(l), self._unravel(abs(l), Nd))
                mi = self._unravel(i, Nd)
                mj = np.add(mi,ml)
                for bc in bcs:
                    if bc == 'pbc':
                        ind = bcs.index(bc)
                        if mj[ind] > Nd[ind]:
                            mj[ind] = 0
                        if mj[ind] < 0:
                            mj[ind] = Nd[ind]
                mj = tuple(mj)
                temp_list.append([mi,mj])
                self.bond_dict[mi].add(mj)
        # write the bond set
        self.bond_set = set(tuple(sorted(l)) for l in temp_list)
        # prep temps
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
