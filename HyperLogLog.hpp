#ifndef HYPERLOGLOG_H
#define HYPERLOGLOG_H
#include <algorithm>
#include <vector>
#include <cmath>
#include "HashFuncGen.hpp"

class HyperLogLog {
private:
  int B;
  int m;
  std::vector<int> M;
  double alpha_m;

  double calcAlpha(int m) {
    if (m == 16) return 0.673;
    if (m == 32) return 0.697;
    if (m == 64) return 0.709;
    return 0.7213 / (1 + 1.079 / m);
  }

  int leadingZeros(uint32_t x, int bits) {
    int count = 1;
    for (int i = bits - 1; i >= 0; --i) {
      if ((x >> i) & 1) break;
      count++;
    }
    return count;
  }

public:
  HyperLogLog(int B_) : B(B_) {
    m = 1 << B;
    M.assign(m, 0);
    alpha_m = calcAlpha(m);
  }

  void add(const std::string& elem) {
    uint32_t hash = HashFuncGen::hash32(elem);
    int index = hash >> (32 - B);
    uint32_t w = hash << B;
    M[index] = std::max(M[index], leadingZeros(w, 32 - B));
  }

  double estimate() {
    double sum = 0.0;
    for (int j = 0; j < m; ++j)
      sum += std::pow(2.0, -M[j]);
    double E = alpha_m * m * m / sum;

    if (E <= 5.0 / 2.0 * m) {
      int V = std::count(M.begin(), M.end(), 0);
      if (V != 0)
        E = m * std::log(static_cast<double>(m) / V);
    }
    return E;
  }
};
#endif //HYPERLOGLOG_H
