import numpy as np
import sympy as sp

from graphviz import Digraph


class Tennet:
    def __init__(self):
        self.symbols = {}
        pass
    
    def mpd(self):
        pass

    def mps(self):
        pass
    
    def symbols():
        pass

    def fsm():

        FSM = Digraph(
            graph_attr={
                'rankdir':'LR',
                'splines':'true',
                'color':'transparent',
                'ranksep':'1'
            },
            node_attr={
                'style':'filled',
                'color':'white',
                'shape':'circle',
                'height':'0.1',
                'fillcolor':'lightgray',
                'fontsize':'10'
            },
            edge_attr={
                "arrowsize": "0.5",
                "fontsize": "8",
            },
        )

        FSM.node(name='0', label='S')
        FSM.node(name='1', label='T')
        FSM.node(name='2', label='F')

        FSM.edge('0','0', label='I')
        FSM.edge('0','1', label='C')
        FSM.edge('1','1', label='A')
        FSM.edge('1','2', label='B')
        FSM.edge('0','2', label='D')
        FSM.edge('2','2', label='I')

        FSM.render('images/fsm', format='png')

        return FSM
