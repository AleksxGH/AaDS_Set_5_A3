#ifndef HYPERLOGLOGPACKED_H
#define HYPERLOGLOGPACKED_H

#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <cstdint>
#include <bitset>

class HyperLogLogPacked {
private:
    std::vector<uint64_t> regs; // упакованные регистры
    uint32_t B;                 // число бит для индекса
    uint32_t m;                 // число регистров = 2^B
    uint32_t regBits;           // бит на один регистр (6 бит достаточно)

    inline uint32_t hash(const std::string& elem) const {
        // простой std::hash, можно заменить на MurmurHash
        return static_cast<uint32_t>(std::hash<std::string>{}(elem));
    }

    uint32_t getRegister(uint32_t idx) const {
        uint32_t bitPos = idx * regBits;
        uint32_t vecIdx = bitPos / 64;
        uint32_t offset = bitPos % 64;
        uint64_t val = (regs[vecIdx] >> offset) & ((1ULL << regBits) - 1);
        return static_cast<uint32_t>(val);
    }

    void setRegister(uint32_t idx, uint32_t value) {
        if (value >= (1U << regBits)) value = (1U << regBits) - 1; // ограничиваем
        uint32_t bitPos = idx * regBits;
        uint32_t vecIdx = bitPos / 64;
        uint32_t offset = bitPos % 64;
        regs[vecIdx] &= ~(((1ULL << regBits) - 1) << offset); // очистка
        regs[vecIdx] |= (static_cast<uint64_t>(value) << offset); // установка
    }

public:
    HyperLogLogPacked(uint32_t B_) : B(B_) {
        regBits = 6;              // максимум 6 бит на регистр
        m = 1U << B;
        uint32_t totalBits = m * regBits;
        uint32_t vecSize = (totalBits + 63) / 64;
        regs.assign(vecSize, 0ULL);
    }

    void add(const std::string& elem) {
        uint32_t h = hash(elem);
        uint32_t idx = h >> (32 - B); // первые B бит для индекса
        uint32_t w = (h << B) | (1U << (B - 1)); // оставшиеся 32-B бит
        uint32_t leadingZeros = 1;
        for (uint32_t mask = (1U << (32 - B - 1)); mask; mask >>= 1) {
            if (w & mask) break;
            leadingZeros++;
        }
        setRegister(idx, std::max(getRegister(idx), leadingZeros));
    }

    double estimate() const {
        double alpha_m = 0.7213 / (1 + 1.079 / m);
        double sum = 0.0;
        uint32_t V = 0; // число нулевых регистров
        for (uint32_t i = 0; i < m; ++i) {
            uint32_t reg = getRegister(i);
            sum += 1.0 / (1 << reg);
            if (reg == 0) V++;
        }
        double E = alpha_m * m * m / sum;

        // Коррекция для малых потоков
        if (E <= 5.0/2.0 * m) { // эмпирическое условие
            if (V != 0) {
                E = m * std::log(static_cast<double>(m) / V); // Linear Counting
            }
        }
        return E;
    }
};

#endif //HYPERLOGLOGPACKED_H
