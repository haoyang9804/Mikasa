#pragma once

#include <stdint.h>
#include <vector>
#include <tuple>
#include <queue>
#include <type_traits>
#include <functional>

template<typename... Ts>
class Node {
public:
  Node (Ts... vals) : vals(vals) {}
  
  // getters and setters
  std::tuple<Ts...> get_vals() { return vals; }
  Node<Ts...>* get_parent() { return parent; }
  Node* set_vals(std::tuple<Ts...> vals) { this->vals = vals; }
  void add_left(Node<Ts...>* left) override { this->left = left; } 
  void add_right(Node<Ts...>* right) override { this->right = right; }
  Node<Ts...>* get_left() override { return this->left; }
  Node<Ts...>* get_right() override { return this->right; }
  
  // lazy tuner
  void lazy_on() ( this->lazy = true; )
  void lazy_off() { this->lazy = false; }
  bool is_lazy() { return this->lazy; }

private:
  std::tuple<Ts...> vals;
  bool lazy = false;
  Node<Ts...>* parent;
  Node<Ts...> *left, *right;
};

// Node on a segment tree
template<typename... Ts>
class STNode : public Node {
public:
  STNode (Ts... vals) : vals(vals) {}
  
  // getters and setters
  std::tuple<Ts...> get_vals() { return vals; }
  Node<Ts...>* get_parent() { return parent; }
  Node* set_vals(std::tuple<Ts...> vals) { this->vals = vals; }
  void add_left(Node<Ts...>* left) override { this->left = left; } 
  void add_right(Node<Ts...>* right) override { this->right = right; }
  Node<Ts...>* get_left() override { return this->left; }
  Node<Ts...>* get_right() override { return this->right; }
  
  // lazy tuner
  void lazy_on() ( this->lazy = true; )
  void lazy_off() { this->lazy = false; }
  bool is_lazy() { return this->lazy; }

private:
  std::tuple<Ts...> vals;
  bool lazy = false;
  Node<Ts...>* parent;
  Node<Ts...> *left, *right;
};

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>, typename... Ts>
class SegmentTree {
public:
  SegmentTree(T l, T r, Ts... vals, std::function<std::tuple<Ts...>(Ts..., Ts...)> mergeF) { 
    this->root = build(l, r, vals...);
    this->mergeF = mergeF;
    this->updateF = updateF;
  }
  ~SegmentTree() {
    std::queue<Node<Ts...>*> node_queue;
    node_queue.push(root);
    while(!node_queue.empty()) {
      auto* top = node_queue.top();
      auto* left = top.get_left();
      auto* right = top.get_right();
      delete top;
      node_queue.pop();
      if (left) node_queue.push(left);
      if (right) node_queue.push(right);
    }
  }

  void update(int ul, int ur, std::function<std::tuple<Ts...>(Ts...)>& updateF, auto* cur_node_p = root) {
    int cur_l = cur_node_p->
  }
  

private:
  Node<T, T, Ts...>* root = nullptr;
  std::function<std::tuple<Ts...>(Ts..., Ts...)> mergeF; 

  Node<T, T, Ts...>* build(T l, T r, Ts... vals) {
    auto* cur_node_p = new Node<T, T, Ts...>(l, r, vals...);
    if (l == r) return cur_node_p;
    T mid = (l + r) / 2;
    cur_node_p->add_left(build(l, mid, vals...));
    cur_node_p->add_right(build(mid+1, r, vals...));
    cur_node_p->set_vals(mergeF(cur_node_p->get_left()->get_vals(), cur_node_p->get_right()->get_vals()));
    return cur_node_p;
  }
};