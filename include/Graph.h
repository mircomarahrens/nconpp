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

struct default_t
{
};

template <class V = default_t, class E = default_t>
class Graph
{
public:
    Graph(std::size_t N) : graph_t(N){};

    ~Graph() = default;

    struct vertex_properties_t : V
    {
        // put any default properties here
    };
    struct edge_properties_t : E
    {
        std::size_t edge_index_t;
        // put any default properties here
    };

    typedef typename boost::adjacency_list<boost::multisetS, boost::vecS, boost::undirectedS, vertex_properties_t, edge_properties_t>
        GraphContainer;
    typedef typename boost::vertex_bundle_type<GraphContainer>::type Vertex;
    typedef typename boost::edge_bundle_type<GraphContainer>::type Edge;

    typedef typename boost::graph_traits<GraphContainer>::vertex_descriptor vertex_descriptor;
    typedef typename boost::graph_traits<GraphContainer>::edge_descriptor edge_descriptor;

    typedef typename boost::graph_traits<GraphContainer>::vertex_iterator vertex_iterator;
    typedef typename boost::graph_traits<GraphContainer>::edge_iterator edge_iterator;
    typedef typename boost::graph_traits<GraphContainer>::out_edge_iterator out_edge_iterator;

    typedef typename boost::graph_traits<GraphContainer>::adjacency_iterator adjacency_iterator;

    /**
     * @brief Operator to return the bundled vertex from a descriptor.
     *
     * @param v
     * @return Vertex
     */
    Vertex operator[](const vertex_descriptor &v)
    {
        return graph_t[v];
    }

    /**
     * @brief Operator to return the bundled edge from a descriptor.
     *
     * @param e
     * @return Edge
     */
    Edge operator[](const edge_descriptor &e)
    {
        return graph_t[e];
    }

