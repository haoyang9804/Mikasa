#pragma once

#include <functional>
#include <queue>
#include <stdint.h>
#include <type_traits>
#include <vector>
#include <stdexcept>
#include <utility>

#define ASSERT(STATEMENT, STR) \
  if (!(STATEMENT)) throw std::logic_error(STR)

#define ERROR(STR) throw std::logic_error(STR)

template<typename T>
struct SupportsAddAndCompare {
  template<typename U, typename = decltype(
    std::declval<U>() + std::declval<U>(), // Add
    std::declval<U>() < std::declval<U>() // compare, for performing max/min
  )>
  static std::true_type test(U*);
  
  template<typename>
  static std::false_type test(...);

  static constexpr bool value = decltype(test<T>(nullptr))::type;
};

template <typename T> class Node {
public:
  Node(T val) : val(val) {}

  // getters and setters
  T get_val() { return val; }
  Node<T> *get_parent() { return parent; }
  Node *set_val(T vals) { this->vals = vals; }
  void add_left(Node<T> *left) override { this->left = left; }
  void add_right(Node<T> *right) override { this->right = right; }
  Node<T> *get_left() override { return this->left; }
  Node<T> *get_right() override { return this->right; }

private:
  // vals are what a tree node maintains
  T val;
  Node<T> *parent;
  Node<T> *left, *right;
};

// Node on a segment tree
// T is the type of lower/upper bound, so T should be integral
// NT is the type of the value maintained by each node,
// Segment tree frequently handle querying/updating values maintained by a segment.
// In each update, the client should provide a update function to update the value
// maintained by leaf nodes. This atomic update accumulates to take effect on the 
// values maintained by non-leaf nodes. This "accumulation" is conventionally
// concretized as adding or comparing. Therefore, NT should support adding and comparing.
template <typename T, typename NT, typename = std::enable_if_t<std::is_integral_v<T>>, typename = std::enable_if<SupportsAddAndCompare<NT>::value>> 
class STNode : public Node<NT> {
public:
  STNode(NT val, T l, T r) : Node<NT>(val), l(l), r(r) {}

  // lazy tuner
  void lazy_on() { this->lazy = true; }
  void lazy_off() { this->lazy = false; }
  bool is_lazy() { return this->lazy; }
  // l and r are the left and right values of a segment
  T l, r;

  bool is_leaf() { return l == r; }

private:
  bool lazy = false;
};

template <typename T, typename NT, typename = std::enable_if_t<std::is_integral_v<T>>, typename = std::enable_if<SupportsAddAndCompare<NT>::value>>
class SegmentTree {
public:
  SegmentTree(T l, T r, NT val) {
    this->root_p = build(l, r, val);
  }
  ~SegmentTree() {
    std::queue<STNode<T, NT> *> node_queue;
    node_queue.push(root_p);
    while (!node_queue.empty()) {
      auto *top = node_queue.top();
      auto *left = top.get_left();
      auto *right = top.get_right();
      delete top;
      node_queue.pop();
      if (left)
        node_queue.push(left);
      if (right)
        node_queue.push(right);
    }
  }

  /**
   * @brief update the segment [ul:ur]
   *
   * @param ul: the left of the interval
   * @param ur: the right of the interval
   * @param updateF: update function for leaf nodes
   */
  void update(const T &ul, const T &ur,
              std::function<void(NT &)> &updateF) {
    STNode<T, NT> *cur_node_p = this->root_p;
    update(ul, ur, updateF, cur_node_p);
  }

  /**
   * @brief update a single point [u]
   *
   * @param ul: the left of the interval
   * @param ur: the right of the interval
   * @param updateF: update function for leaf nodes
   */
  void update(const T &u, std::function<void(NT &)> &updateF) {
    STNode<T, NT> *cur_node_p = this->root_p;
    update(u, updateF, cur_node_p);
  }

private:
  STNode<T, NT> *root_p = nullptr;

  // after update the left child and the right child, we should merge the
  // updates to update the current node
  inline void merge(STNode<T, NT> *cur_node_p) {
    cur_node_p->set_val(cur_node_p->get_left()->get_val() +
                                cur_node_p->get_right()->get_val());
  }

  STNode<T, NT> *build(T l, T r, NT val) {
    auto *cur_node_p = new STNode<T, NT>(l, r, val);
    if (l == r)
      return cur_node_p;
    T mid = (l + r) / 2;
    cur_node_p->add_left(build(l, mid, val));
    cur_node_p->add_right(build(mid + 1, r, val));
    merge(cur_node_p);
    return cur_node_p;
  }

  /**
   * @brief update a single point [ul]
   * 
   * @param u: the updated point
   * @param updateF: update function
   * @param cur_node_p: the current node that is ready to be updated
   */
  void update(const T &u, std::function<void(std::tuple<NT> &)> &updateF, STNode<T, NT> *cur_node_p) {
    if (cur_node_p->is_leaf()) {
      updateF(cur_node_p->get_val());
      return;
    }
    T mid = (cur_node_p->l + cur_node_p->r) / 2;
    if (u <= mid) update(u, updateF, cur_node_p->get_left());
    else update(u, updateF, cur_node_p->get_right());
    merge(cur_node_p);
  }

  /**
   * @brief update the segment [ul:ur]
   *
   * @param ul: the lower bound of the interval
   * @param ur: the upper bound of the interval
   * @param updateF: update function
   * @param cur_node_p: the current node that is ready to be updated
   */
  void update(const T &ul, const T &ur,
              std::function<void(std::tuple<NT> &)> &updateF,
              STNode<T, NT> *cur_node_p) {
    ASSERT(ul <= ur, "The left");
    
    // update a segment requires lazy mark to reduce time cost.
    // However, 

    if (ul <= cur_node_p->l && ur >= cur_node_p->r) {
      updateF(cur_node_p->get_val());
      cur_node_p->lazy_on();
      return;
    }
    if (cur_node_p->is_lazy() && !cur_node_p->is_leaf()) {
      cur_node_p->lazy_off();
    }
    T mid = (cur_node_p->l + cur_node_p->r) / 2;
    if (ul <= mid)
      update(ul, ur, updateF, cur_node_p->get_left());
    if (ur > mid)
      update(ul, ur, updateF, cur_node_p->get_right());
    merge(cur_node_p);
  }

  NT query(const T& ql, const T& qr, STNode<T, NT> *cur_node_p) {
    if (cur_node_p->is_leaf()) return cur_node_p->get_val();
    if (ql <= cur_node_p->l && qr >= cur_node_p->r) {
      return 
    }
  }
};