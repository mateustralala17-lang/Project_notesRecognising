#pragma once
#include <set>
#include <vector>
#include <algorithm>

namespace MusicUtils {
    
    template <typename Func>
    void forEachSubset(const std::set<int>& allPitches, int k, Func callback) {
        std::vector<int> pitches(allPitches.begin(), allPitches.end());
        int n = pitches.size();
        
        // Защита: если просят комбинации больше, чем есть нот, или нулевые/отрицательные — выходим
        if (k <= 0 || k > n) return; 

        // Инициализируем маску. Сначала создаем вектор из k единиц.
        // Затем растягиваем его до размера n, заполняя новые ячейки нулями.
        // Для n=4 и k=2 получится: {1, 1, 0, 0}
        std::vector<int> bitmask(k, 1);
        bitmask.resize(n, 0);

        do {
            std::set<int> subset;
            // Проходим по всей маске. Если видим 1 — берем ноту по этому индексу.
            for (int i = 0; i < n; ++i) {
                if (bitmask[i] == 1) {
                    subset.insert(pitches[i]);
                }
            }
            
            // Вызываем переданную функцию-коллбэк, отдавая ей собранное подмножество
            callback(subset); 
            
        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    }
}