    /**
     * @brief Return the vertex iterators begin() and end() for the whole graph.
     *
     * @return std::pair<vertex_iterator, vertex_iterator>
     */
    std::pair<vertex_iterator, vertex_iterator> vertices()
    {
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
     * @brief Add a new vertex with custom properties to the graph and return its id.
     *
     * @return std::size_t
     */
    std::size_t addVertex(vertex_properties_t vertex_properties)
    {
        auto v = boost::add_vertex(vertex_properties, graph_t);
        return v;
    }

    /**
     * @brief Set for a given vertex the vertex properties.
     *
     * @param vertex
     * @param vertex_properties
     */
    void setVertexProperties(vertex_descriptor vertex, vertex_properties_t vertex_properties)
    {
        graph_t[vertex] = vertex_properties;
    }

    /**
     * @brief Remove a vertex from the graph by its id. All edges corresponding to the vertex are removed beforehand.
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

    /**
     * @brief Merge edges of the second vertex into the first vertex and delete the latter.
     * The properties of the first vertex are preserved.
     *
     * @param src
     * @param dest
     */
    void mergeVertices(std::size_t src, std::size_t dest)
    {
        // obtain vertex descriptor for src and dest
        vertex_descriptor source = boost::vertex(src, graph_t);
        vertex_descriptor target = boost::vertex(dest, graph_t);

        // define and declare out edge iterators
        out_edge_iterator oi, oi_end, next;
        boost::tie(oi, oi_end) = boost::out_edges(target, graph_t);

        // iterate through iterators from begin to end via next
        for (next = oi; oi != oi_end; oi = next)
        {
            ++next;

            // get the bundled edge property
            auto edge_properties = graph_t[*oi];
            auto target_old = boost::target(*oi, graph_t);
            
            // and add a new edge (source, target_old) with the given edge properties
            boost::add_edge(source, target_old, edge_properties, graph_t);
        }
        removeVertex(dest);
    }

    /**
     * @brief Add a new edge to the graph with a edge index.
     *
     * @param src
     * @param dest
     * @param edge_index
     */
    void addEdge(std::size_t src, std::size_t dest, std::size_t edge_index)
    {
        checkEdgeIndex(edge_index);

        edge_properties_t edge_properties;
        edge_properties.edge_index_t = edge_index;

        auto e = boost::add_edge(src, dest, edge_properties, graph_t);
        assert(e.second == true);
    }

    /**
     * @brief Add a new edge to the graph with edge properties.
     *
     * @param src
     * @param dest
     * @param edge_properties
     */
    void addEdge(std::size_t src, std::size_t dest, edge_properties_t edge_properties)
    {
        edge_iterator ei, ei_end, next;
        boost::tie(ei, ei_end) = boost::edges(graph_t);

        checkEdgeIndex(edge_properties.edge_index_t);

        auto e = boost::add_edge(src, dest, edge_properties, graph_t);
        assert(e.second == true);
    }

    /**
     * @brief Set edge properties.
     *
     * @param edge
     * @param edge_properties
     */
    void setVertexProperties(edge_descriptor edge, edge_properties_t edge_properties)
    {
        graph_t[edge] = edge_properties;
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

            checkEdgeIndex(edge_index);

            edge_properties_t edge_properties;
            edge_properties.edge_index_t = edge_index;

            auto e = boost::add_edge(ep.first, ep.second, edge_properties, graph_t);
            assert(e.second == true);
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

    std::pair<std::size_t, std::size_t> getEdge(int edge_index)
    {
        edge_iterator ei, ei_end, next;
        boost::tie(ei, ei_end) = boost::edges(graph_t);

        for (next = ei; ei != ei_end; ei = next)
        {
            ++next;
            if (graph_t[*ei].edge_index_t == edge_index)
            {
                auto source = boost::source(*ei, graph_t);
                auto target = boost::target(*ei, graph_t);
                return std::move(std::make_pair(source, target));
            }
        }

        throw std::runtime_error(ERROR_MESSAGE::EDGE_INDEX_NOT_PRESENT);
    }

    /**
     * @brief Get current edge ids.
     *
     * @return std::set<int>
     */
    std::set<int> getEdges()
    {
        std::set<int> result = {};

        edge_iterator ei, ei_end, next;
        boost::tie(ei, ei_end) = boost::edges(graph_t);

        for (next = ei; ei != ei_end; ei = next)
        {
            ++next;
            result.insert(graph_t[*ei].edge_index_t);
        }

        return result;
    }

    /**
     * @brief Get out edge iterators for begin and end.
     *
     * @param v
     * @return std::pair<out_edge_iterator, out_edge_iterator>
     */
    std::pair<out_edge_iterator, out_edge_iterator> outEdges(const vertex_descriptor &v)
    {
        return boost::out_edges(v, graph_t);
    }

    /**
     * @brief Remove edge by index.
     *
     * @param edge_index
     */
    void removeEdge(int edge_index)
    {
        edge_iterator ei, ei_end, next;
        boost::tie(ei, ei_end) = boost::edges(graph_t);

        bool notFound = true;
        for (next = ei; ei != ei_end; ei = next)
        {
            ++next;
            if (graph_t[*ei].edge_index_t == edge_index)
            {
                boost::remove_edge(*ei, graph_t);
                notFound = false;
            }
        }

        if (notFound)
        {
            throw std::invalid_argument(ERROR_MESSAGE::EDGE_INDEX_NOT_PRESENT);
        }
    }

private:
    GraphContainer graph_t;

    void checkEdgeIndex(int edge_index)
    {
        edge_iterator ei, ei_end, next;
        boost::tie(ei, ei_end) = boost::edges(graph_t);

        for (next = ei; ei != ei_end; ei = next)
        {
            ++next;

            if (graph_t[*ei].edge_index_t == edge_index)
            {
                throw std::invalid_argument(ERROR_MESSAGE::EDGE_INDEX_PRESENT);
            }
        }
    }
};

#endif // NCONPP_GRAPH_H
