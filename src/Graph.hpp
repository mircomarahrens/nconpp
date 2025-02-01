// Copyright 2023 Mirco Marahrens

#ifndef NCONPP_INCLUDE_GRAPH_H_
#define NCONPP_INCLUDE_GRAPH_H_

#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "ErrorMessages.hpp"

namespace GRAPH_PROPERTIES
{
struct default_struct
{
};
} // namespace GRAPH_PROPERTIES

template <class G = GRAPH_PROPERTIES::default_struct, class V = GRAPH_PROPERTIES::default_struct,
          class E = GRAPH_PROPERTIES::default_struct>
class Graph
{
  public:
    Graph() = default;

    Graph(std::size_t N, bool parallel_edges = true, bool directed_edges = false)
    {
        for (std::size_t i = 0; i < N; i++)
        {
            adjacency_list[i] = std::set<std::size_t>();
            vertices[i] = vertex_properties_t();
        }

        // graph_properties = graph_properties_t();
        graph_properties.parallel_edges = parallel_edges;
        graph_properties.directed_edges = directed_edges;
    }

    ~Graph() = default;

    struct graph_properties_t : G
    {
        // put any default property here
        bool parallel_edges = true;
        bool directed_edges = false;
    };

    struct vertex_properties_t : V
    {
        // put any default properties here
        std::set<int> edge_indices;
    };

    struct edge_properties_t : E
    {
        // put any default property here
        std::size_t src, dest;
    };

    // graph properties
    graph_properties_t graph_properties{graph_properties_t()};

    // adjacency list
    std::unordered_map<std::size_t, std::set<std::size_t>> adjacency_list;

    // vertex index : vertex properties
    std::unordered_map<std::size_t, vertex_properties_t> vertices;

    // edge index : edge properties
    std::unordered_map<std::size_t, edge_properties_t> edges;

    /**
     * @brief Get the current number of vertices.
     *
     */
    std::size_t NumVertices()
    {
        return vertices.size();
    }

    /**
     * @brief Get vertex indices.
     *
     * @return std::set<size_t>
     */
    std::set<std::size_t> getVertices()
    {
        std::set<std::size_t> keys;
        for (auto p : vertices)
        {
            keys.insert(p.first);
        }
        return keys;
    }

/**
 * @brief Add a new vertex from a given index to the graph and return its
 * index. Throws exception if vertex index is present.
 *
 * @return std::size_t
 */
#include <optional>

    std::size_t addVertex(std::optional<std::size_t> newVertexIndex = std::nullopt)
    {
        if (newVertexIndex)
        {
            std::size_t newVertex = newVertexIndex.value();
            if (vertices.find(newVertex) == vertices.end())
            {
                vertices[newVertex] = vertex_properties_t();
            }
            else
            {
                throw std::invalid_argument(ErrorMessages::ERROR_VERTEXID_PRESENT);
            }
            return newVertex;
        }
        else
        {
            std::size_t len = vertices.size();
            for (std::size_t id = 0; id < len; id++)
            {
                if (vertices.find(id) == vertices.end())
                {
                    vertices[id] = vertex_properties_t();
                    return id;
                }
            }
            vertices[len] = vertex_properties_t();
            return len;
        }
    }

    /**
     * @brief Remove a vertex by index and return its index.
     * Beforehand all edges corresponding to the vertex are also removed.
     * Throws exception if vertex index is not present.
     *
     * @param vertex
     * @return std::size_t
     */
    std::size_t removeVertex(std::size_t vertex)
    {
        if (vertices.find(vertex) != vertices.end())
        {
            clearVertex(vertex);
            vertices.erase(vertex);
            adjacency_list.erase(vertex);
        }
        else
        {
            throw std::invalid_argument(ErrorMessages::ERROR_VERTEXID_NOTPRESENT);
        }
        return vertex;
    }

