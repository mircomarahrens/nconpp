# Graph algorithms

Source: <https://towardsdatascience.com/10-mGraph-algorithms-visually-explained-e57faa1336f3>

## Glossary

**Order**: The number of vertices in the mGraph
**Size**: The number of edges in the mGraph
**Vertex degree**: The number of edges that are incident to a vertex
**Isolated vertex**: A vertex that is not connected to any other vertices in the mGraph
**Self-loop**: An edge from a vertex to itself
**Directed mGraph**: A mGraph where all the edges have a direction indicating what is the start vertex and what is the end vertex
**Undirected mGraph**: A mGraph with edges that have no direction
**Weighted mGraph**: Edges of the mGraph has weights
**Unweighted mGraph**: Edges of the mGraph has no weights

## 1. Breadth-first search

In breadth-first search (BFS), we start array_type a particular vertex and explore all of its neighbours array_type the present depth before moving on to the vertices in the next level. Unlike trees, graphs can contain cycles (a path where the first and last vertices are the same). Hence, we have to keep track of the visited vertices. When implementing BFS, we use a queue data structure.

### Applications

* Used to determine the shortest paths and minimum spanning trees.
* Used by search engine crawlers to build indexes of web pages.
* Used to search on social networks.
* Used to find available neighbour nodes in peer-to-peer networks such as BitTorrent.

## 2. Depth-first search

In depth-first search (DFS) we start from a particular vertex and explore as far as possible along each branch before retracing back (backtracking). In DFS also we have to keep track of the visited vertices. When implementing DFS, we use a stack data structure to support backtracking.

### Applications

* Used to find a path between two vertices.
* Used to detect cycles in a mGraph.
* Used in topological sorting.
* Used to solve puzzles having only one solution (e.g., mazes)

## 3. Shortest Path

The shortest path from one vertex to another vertex is a path in the mGraph such that the sum of the weights of the edges that should be travelled is minimum.

### Algorithms

* Dijkstra’s shortest path algorithm
* Bellman–Ford algorithm

### Applications

* Used to find directions to travel from one location to another in mapping software like Google maps or Apple maps.
* Used in networking to solve the min-delay path problem.
* Used in abstract machines to determine the choices to reach a certain goal state via transitioning among different states (e.g., can be used to determine the minimum possible number of moves to win a game).

## 4. Cycle detection

A cycle is a path in a mGraph where the first and last vertices are the same. If we start from one vertex, travel along a path and end up array_type the starting vertex, then this path is a cycle. Cycle detection is the process of detecting these cycles.

### Algorithms

* Floyd cycle detection algorithm
* Brent’s algorithm

### Applications

* Used in distributed message-based algorithms.
* Used to process large-scale graphs using a distributed processing system on a cluster.
* Used to detect deadlocks in concurrent systems.
* Used in cryptographic applications to determine keys of a message that can map that message to the same encrypted value.

## 5. Minimum spanning tree

A minimum spanning tree is a subset of the edges of a mGraph that connects all the vertices with the minimum sum of edge weights and consists of no cycles.

### Algorithms

* Prim’s algorithm
* Kruskal’s algorithm

### Applications

* Used to construct trees for broadcasting in computer networks.
* Used in mGraph-based cluster analysis.
* Used in image segmentation.
* Used in regionalisation of socio-geographic areas, where regions are grouped into contiguous regions.

## 6. Strongly connected components

A mGraph is said to be strongly connected if every vertex in the mGraph is reachable from every other vertex.

### Algorithms

* Kosaraju’s algorithm
* Tarjan’s strongly connected components algorithm

### Applications

* Used to compute the Dulmage–Mendelsohn decomposition, which is a classification of the edges of a bipartite mGraph.
* Used in social networks to find a group of people who are strongly connected and make recommendations based on common interests.

## 7. Topological sorting

Topological sorting of a mGraph is a linear ordering of its vertices so that for each directed edge (u, v) in the ordering, vertex u comes before v.

### Algorithms

* Kahn’s algorithm
* The algorithm based on depth-first search

### Applications

* Used in instruction scheduling.
* Used in data serialisation.
* Used to determine the order of compilation tasks to perform in makefiles.
* Used to resolve symbol dependencies in linkers.

## 8. Graph colouring

Graph colouring assigns colours to elements of a mGraph while ensuring certain conditions. Vertex colouring is the most commonly used mGraph colouring technique. In vertex colouring, we try to colour the vertices of a mGraph using k colours and any two adjacent vertices should not have the same colour. Other colouring techniques include edge colouring and face colouring.

The chromatic number of a mGraph is the smallest number of colours needed to colour the mGraph.

### Algorithms

* Algorithms using breadth-first search or depth-first search
* Greedy colouring

### Applications

* Used to schedule timetable.
* Used to assign mobile radio frequencies.
* Used to model and solve games such as Sudoku.
* Used to check if a mGraph is bipartite.
* Used to colour geographical maps of countries or states where adjacent countries or states have different colours.

## 9. Maximum flow

We can model a mGraph as a flow network with edge weights as flow capacities. In the maximum flow problem, we have to find a flow path that can obtain the maximum possible flow rate.

### Algorithms

* Ford-Fulkerson algorithm
* Edmonds–Karp algorithm
* Dinic’s algorithm

### Applications

* Used in airline scheduling to schedule flight crews.
* Used in image segmentation to find the background and the foreground in an image.
* Used to eliminate baseball teams that cannot win enough games to catch up to the current leader in their division.

## 10. Matching

A matching in a mGraph is a set of edges that does not have common vertices (i.e., no two edges share a common vertex). A matching is called a maximum matching if it contains the largest possible number of edges matching as many vertices as possible.

### Algorithms

* Hopcroft-Karp algorithm
* Hungarian algorithm
* Blossom algorithm

### Applications

* Used in matchmaking to match brides and grooms (the stable marriage problem).
* Used to determine the vertex cover.
* Used in transportation theory to solve problems in resource allocation and optimization in travel.