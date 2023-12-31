#pragma once

#include <array>
#include <vector>

// Considering that the input x will most of the time
// not be greater than the maximum value of int
// I fix the type and do not make this class templated.
class BinaryTrie {
private:
  // Each node except for leaf has two children
  // A non-leaf node stores the ids of the two children
  using node = std::array<int, 2>;
  std::vector<node> nodes;
  // The number of bits this BinaryTrie will consider.
  int BITS = 20;

  void create_new_node(int &new_node) {
    new_node = nodes.size();
    nodes.push_back({-1, -1});
  }

  bool is_empty(int node_id, int bit) { return nodes[node_id][bit] == -1; }

public:
  BinaryTrie() { nodes = {{-1, -1}}; }
  void setBits(int bits) { this->BITS = bits; }
  void insert(int x) {
    int node_id = 0;
    for (int i = BITS - 1; i >= 0; i--) {
      int bit = (x >> i) & 1;
      if (is_empty(node_id, bit)) {
        create_new_node(nodes[node_id][bit]);
      }
      node_id = nodes[node_id][bit];
    }
  }
  // find a value
  bool find(int x) {
    int node_id = 0;
    for (int i = BITS - 1; i >= 0; i--) {
      int bit = (x >> i) & 1;
      if (is_empty(node_id, bit))
        return false;
      node_id = nodes[node_id][bit];
    }
    return true;
  }

  // return a minimum xor between x and one stored value
  // on the trie, suppose each node except for leaf
  // has at least one child
  int minimum_xor(int x) {
    int node_id = 0;
    for (int i = BITS - 1; i >= 0; i--) {
      int bit = (x >> i) & 1;
      if (is_empty(node_id, bit))
        bit ^= 1;
      x ^= (bit << i);
      node_id = nodes[node_id][bit];
    }
    return x;
  }

  // return a maximum sum between x and one stored value
  // on the trie, suppose each node except for leaf
  // has at least one child
  int maximum_xor(int x) {
    int node_id = 0;
    for (int i = BITS - 1; i >= 0; i--) {
      int bit = (x >> i) & 1;
      if (!is_empty(node_id, bit ^ 1))
        bit ^= 1;
      x ^= (bit << i);
      node_id = nodes[node_id][bit];
    }
    return x;
  }
};