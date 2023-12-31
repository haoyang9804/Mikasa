// Passed Codeforces 1895D https://codeforces.com/problemset/problem/1895/D
#include "../BinaryTrie.h"
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using pii = pair<int, int>;

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

#define LOG_VEC(vec, len)                                                      \
  for (int i = 0; i < len; i++)                                                \
    cout << #vec << "[" << i << "] = " << vec[i] << endl;

#define ALL(vec) vec.begin(), vec.end()

int main() {
  ACCERLERATE;
  int n;
  cin >> n;
  vector<int> a(n - 1);
  for (int i = 0; i < n - 1; i++)
    cin >> a[i];
  vector<int> sum(n - 1);
  sum[0] = a[0];
  for (int i = 1; i < n - 1; i++)
    sum[i] = a[i] ^ sum[i - 1];
  BinaryTrie bt;
  bt.insert(0);
  for (int i = 0; i < n - 1; i++) {
    bt.insert(sum[i]);
  }
  for (int i = 0; i < n; i++) {
    if (bt.maximum_xor(i) == n - 1) {
      cout << i << " ";
      for (int j : sum)
        cout << (i ^ j) << " ";
      break;
    }
  }
}