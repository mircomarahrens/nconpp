//
// Created by mirco on 3/13/2023.
//

#ifndef NCONPP_GRAPH_H
#define NCONPP_GRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/properties.hpp>

class Graph {
    typedef boost::adjacency_list
            <
                    boost::vecS,
                    boost::vecS,
                    boost::undirectedS
            > boost_Graph;
public:
    /**
     * Default constructor.
     */
    explicit Graph() = default;

    /**
     * Constructor for a graph with n nodes.
     * @param N
     */
    explicit Graph(std::size_t N) : g(N) {}

    /**
     * Add a new vertex to the graph.
     */
    std::size_t addVertex() {
        auto v = boost::add_vertex(g);
        return v;
    }

    /**
     * Remove a vertex from the graph.  All edges corresponding to the vertex are removed beforehand.
     * @param index
     */
    void removeVertex(std::size_t index) {
        auto v = boost::vertex(index, g);
        boost::clear_vertex(v, g); // ensure all edges to vertex are removed beforehand
        boost::remove_vertex(v, g);
    }

    /**
     * Remove all vertices from the graph. All edges corresponding to the vertex are removed beforehand.
     */
    void removeAllVertices() {
        boost::graph_traits<boost_Graph>::vertex_iterator vi, vi_end, next;
        boost::tie(vi, vi_end) = boost::vertices(g);
        for (next = vi; vi != vi_end; vi = next) {
            ++next;
            boost::clear_vertex(*vi, g); // ensure all edges to vertex are removed beforehand
            boost::remove_vertex(*vi, g);
        }
    }

    /**
     * Retrieve all current vertex indices.
     * @return
     */
    std::vector<std::size_t> getVertices() {
        std::vector<std::size_t> result = {};

        // get the property map for vertex id
        typedef boost::property_map<boost_Graph, boost::vertex_index_t>::type VertexIdMap;
        VertexIdMap vertex_id = boost::get(boost::vertex_index, g);

        // tie vertex iterators
        boost::graph_traits<boost_Graph>::vertex_iterator vi, vi_end, next;
        boost::tie(vi, vi_end) = boost::vertices(g);

        for (next = vi; vi != vi_end; vi = next) {
            ++next;
            result.emplace_back(vertex_id[*vi]);
        }

        return std::move(result);
    }

    /**
     * Add a new edge to the graph.
     * @param src
     * @param dest
     */
    std::pair<std::size_t, std::size_t> addEdge(std::size_t src, std::size_t dest) {
        auto s = boost::vertex(src, g);
        auto d = boost::vertex(dest, g);
        auto e = boost::add_edge(s, d, g);
        assert(e.second == true);
        return std::make_pair(s, d);
    }

    /**
     * Remove an edge between source and destination vertices.
     * @param src
     * @param dest
     */
    void removeEdge(std::size_t src, std::size_t dest) {
        auto s = boost::vertex(src, g);
        auto d = boost::vertex(dest, g);
        boost::remove_edge(s, d, g);
    }

    /**
     * Retrieve all edges as pair of vertex indices.
     * @return
     */
    std::vector<std::pair<std::size_t, std::size_t>> getEdges() {
        std::vector<std::pair<std::size_t, std::size_t>> result = {};

        // get the property map for vertex id
        typedef boost::property_map<boost_Graph, boost::vertex_index_t>::type VertexIdMap;
        VertexIdMap vertex_id = boost::get(boost::vertex_index, g);

        // tie vertex iterators
        boost::graph_traits<boost_Graph>::vertex_iterator vi, vi_end, next;
        boost::tie(vi, vi_end) = boost::vertices(g);

        // iterate through vertices
        for (next = vi; vi != vi_end; vi = next) {
            ++next;

            // iterate through adjacent vertices
            boost::graph_traits<boost_Graph>::adjacency_iterator ai, ai_end;
            for (boost::tie(ai, ai_end) = boost::adjacent_vertices(*vi, g); ai != ai_end; ++ai) {
                result.emplace_back(vertex_id[*vi], vertex_id[*ai]);
            }
        }

        return std::move(result);
    }

    /**
     * Retrieve current connected components of the graph.
     * @return
     */
    std::vector<std::vector<std::size_t>> getConnectedComponents() {
        // Assume g is a graph object of type boost::adjacency_list
        std::vector<std::size_t> component(boost::num_vertices(g)); // Create an array for storing component numbers

        std::size_t num_components = boost::connected_components(g, &component[0]); // Calculate the connected components

        std::vector<std::vector<std::size_t>> result(num_components); // Create a vector of vectors for storing the result

        for (std::size_t i = 0; i < component.size(); ++i) { // Loop over the component array
            result[component[i]].push_back(i); // Push the vertex index into the corresponding vector
        }

        return result;
    }

private:
    boost_Graph g;
};

#endif //NCONPP_GRAPH_H
