// Passed Luogu P3372: https://www.luogu.com.cn/problem/P3372

#include "../SegmentTree.h"
#include <bits/stdc++.h>

using namespace std;
using ll = long long;

int main() {
  int n, m;
  cin >> n >> m;
  vector<ll> vec(n);
  for (int i = 0; i < n; i++) {
    cin >> vec[i];
  }
  SegmentTree<ll, ll> st(vec, [](ll x, ll y) { return x + y; });
  st.set_update_function(
      [](ll cur, ll val, ll l, ll r) { return cur + (r - l + 1) * 1ll * val; });
  st.set_lazy_update(
      [](ll lazy, ll another_lazy) { return lazy + another_lazy; });
  int a;
  ll x, y;
  ll k;
  for (int i = 0; i < m; i++) {
    cin >> a;
    if (a == 1) {
      cin >> x >> y >> k;
      st.update(x - 1, y - 1, k);
    } else {
      cin >> x >> y;
      cout << st.query(x - 1, y - 1) << endl;
    }
  }
}