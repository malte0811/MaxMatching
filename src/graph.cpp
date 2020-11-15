#include "graph.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <optional>

namespace {

// Using a function for converting the DIMACS node ids to and from our node ids
// makes the in and output code more understandable.
NodeId from_dimacs_id(size_type dimacs_node_id) {
    if (dimacs_node_id <= 0) {
        throw std::runtime_error("Non-positive DIMACS node id can not be converted.");
    }
    return dimacs_node_id - 1;
}

// Returns the first line which is not a comment, i.e. does not start with c.
std::string read_next_non_comment_line(std::istream& input) {
    std::string line;
    do {
        if (!std::getline(input, line)) {
            throw std::runtime_error("Unexpected end of DIMACS stream.");
        }
    } while (line[0] == 'c');
    return line;
}

} // end of anonymous namespace

/////////////////////////////////////////////
//! \c Node definitions
/////////////////////////////////////////////

void Node::add_neighbor(NodeId const id) {
    assert(std::find(_neighbors.begin(), _neighbors.end(), id) == _neighbors.end());
    _neighbors.push_back(id);
}

void Node::remap_neighbors(std::vector<std::optional<NodeId>> const& mapper) {
    size_t new_num_neighbors = 0;
    for (auto const& neighbor : _neighbors) {
        auto const& mapped = mapper.at(neighbor);
        if (mapped) {
            _neighbors.at(new_num_neighbors) = *mapped;
            ++new_num_neighbors;
        }
    }
    _neighbors.resize(new_num_neighbors);
}

/////////////////////////////////////////////
//! \c Graph definitions
/////////////////////////////////////////////

// Whenever reasonably possible you should prefer to use `:`
// to initalize the members of your class, instead of
// assigning values to them after they were default initialized.
// Note you should initialize them in the same order
// they were declare in back in the class body!
Graph::Graph(NodeId const num_nodes)
        : _nodes(num_nodes) {}

void Graph::add_edge(NodeId node1_id, NodeId node2_id) {
    if (node1_id == node2_id) {
        throw std::runtime_error("Graph class does not support loops!");
    }

    _nodes[node1_id].add_neighbor(node2_id);
    _nodes[node2_id].add_neighbor(node1_id);
}

Graph Graph::read_dimacs(std::istream& input) {
    // When parsing the DIMACS format, there are some words we are not interested in.
    // We read them into this variable and never use the afterwards.
    std::string unused_word{};

    // As we need to watch out for comments, we first need to read the input by line.
    // In order to split non-comment lines into multiple variables we use a std::stringstream.
    std::stringstream first_buffering_stream{};

    std::string const first_line = read_next_non_comment_line(input);

    size_type num_nodes{};
    size_type num_edges{};
    first_buffering_stream << first_line;
    first_buffering_stream >> unused_word >> unused_word >> num_nodes >> num_edges;

    // Now we successively add edges to our graph;
    Graph graph(num_nodes);
    for (size_type i = 1; i <= num_edges; ++i) {
        // This works just as parsing the first line!
        std::stringstream ith_buffering_stream{};
        std::string const ith_line = read_next_non_comment_line(input);
        NodeId dimacs_node1{};
        NodeId dimacs_node2{};
        ith_buffering_stream << ith_line;
        ith_buffering_stream >> unused_word >> dimacs_node1 >> dimacs_node2;
        graph.add_edge(from_dimacs_id(dimacs_node1), from_dimacs_id(dimacs_node2));
    }

    return graph;
}

std::vector<NodeId> Graph::delete_nodes(std::vector<bool> const& should_remove) {
    NodeId new_num_nodes = 0;
    std::vector<NodeId> id_mapper;
    std::vector<std::optional<NodeId>> inverse_id_mapper(num_nodes());
    for (NodeId i = 0; i < num_nodes(); ++i) {
        assert(new_num_nodes <= i);
        if (not should_remove.at(i)) {
            inverse_id_mapper.at(i) = new_num_nodes;
            id_mapper.push_back(i);
            ++new_num_nodes;
            assert(id_mapper.size() == new_num_nodes);
        }
    }
    Graph new_graph(new_num_nodes);
    for (NodeId new_node = 0; new_node < new_num_nodes; ++new_node) {
        for (auto const& neighbor_old : node(id_mapper.at(new_node)).neighbors()) {
            auto const& mapped_neighbor = inverse_id_mapper.at(neighbor_old);
            if (mapped_neighbor and new_node < *mapped_neighbor) {
                new_graph.add_edge(new_node, *mapped_neighbor);
            }
        }
    }
    *this = new_graph;
    return id_mapper;
}

Graph Graph::shuffle_with_seed(unsigned long seed) const {
    Graph result(num_nodes());
    std::vector<NodeId> map(num_nodes());
    std::iota(map.begin(), map.end(), 0);
    std::mt19937 random(seed);
    std::shuffle(map.begin(), map.end(), random);
    for (NodeId i = 0; i < num_nodes(); ++i) {
        auto const& mapped = map.at(i);
        for (auto const& neighbor : node(i).neighbors()) {
            if (mapped < map.at(neighbor)) {
                result.add_edge(mapped, map.at(neighbor));
            }
        }
    }
    for (NodeId i = 0; i < num_nodes(); ++i) {
        auto& node = result._nodes.at(i)._neighbors;
        std::sort(node.begin(), node.end());
    }
    return result;
}

Graph Graph::with_extra_all_edge_vertices(NodeId extra_vertices) const {
    Graph result = *this;
    result._nodes.resize(num_nodes() + extra_vertices);
    for (NodeId new_node_id = num_nodes(); new_node_id < result.num_nodes(); ++new_node_id) {
        for (NodeId neighbor = 0; neighbor < num_nodes(); ++neighbor) {
            result.add_edge(neighbor, new_node_id);
        }
    }
    return result;
}