    /**
     * @brief Add a new edge to the graph.
     * Throws an exception if either the source or the destination
     * is not a valid vertex or the edge index is already present.
     *
     * @param edgeIndex
     * @param src
     * @param dest
     * @return int
     */
    int addEdge(int edgeIndex, std::size_t src, std::size_t dest)
    {
        if (vertices.find(src) == vertices.end())
        {
            throw std::invalid_argument(ErrorMessages::ERROR_SOURCEID_NOTPRESENT);
        }

        if (vertices.find(dest) == vertices.end())
        {
            throw std::invalid_argument(ErrorMessages::ERROR_DESTID_NOTPRESENT);
        }

        if (edges.find(edgeIndex) != edges.end())
        {
            throw std::invalid_argument(ErrorMessages::ERROR_EDGEID_PRESENT);
        }

        if (!graph_properties.parallel_edges)
        {
            if (adjacency_list[dest].find(src) != adjacency_list[dest].end())
            {
                throw std::invalid_argument(ErrorMessages::ERROR_PARALLEL_EDGE_PRESENT);
            }

            if (adjacency_list[src].find(dest) != adjacency_list[src].end())
            {
                throw std::invalid_argument(ErrorMessages::ERROR_PARALLEL_EDGE_PRESENT);
            }
        }

        edges[edgeIndex] = {.src = src, .dest = dest};

        vertices[src].edge_indices.insert(edgeIndex);
        vertices[dest].edge_indices.insert(edgeIndex);

        adjacency_list[src].insert(dest);
        if (!graph_properties.directed_edges)
        {
            adjacency_list[dest].insert(src);
        }

        return edgeIndex;
    }

    /**
     * @brief Remove all edges corresponding to the vertex.
     *
     * @param vertex
     */
    void clearVertex(std::size_t vertex)
    {
        for (int i : vertices[vertex].edge_indices)
        {
            removeEdge(i);
        }
    }

    /**
     * @brief Merge edges of vertex dest into src and remove dest from graph.
     * Throws an exception if src or dest vertex not present.
     *
     * @param src
     * @param dest
     */
    void mergeVertices(std::size_t src, std::size_t dest)
    {
        if (src != dest)
        {
            if (vertices.find(src) == vertices.end())
            {
                throw std::invalid_argument(ErrorMessages::ERROR_SOURCEID_NOTPRESENT);
            }

            if (vertices.find(dest) == vertices.end())
            {
                throw std::invalid_argument(ErrorMessages::ERROR_DESTID_NOTPRESENT);
            }

            auto edge_indices = getEdges();
            for (int i : edge_indices)
            {
                auto &edge = edges[i];
                if (edge.src == dest)
                {
                    edge.src = src;
                    vertices[src].edge_indices.insert(i);
                    adjacency_list[src].insert(dest);
                    vertices[dest].edge_indices.erase(i);
                }
                else if (edge.dest == dest)
                {
                    adjacency_list[src].insert(edge.dest);
                    edge.dest = src;
                    vertices[src].edge_indices.insert(i);
                    vertices[dest].edge_indices.erase(i);
                }
            }
            removeVertex(dest);
        }
    }

    /**
     * @brief Get the current number of edges.
     *
     */
    std::size_t NumEdges()
    {
        return edges.size();
    }

    /**
     * @brief Get edge indices.
     *
     * @return std::set<size_t>
     */
    std::set<std::size_t> getEdges()
    {
        std::set<std::size_t> keys;
        for (auto p : edges)
        {
            keys.insert(p.first);
        }
        return keys;
    }

    /**
     * @brief Remove an edge by index and return its index.
     * Throws an exception if index is not present.
     *
     * @param edgeIndex
     * @return int
     */
    int removeEdge(int edgeIndex)
    {
        if (edges.find(edgeIndex) == edges.end())
        {
            throw std::invalid_argument(ErrorMessages::ERROR_EDGEID_NOTPRESENT);
        }
        else
        {
            auto src = edges[edgeIndex].src;
            auto dest = edges[edgeIndex].dest;

            adjacency_list[src].erase(dest);
            if (!graph_properties.directed_edges && graph_properties.parallel_edges)
            {
                adjacency_list[dest].erase(src);
            }

            vertices[src].edge_indices.erase(edgeIndex);
            vertices[dest].edge_indices.erase(edgeIndex);

            edges.erase(edgeIndex);
        }
        return edgeIndex;
    }
};

#endif // NCONPP_INCLUDE_GRAPH_H_
