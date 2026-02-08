#ifndef HASHFUNCGEN_H
#define HASHFUNCGEN_H

#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

class HashFuncGen {
public:
  // 32-bit FNV-1a хеш
  static uint32_t hash32(const std::string &s) {
    uint32_t hash = 2166136261u;
    for (char c : s) {
      hash ^= static_cast<uint32_t>(c);
      hash *= 16777619u;
    }
    return hash;
  }

  // Проверка равномерности
  static void testUniformity(const std::vector<std::string>& data, int numBuckets = 10) {
    std::vector<int> counts(numBuckets, 0);
    for (const auto& s : data) {
      uint32_t h = hash32(s);
      int bucket = h % numBuckets;
      counts[bucket]++;
    }

    std::cout << "Распределение по " << numBuckets << " корзинам:\n";
    for (int i = 0; i < numBuckets; ++i) {
      std::cout << "Bucket " << i << ": " << counts[i] << "\n";
    }
  }
};

#endif //HASHFUNCGEN_H
