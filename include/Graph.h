//
// Created by mirco on 4/16/2023.
//

#ifndef NCONPP_GRAPH_H
#define NCONPP_GRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <string>

namespace ERROR_MESSAGE
{
    const static std::string EDGE_INDEX_PRESENT = "Edge index already present.";
    const static std::string EDGE_INDEX_NOT_PRESENT = "Edge index not present.";
}

struct default_vertex_properties
{
};

struct default_edge_properties 
{
    std::size_t edge_index;
};

template<typename V = default_vertex_properties, typename E = default_edge_properties>
class Graph
{
public:
    Graph(std::size_t N) : graph_t(N){};

    ~Graph() = default;

    typedef typename boost::adjacency_list<boost::multisetS, boost::vecS, boost::undirectedS, V, E>
        GraphContainer;
    typedef typename boost::vertex_bundle_type<GraphContainer>::type Vertex;
    typedef typename boost::edge_bundle_type<GraphContainer>::type Edge;

    typedef typename boost::graph_traits<GraphContainer>::vertex_descriptor vertex_descriptor;
    typedef typename boost::graph_traits<GraphContainer>::edge_descriptor edge_descriptor;

    typedef typename boost::graph_traits<GraphContainer>::vertex_iterator vertex_iterator;
    typedef typename boost::graph_traits<GraphContainer>::edge_iterator edge_iterator;
    typedef typename boost::graph_traits<GraphContainer>::out_edge_iterator out_edge_iterator;

    /**
     * @brief Operator to return the bundled vertex from a descriptor.
     * 
     * @param v 
     * @return Vertex 
     */
    Vertex operator[](const vertex_descriptor& v) 
    {
        return graph_t[v];
    }

    /**
     * @brief Operator to return the bundled edge from a descriptor.
     * 
     * @param e 
     * @return Edge 
     */
    Edge operator[](const edge_descriptor& e)
    {
        return graph_t[e];
    }

    /**
     * @brief Return the vertex iterators begin() and end() for the whole graph.
     * 
     * @return std::pair<vertex_iterator, vertex_iterator> 
     */
    std::pair<vertex_iterator, vertex_iterator> vertices() {
        return boost::vertices(graph_t);
    }

    /**
     * @brief Get vertex indices.
     *
     * @return std::set<size_t>
     */
    std::set<size_t> getVertices()
    {
        std::set<std::size_t> result = {};
        auto pm = boost::get(boost::vertex_index, graph_t);
        for (std::size_t i = 0; i < boost::num_vertices(graph_t); i++)
        {
            result.insert(pm[i]);
        }
        return std::move(result);
    }

    /**
     * @brief Add a new vertex to the graph and return its id.
     *
     * @return std::size_t
     */
    std::size_t addVertex()
    {
        auto v = boost::add_vertex(graph_t);
        return v;
    }

    /**
     * @brief Remove a vertex from the graph by id.
     * All edges corresponding to the vertex are removed beforehand.
     *
     * @param vertex_index
     */
    void removeVertex(std::size_t vertex_index)
    {
        boost::clear_vertex(vertex_index, graph_t); // ensure all edges to vertex are removed beforehand
        boost::remove_vertex(vertex_index, graph_t);
    }

    /**
     * @brief Get the current number of vertives.
     *
     */
    std::size_t numVertices()
    {
        return boost::num_vertices(graph_t);
    }

    // /**
    //  * @brief Merge edges of the second vertex into the first vertex.
    //  *
    //  * @param id1
    //  * @param id2
    //  */
    // void mergeVertices(std::size_t id1, std::size_t id2)
    // {
    //     // obtain vertex descriptor
    //     vertex_descriptor v1d = boost::vertex(id1, graph_t);
    //     vertex_descriptor v2d = boost::vertex(id2, graph_t);

    //     // get iterator range for adjacent vertices of second vertex
    //     adjacency_iterator ai, ai_end, next;
    //     boost::tie(ai, ai_end) = boost::adjacent_vertices(v2d, graph_t);

    //     auto edge_index_map = boost::get(boost::edge_index_t(), graph_t);

    //     // iterate over adjacent vertices
    //     for (next = ai; ai != ai_end; ai = next)
    //     {
    //         ++next;

    //         // obtain the corresponding edge from vertices
    //         std::pair<edge_descriptor, bool> e = boost::edge(v2d, *ai, graph_t);
    //         edge_descriptor edge = e.first;

    //         auto index = boost::get(edge_index_map, edge);

    //         vertex_descriptor target = boost::target(edge, graph_t);
    //         if (e.second && target != v1d)
    //         {
    //             vertex_descriptor source = boost::source(edge, graph_t);
    //             boost::remove_edge(edge, graph_t);

    //             e = boost::add_edge(v1d, target, graph_t);
    //             boost::put(edge_index_map, e.first, index);
    //         }
    //     }

    //     // clear and remove dest vertex
    //     boost::clear_vertex(v2d, graph_t);
    //     boost::remove_vertex(v2d, graph_t);
    // }

    // /**
    //  * @brief Get current vertex ids.
    //  *
    //  * @return std::set<std::size_t>
    //  */
    // std::set<std::size_t> getVertices()
    // {
    //     std::set<std::size_t> result = {};

