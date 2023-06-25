//
// Created by mirco on 4/16/2023.
//

#ifndef NCONPP_GRAPHNEW_H
#define NCONPP_GRAPHNEW_H

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace ERROR_MESSAGE
{
    const static std::string VERTEX_PRESENT = "Vertex index already present.";
    const static std::string VERTEX_NOTPRESENT = "Vertex index not present.";
    const static std::string EDGE_PRESENT = "Edge index already present.";
    const static std::string EDGE_NOTPRESENT = "Edge index not present.";
    const static std::string SOURCE_NOTPRESENT = "Source vertex index not present.";
    const static std::string DEST_NOTPRESENT = "Destination vertex index not present.";
}

struct default_t
{
};

template <class V = default_t, class E = default_t>
class Graph
{
public:
    Graph() = default;

    Graph(std::size_t N) {
        for (std::size_t i = 0; i < N; i++) 
        {
            vertices[i] = vertex_properties();
        }
    };

    ~Graph() = default;

    struct vertex_properties : V
    {   
        // put any default properties here
    };

    struct edge_properties : E
    {
        std::size_t src, dest;
        bool directed = false;
        // put any default property here
    };

    // vertex index : vertex properties
    std::unordered_map<std::size_t, vertex_properties> vertices;

    // edge index : edge properties
    std::unordered_map<int, edge_properties> edges;

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
     * @return std::unordered_set<size_t>
     */
    std::unordered_set<std::size_t> getVertices()
    {
        std::unordered_set<std::size_t> keys;
        for (auto p : vertices) 
        {
            keys.insert(p.first);
        }
        return keys;
    }

    /**
     * @brief Add a new vertex to the graph and return its index.
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
            throw std::invalid_argument(ERROR_MESSAGE::VERTEX_PRESENT);
        }
        return newVertex;
    }

    /**
     * @brief Remove a present vertex by index and return its index.
     * Throws exception if vertex index is not present.
     * 
     * @param oldVertex 
     * @return std::size_t 
     */
    std::size_t removeVertex(std::size_t oldVertex)
    {
        if (vertices.find(oldVertex) != vertices.end())
        {
            vertices.erase(oldVertex);
        }
        else
        {
            throw std::invalid_argument(ERROR_MESSAGE::VERTEX_NOTPRESENT);
        }
        return oldVertex;
    }


    int addEdge(int edgeIndex, std::size_t src, std::size_t dest, bool directed = false)
    {
        if (vertices.find(src) == vertices.end())
        {
            throw std::invalid_argument(ERROR_MESSAGE::SOURCE_NOTPRESENT);
        }

        if (vertices.find(dest) == vertices.end())
        {
            throw std::invalid_argument(ERROR_MESSAGE::DEST_NOTPRESENT);
        }

        if (edges.find(edgeIndex) != edges.end())
        {
            throw std::invalid_argument(ERROR_MESSAGE::EDGE_PRESENT);
        }

        edges[edgeIndex] = {.src = src, .dest = dest, .directed = directed};

        return edgeIndex;
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
     * @return std::unordered_set<size_t>
     */
    std::unordered_set<std::size_t> getEdges()
    {
        std::unordered_set<std::size_t> keys;
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
            throw std::invalid_argument(ERROR_MESSAGE::EDGE_NOTPRESENT);
        }
        else 
        {
            edges.erase(edgeIndex);
        }
        return edgeIndex;
    }
};

#endif // NCONPP_GRAPHNEW_H
