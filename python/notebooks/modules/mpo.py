from __future__ import annotations
import itertools
import numpy as np
import sympy as sp


class FSM:
    def __init__(self):
        self.start = 0
        self.states = {self.start: "S"}  # Q with initial state S
        self.symbols = dict()  # Sigma
        self.transitions = dict()  # delta
        self.end = int()  # single-accept state, |F|=1
        self.table = self._construct_table()

    def add_state(self, state: int, label: str = "", is_final: bool = False):
        """
        Adds a state to the finite state machine.

        Args:
            state (int): The state to be added.
            label (str, optional): The label for the state. Defaults to an empty string.
            is_final (bool, optional): Indicates if the state is an end state. Defaults to False.

        Raises:
            ValueError: If the end state already exists or if the state already exists.

        Returns:
            None
        """
        if is_final:
            if self.end:
                raise ValueError("End state already exists")
            self.end = state

        if state not in self.states:
            self.states[state] = label
        else:
            raise ValueError("State already exists")

        self.table = self._construct_table()

    def add_transition(self, pair: tuple[int, int], label: str):
        """
        Adds a transition to the finite state machine.

        Args:
            pair (tuple[int, int]): A tuple representing the pair of states for the transition.
            label (str): The label associated with the transition.

        Raises:
            ValueError: If either state in the pair does not exist in the finite state machine.

        Returns:
            None
        """
        if pair[0] in self.states:
            if pair[1] in self.states:
                self.transitions[pair] = label
            else:
                raise ValueError("State {val} does not exist".format(val=pair[1]))
        else:
            raise ValueError("State {val} does not exist".format(val=pair[0]))

        self.table = self._construct_table()

    def add_transitions(self, transitions: tuple[str, str]):
        return "Not implemented yet"

    def _construct_table(self) -> np.matrix:
        """
        Constructs a transition table for the finite state machine.

        Returns:
            np.ndarray: A 2D numpy array representing the transition table.
        """
        table = np.matrix(np.full((len(self.states), len(self.states)), "0", dtype=str))

        for pair, label in self.transitions.items():
            table[pair] = label

        return table

    def get_table(self):
        """
        Returns the table associated with the FSM.

        Returns:
            table (Table): The table associated with the FSM.
        """
        return self.table

    def draw(self):
        """
        Draws the finite state machine using Graphviz.
        """
        from graphviz import Digraph

        drawing = Digraph(
            graph_attr={
                "rankdir": "LR",
                "splines": "true",
                "color": "transparent",
                "ranksep": "1",
            },
            node_attr={
                "style": "filled",
                # 'color':'white',
                "shape": "circle",
                "height": "0.1",
                "fillcolor": "lightgray",
                "fontsize": "10",
            },
            edge_attr={
                "arrowsize": "0.5",
                "fontsize": "8",
            },
        )

        for state, label in self.states.items():
            if state == self.end or state == self.start:
                drawing.node(str(state), label, shape="doublecircle")
            else:
                drawing.node(str(state), label)

        for transition in self.transitions:
            drawing.edge(
                str(transition[0]),
                str(transition[1]),
                label=self.transitions[transition],
            )

        drawing.render("images/fsm", format="png")

        return drawing

    def clear(self):
        self.states = {self.start: "S"}
        self.transitions = dict()
        self.end = int()
        self.table = self._construct_table()


class MPO(FSM):
    def __init__(self, local_dim=2):
        super().__init__()
        self.local_dim = local_dim
        self.symbols = dict()
        self.site = sp.Symbol("i", real=True)
        self.sites = int()

    def default(self):
        self.sites = 2

        self.add_state(1, "T")
        self.add_state(2, "F", True)

        self.add_transition((0, 0), "I")
        self.add_transition((0, 1), "B")
        self.add_transition((1, 2), "C")
        self.add_transition((0, 2), "D")
        self.add_transition((1, 1), "A")
        self.add_transition((2, 2), "I")

    def add_transition(self, pair, label):
        if label not in self.symbols:
            self.symbols[label] = sp.MatrixSymbol(label, self.local_dim, self.local_dim)
        if pair in self.transitions:
            if self.transitions[pair] == label:
                raise ValueError("Transition already exists")
        return super().add_transition(pair, label)

    def __getitem__(self, key) -> str:
        return self.table[key]

    def __setitem__(self, key, value):
        self.table[key] = value

    def __str__(self):
        return self.table.__str__()

    def __matmul__(self, other) -> MPO:
        if not isinstance(other, MPO):
            raise ValueError("Operand must be an instance of class MPO")

        result = MPO()

        if self.site != other.site:
            # matmul elements
            pass
        result = self.get_symbolic_table() @ other.get_symbolic_table()

        # for i in range(len(self.states)):
        #     for j in range(len(self.states)):
        #         ev = ''
        #         for k in range(len(self.states)):
        #             if self.table[i, k] == '0' or other.table[k, j] == '0':
        #                 val = ''
        #             else:
        #                 val = self.table[i, k] + other.table[k, j]
        #                 if ev == '':
        #                     ev = val
        #                 else:
        #                     ev = ev + '+' + val
        #         print(ev)
        #         result[i, j] = ev

        return result

    def get_symbolic_table(self):
        """
        Returns a symbolic table representing the transitions between states.

        Input:
            local_dim: Local dimension for symbolic matrices.

        Returns:
            table (SymPy Matrix): A symbolic table with dimensions len(self.states) x len(self.states),
                                    where each element represents a transition between two states.
        """
        table = sp.zeros(len(self.states), len(self.states))
        for pair, label in self.transitions.items():
            table[pair] = sp.MatrixSymbol(label, self.local_dim, self.local_dim)
            self.symbols[label] = table[pair]

        indices = [i for i in range(len(self.states))]
        perm = set(itertools.product(indices, indices))

        diff = set(self.transitions.keys()).symmetric_difference(perm)
        for pair in diff:
            table[pair] = 0
        return table

    def get_site_symbol(self) -> str:
        """
        Returns the symbolic site index used in the MPO.
        """
        return self.site
