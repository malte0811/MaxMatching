#ifndef GRAPH_HPP
#define GRAPH_HPP

/**
   @file graph.hpp

   @brief This file provides a simple class @c Graph to model unweighted undirected graphs.
**/

#include <iosfwd>
#include <cstdint>
#include <limits>
#include <vector>

using size_type = uint32_t;
using NodeId = size_type;
using EdgeSet = std::vector<std::pair<NodeId, NodeId>>;

/**
   @class Node

   @brief A @c Node stores an array of neighbors (via their ids).

   @note The neighbors are not necessarily ordered, so searching for a specific neighbor takes O(degree)-time.
**/
class Node
{
public:
   /** @brief Create an isolated node (you can add neighbors later). **/
   Node() = default;

   /** @return The number of neighbors of this node. **/
   [[nodiscard]] size_type degree() const;

   /** @return The array of ids of the neighbors of this node. **/
   [[nodiscard]] std::vector<NodeId> const & neighbors() const;

private:
   friend class Graph;

   /**
      @brief Adds @c id to the list of neighbors of this node.
      @warning Does not check whether @c id is already in the list of neighbors (a repeated neighbor is legal, and
      models parallel edges).
      @warning Does not check whether @c id is the identity of the node itself (which would create a loop!).
   **/
   void add_neighbor(NodeId id);

   void remap_neighbors(std::vector<NodeId> const& mapper, NodeId new_node_count);

   std::vector<NodeId> _neighbors;
}; // class Node

/**
   @class Graph

   @brief A @c Graph stores an array of @c Node s, but no array of edges. The list of edges is implicitly given
   by the fact that the nodes know their neighbors.

   This class models undirected graphs only (in the sense that the method @c add_edge(node1, node2) adds both @c node1
   as a neighbor of @c node2 and @c node2 as a neighbor of @c node1). It also forbids loops, but parallel edges are
   legal.

   @warning Nodes are numbered starting at 0, as is usually done in programming,
    instead starting at 1, as is done in the DIMACS format that your program should take as input!
    Be careful.
**/
class Graph
{
public:
   /**
      @brief Creates a @c Graph with @c num_nodes isolated nodes.

      The number of nodes in the graph currently cannot be changed. You can only add edges between the existing nodes.
   **/
   explicit Graph(NodeId num_nodes);

   /** @return The number of nodes in the graph. **/
   [[nodiscard]] NodeId num_nodes() const;

   /**
      @return A reference to the id-th entry in the array of @c Node s of this graph.
   **/
   [[nodiscard]] Node const & node(NodeId id) const;

   /**
      @brief Adds the edge <tt> {node1_id, node2_id} </tt> to this graph.

      Checks that @c node1_id and @c node2_id are distinct and throws an exception otherwise.
      This method adds both @c node1_id as a neighbor of @c node2_id and @c node2_id as a neighbor of @c node1_id.

      @warning Does not check that the edge does not already exist, so this class can be used to model non-simple graphs.
   **/
   void add_edge(NodeId node1_id, NodeId node2_id);

   /**
    * Removes a set of nodes (and all incident edges) from the graph
    * @param should_remove vector of length num_nodes(). true indicates that the node should be removed.
    * @return A vector mapping the new node index of each remaining node to the old index, i.e.
    * result[new_node_id] == old_node_id
    */
   std::vector<NodeId> delete_nodes(std::vector<bool> const& should_remove);

   /**
    * Reads a graph in DIMACS format from the given istream and returns that graph.
    */
   static Graph read_dimacs(std::istream & str);
   /**
     @brief Prints the graph to the given ostream in DIMACS format.
   **/
   friend std::ostream & operator<<(std::ostream & str, Graph const & graph);
private:
   std::vector<Node> _nodes;
}; // class Graph
//BEGIN: Inline section

inline
size_type Node::degree() const
{
   return neighbors().size();
}

inline
std::vector<NodeId> const & Node::neighbors() const
{
   return _neighbors;
}

inline
NodeId Graph::num_nodes() const
{
   return _nodes.size();
}

inline
Node const & Graph::node(NodeId const id) const
{
   return _nodes[id];
}

#endif /* GRAPH_HPP */
