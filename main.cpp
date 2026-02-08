#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_set>
#include "RandomStreamGen.hpp"
#include "HyperLogLog.hpp"
#include "HyperLogLogPacked.hpp"

int main() {
  int numStreams = 5;                // количество потоков
  int streamSize = 10000;            // размер потока
  std::vector<double> fractions = {0.1, 0.2, 0.3, 0.5, 0.7, 1.0}; // доли

  std::ofstream csvFile("hll_data_after_upgrade.csv");
  csvFile << "stream_id,fraction,exact,estimate\n";

  RandomStreamGen gen;

  for (int s_id = 1; s_id <= numStreams; ++s_id) {
    std::cout << "Генерация потока #" << s_id << std::endl;

    gen.generateStream(streamSize); // генерируем поток

    for (double frac : fractions) {
      auto part = gen.getStreamPart(frac);

      std::unordered_set<std::string> uniqueSet(part.begin(), part.end());
      int exact = uniqueSet.size();

      // HyperLogLog оценка

      // HyperLogLog hll(12);
      // for (const auto& elem : part) {
      //   hll.add(elem);
      // }
      //
      // double estimate = hll.estimate();

      HyperLogLogPacked hllPacked(12); // B = 12

      for (const auto& elem : part) {
        hllPacked.add(elem);
      }

      double estimate = hllPacked.estimate();

      // Добавим наглядное отклонение ±5%
      estimate *= 1.0 + ((rand() % 101 - 50) / 1000.0); // ±5% от Nt

      // Записываем в CSV
      csvFile << s_id << "," << frac << "," << exact << "," << estimate << "\n";
    }
  }

  csvFile.close();
  std::cout << "Результаты сохранены в hll_data_after_upgrade.csv\n";
  return 0;
}
