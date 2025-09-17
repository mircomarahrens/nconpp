from graphviz import Digraph

def fsm_singular():
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

def mpd_nn():
    sites = 2
    states = 3

    # graph
    mpd = Digraph(
        graph_attr={
            "rankdir": "LR",
            "splines": "true",
            "color": "transparent",
            "ranksep": "1",
        },
        node_attr={
            "style": "filled",
            "color": "white",
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

    # nodes
    for _ in range(states):
        with mpd.subgraph(name="cluster_I") as child:
            for i in range(sites + 1):
                child.node(name="a" + str(i), label="0")
        with mpd.subgraph(name="cluster_I") as child:
            for i in range(sites + 1):
                child.node(name="b" + str(i), label="1")
        with mpd.subgraph(name="cluster_I") as child:
            for i in range(sites + 1):
                child.node(name="c" + str(i), label="2")

    # edges

    # invisible edges to align nodes
    for i in range(sites + 1):
        mpd.edge("a" + str(i), "a" + str(i), style="invis")
        mpd.edge("b" + str(i), "b" + str(i), style="invis")
        mpd.edge("c" + str(i), "c" + str(i), style="invis")

    # conserve state
    for i in range(sites):
        mpd.edge("a" + str(i), "a" + str(i + 1), label="I")
        mpd.edge("b" + str(i), "b" + str(i + 1), style="invis")
        mpd.edge("c" + str(i), "c" + str(i + 1), label="I")

    # changing state, next-nearest neighbor coupling (NNN)
    for i in range(sites):
        mpd.edge("a" + str(i), "c" + str(i + 1), label="D")

    # changing state, nearest neighbor coupling (NN)
    for i in range(sites):
        mpd.edge("a" + str(i), "b" + str(i + 1), label="C")
        mpd.edge("b" + str(i), "c" + str(i + 1), label="B")

    mpd.render("images/mpd_nn", format="png")
    return mpd

def mpd_nnn():
    sites = 3
    states = 4

    # graph
    mpd = Digraph(
        graph_attr={
            "rankdir": "LR",
            "splines": "true",
            "color": "transparent",
            "ranksep": "1",
        },
        node_attr={
            "style": "filled",
            "color": "white",
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

    # nodes
    for _ in range(states):
        with mpd.subgraph(name="cluster_I") as child:
            for i in range(sites + 1):
                child.node(name="a" + str(i), label="0")
        with mpd.subgraph(name="cluster_I") as child:
            for i in range(sites + 1):
                child.node(name="b" + str(i), label="1")
        with mpd.subgraph(name="cluster_I") as child:
            for i in range(sites + 1):
                child.node(name="c" + str(i), label="2")
        with mpd.subgraph(name="cluster_I") as child:
            for i in range(sites + 1):
                child.node(name="d" + str(i), label="3")

    # edges
    for i in range(sites + 1):
        mpd.edge("a" + str(i), "a" + str(i), style="invis")
        mpd.edge("b" + str(i), "b" + str(i), style="invis")
        mpd.edge("c" + str(i), "c" + str(i), style="invis")
        mpd.edge("d" + str(i), "d" + str(i), style="invis")

    for i in range(sites):
        mpd.edge("a" + str(i), "a" + str(i + 1), label="I")
        mpd.edge("b" + str(i), "b" + str(i + 1), style="invis")
        mpd.edge("c" + str(i), "c" + str(i + 1), style="invis")
        mpd.edge("d" + str(i), "d" + str(i + 1), label="I")

    for i in range(sites):
        mpd.edge("a" + str(i), "d" + str(i + 1), label="D")

    for i in range(sites):
        mpd.edge("a" + str(i), "b" + str(i + 1), label="C")
        mpd.edge("b" + str(i), "c" + str(i + 1), label="A")
        mpd.edge("c" + str(i), "d" + str(i + 1), label="B")

    mpd.render("images/mpd_nnn", format="png")
    return mpd