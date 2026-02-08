#ifndef RANDOMSTREAMGEN_H
#define RANDOMSTREAMGEN_H

#include <vector>
#include <string>
#include <random>

class RandomStreamGen {
private:
  std::vector<std::string> stream;
  std::string alphabet;
  std::mt19937 rng;

public:
  explicit RandomStreamGen(int seed = 42) : rng(seed) {
    alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-";
  }

  // Генерация одной случайной строки
  std::string randomString(int maxLength = 30) {
    std::uniform_int_distribution<int> lenDist(1, maxLength);
    std::uniform_int_distribution<int> charDist(0, alphabet.size() - 1);
    int len = lenDist(rng);
    std::string result;
    for (int i = 0; i < len; ++i) {
      result += alphabet[charDist(rng)];
    }
    return result;
  }

  // Генерация потока длиной n
  void generateStream(int n) {
    stream.clear();
    for (int i = 0; i < n; ++i) {
      stream.push_back(randomString());
    }
  }

  // Получить поток на определённый процент
  std::vector<std::string> getStreamPart(double fraction) {
    double cutoff = static_cast<double>(stream.size()) * fraction;
    if (cutoff > static_cast<double>(stream.size())) cutoff = static_cast<double>(stream.size());
    return std::vector<std::string>(stream.begin(), stream.begin() + cutoff);
  }

  [[nodiscard]] size_t getSize() const { return stream.size(); }
};

#endif //RANDOMSTREAMGEN_H
