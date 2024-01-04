#pragma once

#include <vector>

class Number {
public:
  static bool isPrime(long long x) {
    if (x < 2)
      return 0;
    for (long long i = 2; i * i <= x; i++) {
      if (x % i == 0)
        return 0;
    }
    return 1;
  }

  void Eratosthenes(int n) {
    std::vector<int> prime;
    bool is_prime[n + 1];
    is_prime[0] = is_prime[1] = false;
    for (int i = 2; i <= n; ++i)
      is_prime[i] = true;
    for (int i = 2; i * i <= n; ++i) {
      if (is_prime[i]) {
        prime.push_back(i);
        for (int j = i * i; j <= n; j += i)
          is_prime[j] = false;
      }
    }
  }
};