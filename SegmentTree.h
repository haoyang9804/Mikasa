#pragma once

#include <cstddef>
#include <functional>
#include <queue>
#include <stdint.h>
#include <vector>
#include <stdexcept>
#include <utility>
#include <type_traits>

#define ASSERT(STATEMENT, STR) \
  if (!(STATEMENT)) throw std::logic_error(STR)

#define ERROR(STR) throw std::logic_error(STR)

// STNode describes the node on a segment tree
// T is the type of lower/upper bound, so T should be integral
// NT is the type of the value maintained by each node,
// set_pushupF is required, pushupF describes how two vals maintained
// by two children should be pushupd to affect their common parent.
template <typename T, typename NT, typename = std::enable_if_t<std::is_arithmetic_v<T>>> 
class STNode {
public:
  STNode(NT val, T l, T r) : val(val), l(l), r(r) {}
  STNode(T l, T r) : l(l), r(r) {}

  // getters and setters
  T get_val() { return val; }
  STNode *get_parent() { return parent; }
  void set_val(NT val) { this->val = val; }
  void add_left(STNode *left) { this->left = left; }
  void add_right(STNode *right) { this->right = right; }
  STNode *get_left() { return this->left; }
  STNode *get_right() { return this->right; }

  // lazy tuner
  void lazy_on(NT lazy_val, const std::function<NT(NT, NT)>& lazy_update) { 
    if (!this->lazy) {
      this->lazy = true; 
      this->lazy_val = lazy_val; 
    }
    else {
      ASSERT(lazy_update, "You should specify how to update lazy values");
      this->lazy_val = lazy_update(this->lazy_val, lazy_val);
    }
  }
  void lazy_off() { this->lazy = false; }
  bool is_lazy() { return this->lazy; }
  NT get_lazy_val() { return this-> lazy_val; }
  // l and r are the left and right values of a segment
  T l, r;

  bool is_leaf() { return l == r; }

private:
  NT val;
  STNode *parent;
  STNode *left, *right;
  bool lazy = false;
  NT lazy_val;
};

template <typename T, typename NT, typename = std::enable_if_t<std::is_integral_v<T>>>
class SegmentTree {
public:
  // There are two ways to initialize this tree.
  // The first way is initialize all STNode instance with the same val
  // The second is provide a vector, use it to build the tree.

  SegmentTree(std::vector<NT> vec, const std::function<NT(NT,NT)>& pushupF) {
    this->vec = vec;
    this->pushupF = pushupF;
    this->root_p = build(0, vec.size()-1);
  }

  SegmentTree(T l, T r, NT val, const std::function<NT(NT,NT)>& pushupF) {
    this->pushupF = pushupF;
    this->root_p = build(l, r, val);
  }
  
  ~SegmentTree() {
    std::queue<STNode<T, NT> *> node_queue;
    node_queue.push(root_p);
    while (!node_queue.empty()) {
      auto *top = node_queue.front();
      auto *left = top->get_left();
      auto *right = top->get_right();
      delete top;
      node_queue.pop();
      if (left)
        node_queue.push(left);
      if (right)
        node_queue.push(right);
    }
  }

  void set_update_function(const std::function<NT(NT,NT)> &updateF) {
    this->updateF_ele = updateF;
  }

  void set_update_function(const std::function<NT(NT,NT,T,T)> &updateF) {
    this->updateF_seg = updateF;
  }

  void set_lazy_update(const std::function<NT(NT, NT)> &lazy_update) {
    this->lazy_update = lazy_update;
  }

  /**
   * @brief update the segment [ul:ur]
   *
   * @param ul: the left of the interval
   * @param ur: the right of the interval
   * @param u_val: the value by which to update the value maintained by the node
   */
  void update(const T &ul, const T &ur, const NT& u_val) {
    ASSERT(ul >= root_p->l && ur <= root_p->r, "lower bound or upper bound is out of range");
    ASSERT(updateF_ele || updateF_seg, "You should define your update function.");
    STNode<T, NT> *cur_node_p = this->root_p;
    update(ul, ur, cur_node_p, u_val);
  }

  /**
   * @brief update a single element [u]
   *
   * @param ul: the left of the interval
   * @param ur: the right of the interval
   * @param u_val: the value by which to update the value maintained by the node
   */
  void update(const T &u, const NT& u_val) {
    ASSERT(u >= root_p->l && u <= root_p->r, "lower bound or upper bound is out of range");
    ASSERT(updateF_ele || updateF_seg, "You should define your update function.");
    STNode<T, NT> *cur_node_p = this->root_p;
    update(u, cur_node_p, u_val);
  }


  /**
   * @brief query from a segment [ql:qr]
   * 
   * @param ql lower bound
   * @param qr upper bound
   * @return NT 
   */
  NT query(const T& ql, const T& qr) {
    ASSERT(ql >= root_p->l && qr <= root_p->r, "lower bound or upper bound is out of range");
    ASSERT(updateF_ele || updateF_seg, "You should define your update function.");
    STNode<T, NT> *cur_node_p = this->root_p;
    return query(ql, qr, cur_node_p);
  }