    //     vertex_iterator vi, vi_end;
    //     for (boost::tie(vi, vi_end) = boost::vertices(graph_t); vi != vi_end; ++vi)
    //     {
    //         result.insert(graph_t[*vi].id);
    //     }

    //     return result;
    // }

    // /**
    //  * @brief Remove all vertices from the graph.
    //  * All edges corresponding to the vertex are removed beforehand.
    //  *
    //  */
    // void removeVertices()
    // {
    //     vertex_iterator vi, vi_end;
    //     for (boost::tie(vi, vi_end) = boost::vertices(graph_t); vi != vi_end; ++vi)
    //     {
    //         boost::clear_vertex(*vi, graph_t); // ensure all edges to vertex are removed beforehand
    //         boost::remove_vertex(*vi, graph_t);
    //     }
    // }

    /**
     * @brief Add a new edge to the graph with a (optionally) custom Id (default 0).
     *
     * @param src
     * @param dest
     * @param customId
     * @return auto
     */
    void addEdge(std::size_t src, std::size_t dest, int edge_index)
    {
        if (edgeMap.find(edge_index) != edgeMap.end())
        {
            throw std::invalid_argument(ERROR_MESSAGE::EDGE_INDEX_PRESENT);
        }

        auto e = boost::add_edge(src, dest, graph_t);
        graph_t[e.first].edge_index = edge_index;
        assert(e.second == true);
        edgeMap[edge_index] = std::make_pair(src, dest);
    }

    /**
     * @brief Add multiple edges via list of tuples pair{src, dest}, edge_index.
     *
     * @param edges
     */
    void addEdges(std::list<std::tuple<std::pair<std::size_t, std::size_t>, int>> edges)
    {
        for (auto edge : edges)
        {
            std::pair<std::size_t, std::size_t> ep = std::get<0>(edge);
            int edge_index = std::get<1>(edge);

            if (edgeMap.find(edge_index) != edgeMap.end())
            {
                throw std::invalid_argument(ERROR_MESSAGE::EDGE_INDEX_PRESENT);
            }

            auto e = boost::add_edge(ep.first, ep.second, graph_t);
            graph_t[e.first].edge_index = edge_index;
            assert(e.second == true);
            edgeMap[edge_index] = ep;
        }
    }

    /**
     * @brief Get the current number of edges.
     *
     * @return std::size_t
     */
    std::size_t numEdges()
    {
        return boost::num_edges(graph_t);
    }

    const std::pair<std::size_t, std::size_t> &getEdge(int index)
    {
        return edgeMap[index];
    }

    /**
     * @brief Get current edge ids.
     *
     * @return std::set<int>
     */
    std::set<int> getEdges()
    {
        std::set<int> result = {};

        for (auto kv : edgeMap)
        {
            result.insert(kv.first);
        }

        return result;
    }

    /**
     * @brief Get out edge iterators for begin and end.
     * 
     * @param v 
     * @return std::pair<out_edge_iterator, out_edge_iterator> 
     */
    std::pair<out_edge_iterator, out_edge_iterator> outEdges(const vertex_descriptor& v)
    {
        return boost::out_edges(v, graph_t);
    }

    /**
     * @brief Remove edge by index.
     *
     * @param edge_index
     */
    void removeEdge(std::size_t edge_index)
    {
        if (edgeMap.find(edge_index) == edgeMap.end())
        {
            throw std::invalid_argument(ERROR_MESSAGE::EDGE_INDEX_NOT_PRESENT);
        }

        auto e = boost::edge(edgeMap[edge_index].first, edgeMap[edge_index].second, graph_t);
        assert(e.second == true);
        boost::remove_edge(e.first, graph_t);
        edgeMap.erase(edge_index);
    }

private:
    GraphContainer graph_t;

    std::unordered_map<int, std::pair<std::size_t, std::size_t>> edgeMap;

    
    // typedef typename boost::graph_traits<GraphContainer>::vertex_descriptor vertex_descriptor;
    // typedef typename boost::graph_traits<GraphContainer>::adjacency_iterator adjacency_iterator;
    // typedef typename boost::graph_traits<GraphContainer>::vertex_iterator vertex_iterator;
    // typedef typename boost::property_map<GraphContainer, boost::vertex_index_t>::type vertex_index_property_map_t;

    // typedef typename boost::graph_traits<GraphContainer>::edge_descriptor edge_descriptor;
    // typedef typename boost::graph_traits<GraphContainer>::edge_iterator edge_iterator;
    // typedef typename boost::property_map<GraphContainer, boost::edge_index_t>::type edge_index_property_map_t;


    // // store connected components
    // std::vector<std::vector<std::size_t>> mConnectedComponents = {};

    // typedef typename boost::property_map<graph_t, boost::edge_index_t>::type edge_index_pm;
    // typedef typename boost::graph_traits<graph_t>::vertex_descriptor vertex;
    // typedef typename boost::graph_traits<graph_t>::adjacency_iterator Adjacency_Iterator;
};

#endif // NCONPP_GRAPH_H
