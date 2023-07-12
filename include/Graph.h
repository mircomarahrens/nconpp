//
// Created by mirco on 4/16/2023.
//

#ifndef NCONPP_GRAPH_H
#define NCONPP_GRAPH_H

#include <list>
#include <string>
#include <unordered_map>
#include <set>

namespace ERROR_MESSAGE
{
    const static std::string VERTEXID_PRESENT = "Vertex identifier already present.";
    const static std::string VERTEXID_NOTPRESENT = "Vertex identifier not present.";
    const static std::string EDGE_PRESENT = "Edge already present.";
    const static std::string EDGEID_PRESENT = "Edge identifier already present.";
    const static std::string EDGEID_NOTPRESENT = "Edge identifier not present.";
    const static std::string PARALLEL_EDGE_PRESENT = "Parallel edge already present.";
    const static std::string SOURCEID_NOTPRESENT = "Source vertex identifier not present.";
    const static std::string DESTID_NOTPRESENT = "Destination vertex identifier not present.";
}

namespace GRAPH_PROPERTIES
{
    struct default_t {};

    enum Color { BLACK, GRAY, WHITE };
}

template <class V = GRAPH_PROPERTIES::default_t, class E = GRAPH_PROPERTIES::default_t>
class Graph
{
public:
    Graph() = default;

    Graph(std::size_t N, bool parallel_edges = true) : m_parallel_edges(parallel_edges)
    {
        for (std::size_t i = 0; i < N; i++)
        {
            adjacency_list[i] = std::set<std::size_t>();
            vertices[i] = vertex_properties();
        }
    };

    ~Graph() = default;

    struct vertex_properties : V
    {
        std::set<int> edge_indices;
        GRAPH_PROPERTIES::Color color = GRAPH_PROPERTIES::Color::WHITE;
        // put any default properties here
    };

    struct edge_properties : E
    {
        std::size_t src, dest;
        bool directed = false;
        // put any default property here
    };

    // adjacency list
    std::unordered_map<std::size_t, std::set<std::size_t>> adjacency_list;

    // vertex index : vertex properties
    std::unordered_map<std::size_t, vertex_properties> vertices;

    // edge index : edge properties
    std::unordered_map<std::size_t, edge_properties> edges;

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
     * @brief Add a new vertex from a given index to the graph and return its index.
     * Throws exception if vertex index is present.
     *
     * @return std::size_t
     */
    std::size_t addVertex(std::size_t newVertex)
    {
        if (vertices.find(newVertex) == vertices.end())
        {
            vertices[newVertex] = vertex_properties();
        }
        else
        {
            throw std::invalid_argument(ERROR_MESSAGE::VERTEXID_PRESENT);
        }
        return newVertex;
    }

    /**
     * @brief Adds a new vertex to the graph and return its index.
     * 
     * @return std::size_t 
     */
    std::size_t addVertex()
    {
        std::size_t len = vertices.size();
        for (std::size_t id = 0; id < len; id++)
        {
            if (vertices.find(id) == vertices.end())
            {
                vertices[id] = vertex_properties();
                return id;
            }
        }
        vertices[len] = vertex_properties();
        return len; 
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
            throw std::invalid_argument(ERROR_MESSAGE::VERTEXID_NOTPRESENT);
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
     * @param directed
     * @return int
     */
    int addEdge(int edgeIndex, std::size_t src, std::size_t dest, bool directed = false)
    {
        if (vertices.find(src) == vertices.end())
        {
            throw std::invalid_argument(ERROR_MESSAGE::SOURCEID_NOTPRESENT);
        }

        if (vertices.find(dest) == vertices.end())
        {
            throw std::invalid_argument(ERROR_MESSAGE::DESTID_NOTPRESENT);
        }

        if (edges.find(edgeIndex) != edges.end())
        {
            throw std::invalid_argument(ERROR_MESSAGE::EDGEID_PRESENT);
        }

        if (!m_parallel_edges)
        {
            if (adjacency_list[dest].find(src) != adjacency_list[dest].end())
            {
                throw std::invalid_argument(ERROR_MESSAGE::PARALLEL_EDGE_PRESENT);
            }

            if (adjacency_list[src].find(dest) != adjacency_list[src].end())
            {
                throw std::invalid_argument(ERROR_MESSAGE::PARALLEL_EDGE_PRESENT);
            }
        }

        edges[edgeIndex] = {.src = src, .dest = dest, .directed = directed};

        vertices[src].edge_indices.insert(edgeIndex);
        vertices[dest].edge_indices.insert(edgeIndex);

        adjacency_list[src].insert(dest);
        if (!directed || !m_parallel_edges)
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
                throw std::invalid_argument(ERROR_MESSAGE::SOURCEID_NOTPRESENT);
            }

            if (vertices.find(dest) == vertices.end())
            {
                throw std::invalid_argument(ERROR_MESSAGE::DESTID_NOTPRESENT);
            }

            auto edge_indices = getEdges();
            for (int i : edge_indices)
            {
                auto& edge = edges[i];
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
            throw std::invalid_argument(ERROR_MESSAGE::EDGEID_NOTPRESENT);
        }
        else
        {
            auto src = edges[edgeIndex].src;
            auto dest = edges[edgeIndex].dest;

            adjacency_list[src].erase(dest);
            if (!edges[edgeIndex].directed && m_parallel_edges)
            {
                adjacency_list[dest].erase(src);
            }

            vertices[src].edge_indices.erase(edgeIndex);
            vertices[dest].edge_indices.erase(edgeIndex);

            edges.erase(edgeIndex);
        }
        return edgeIndex;
    }

private:
    // graph property
    bool m_parallel_edges = true;
};

#endif // NCONPP_GRAPH_H