  /**
   * @brief query from an element [q]
   * 
   * @param q the index of the element
   * @return NT 
   */
  NT query(const T& q) {
    ASSERT(q >= root_p->l && q <= root_p->r, "lower bound or upper bound is out of range");
    ASSERT(updateF_ele || updateF_seg, "You should define your update function.");
    STNode<T, NT> *cur_node_p = this->root_p;
    return query(q, q, cur_node_p);
  }

private:
  STNode<T, NT> *root_p = nullptr;
  // 1st NT arg: val of the left child
  // 2nd NT arg: val of the right child
  // ret: val of the current node
  std::function<NT(NT,NT)> pushupF = nullptr;
  // 1st NT arg: val of the current node
  // 2nd NT arg: val for performing update
  // Take x += 3 for instance, x is the 1st arg and 3 is the second arg
  std::function<NT(NT,NT)> updateF_ele = nullptr;
  // 1st NT arg: val of the current node
  // 2nd NT arg: val for performing update
  // 1st T arg: lower bound of the segment
  // 2nd T arg: upper bound of the segment
  std::function<NT(NT,NT,T,T)> updateF_seg = nullptr;
  std::function<NT(NT, NT)> lazy_update = nullptr;
  std::vector<NT> vec = {};

  // after update the left child and the right child, we should pushup the
  // updates to update the current node
  inline void pushup(STNode<T, NT> *cur_node_p) {
    ASSERT(pushupF, "You must provide your pushup function!");
    cur_node_p->set_val(pushupF(cur_node_p->get_left()->get_val(),
                                cur_node_p->get_right()->get_val()));
  }

  inline T cut_it_half(T l, T r) {
    return (l + r) / 2;
  }

  STNode<T, NT> *build(T l, T r) {
    auto *cur_node_p = new STNode<T, NT>(l, r);
    if (l == r) {
      cur_node_p->set_val(this->vec[l]);
      return cur_node_p;
    }
    T mid = cut_it_half(l, r);
    cur_node_p->add_left(build(l, mid));
    cur_node_p->add_right(build(mid + 1, r));
    pushup(cur_node_p);
    return cur_node_p;
  }

  STNode<T, NT> *build(T l, T r, NT val) {
    auto *cur_node_p = new STNode<T, NT>(val, l, r);
    if (l == r)
      return cur_node_p;
    T mid = cut_it_half(l, r);
    cur_node_p->add_left(build(l, mid, val));
    cur_node_p->add_right(build(mid + 1, r, val));
    pushup(cur_node_p);
    return cur_node_p;
  }

  void update(const T &u, STNode<T, NT> *cur_node_p, const NT& u_val) {
    if (cur_node_p->is_leaf()) {
      cur_node_p->set_val(updateF_ele(cur_node_p->get_val(), u_val));
      return;
    }
    T mid = cut_it_half(cur_node_p->l, cur_node_p->r);
    if (u <= mid) update(u, cur_node_p->get_left(), u_val);
    else update(u, cur_node_p->get_right(), u_val);
    pushup(cur_node_p);
  }

  void spread_laziness(STNode<T, NT> *cur_node_p) {
    if (cur_node_p->is_lazy() && !cur_node_p->is_leaf()) {
      cur_node_p->get_left()->lazy_on(cur_node_p->get_lazy_val(), lazy_update);
      cur_node_p->get_right()->lazy_on(cur_node_p->get_lazy_val(), lazy_update);
      cur_node_p->get_left()->set_val(updateF_seg(cur_node_p->get_left()->get_val(), cur_node_p->get_lazy_val(), cur_node_p->get_left()->l, cur_node_p->get_left()->r));
      cur_node_p->get_right()->set_val(updateF_seg(cur_node_p->get_right()->get_val(), cur_node_p->get_lazy_val(), cur_node_p->get_right()->l, cur_node_p->get_right()->r));
      cur_node_p->lazy_off();
    }
  }

  void update(const T &ul, const T &ur,
              STNode<T, NT> *cur_node_p, const NT& u_val) {
    ASSERT(ul <= ur, "The lower bound is larger than the upper bound.");
    
    // update a segment requires lazy mark to reduce time cost.
    // However, 

    if (ul <= cur_node_p->l && ur >= cur_node_p->r) {
      cur_node_p->set_val(updateF_seg(cur_node_p->get_val(), u_val, cur_node_p->l, cur_node_p->r));
      cur_node_p->lazy_on(u_val, lazy_update);
      return;
    }
    spread_laziness(cur_node_p);
    T mid = cut_it_half(cur_node_p->l, cur_node_p->r);
    if (ul <= mid)
      update(ul, ur, cur_node_p->get_left(), u_val);
    if (ur > mid)
      update(ul, ur, cur_node_p->get_right(), u_val);
    pushup(cur_node_p);
  }

  NT query(const T& ql, const T& qr, STNode<T, NT> *cur_node_p) {
    if (ql <= cur_node_p->l && qr >= cur_node_p->r) {
      return cur_node_p->get_val();
    }
    spread_laziness(cur_node_p);
    T mid = cut_it_half(cur_node_p->l, cur_node_p->r);
    NT result; bool first_branch = false;
    if (ql <= mid) {
      first_branch = true;
      result = query(ql, qr, cur_node_p->get_left());
    }
    if (qr > mid) {
      if (first_branch) result += query(ql, qr, cur_node_p->get_right());
      else result = query(ql, qr, cur_node_p->get_right());
    }
    return result;
  }
};