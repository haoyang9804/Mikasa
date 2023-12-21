// Time limit exceed on Codeforces Cyclic MEX:
// https://codeforces.com/contest/1905/problem/D. Though not passed, this test
// can show the correctness of the implementation in small-size inputs.
#include "../SegmentTree.h"
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
const int MOD = 998244353;

vector<int> _ADD_MOD_arr;
int __ADD_MOD_arr_sum;
int _ADD_MOD_len;

#define ADD_MOD(...)                                                           \
  _ADD_MOD_arr = {__VA_ARGS__};                                                \
  _ADD_MOD_len = _ADD_MOD_arr.size();                                          \
  __ADD_MOD_arr_sum = 0;                                                       \
  for (int i = 0; i < _ADD_MOD_len; i++) {                                     \
    __ADD_MOD_arr_sum = (0ll + __ADD_MOD_arr_sum + _ADD_MOD_arr[i]) % MOD;     \
  }

#define ACCERLERATE                                                            \
  ios::sync_with_stdio(false);                                                 \
  cin.tie(nullptr);                                                            \
  cout.tie(nullptr);

#define INIT_VEC(vec, len)                                                     \
  for (int i = 0; i < len; i++)                                                \
    cin >> vec[i];

#define MANYCASE(...)                                                          \
  int t;                                                                       \
  cin >> t;                                                                    \
  while (t--) {                                                                \
    __VA_ARGS__                                                                \
  }

int main() {
  ACCERLERATE;
  struct S {
    int cnt, sum;
    S(int cnt, int sum) : cnt(cnt), sum(sum) {}
    S() {
      cnt = 0;
      sum = 0;
    }
    S operator+(const S &s) { return S(this->cnt + s.cnt, this->sum + s.sum); }
  };
  SegmentTree<int, S> *st;
  MANYCASE(
      int n; cin >> n; vector<int> arr(n); INIT_VEC(arr, n); bool vis[n + 1];
      memset(vis, false, sizeof(vis)); int lowp = 0;
      vector<S> cnt(n + 1, S(0, 0));
      for (int i = 0; i < n; i++) {
        vis[arr[i]] = true;
        while (vis[lowp]) {
          lowp++;
        }
        cnt[lowp].cnt++;
        cnt[lowp].sum += lowp;
      }
      st = new SegmentTree<int, S>(cnt, [](S x, S y) { return x + y; });
      st->set_update_function([](S cur, S val, int l, int r) { return val; });
      st->set_lazy_update([](S lazy, S another_lazy) { return another_lazy; });
      int res = st->query(0, n).sum;
      for (int i = 0; i < n - 1; i++) {
        S tmp = st->query(arr[i], n);
        if (arr[i] + 1 <= n - 1)
          st->update(arr[i] + 1, n - 1, S(0, 0));
        st->update(arr[i], S(tmp.cnt, tmp.cnt * arr[i]));
        res = max(res, st->query(0, n).sum);
      } cout
      << res << endl;
      delete st;)
}