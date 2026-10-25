#include <windows.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include "../include/DataTypes.h"
#include "../include/AppController.h"
#include "../include/PianoKeyboard.h"
using namespace std;

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    PianoKeyboard myPiano;
    AppController controller(&myPiano);

    std::cout << "\n=== ТЕСТ 1: Определение аккорда по 3 нотам (До мажор) ===" << std::endl;
    controller.press(60); // До
    controller.press(64); // Ми
    controller.press(67); // Соль 
    // Ожидаем: горят 60, 64, 67. Аккорд: major, Тоника: 0, Бас: 60.

    std::cout << "\n=== ТЕСТ 2: Забывание аккорда при отжатии одной ноты ===" << std::endl;
    controller.release(64); 
    // Ожидаем: горят 60, 67. Аккордов нет.

    std::cout << "\n=== ТЕСТ 3: Очистка клавиатуры ===" << std::endl;
    controller.release(60);
    controller.release(67);
    // Ожидаем: ничего не нажато.

    std::cout << "\n=== ТЕСТ 4: Построение аккорда в пустой октаве (Ре минор) ===" << std::endl;
    ChordCommand dMinor{2, 0, "minor"}; // Тоника Ре (2), Основной вид (0)
    controller.press(dMinor);
    // Ожидаем: горят 62, 65, 69. Аккорд: minor, Тоника: 2, Бас: 62.

    std::cout << "\n=== ТЕСТ 5: Удаление всего аккорда одной командой ===" << std::endl;
    controller.release(dMinor);
    // Ожидаем: ничего не нажато (все 3 ноты стерты).

    std::cout << "\n=== ТЕСТ 6: Умное построение аккорда с привязкой и обращением ===" << std::endl;
    controller.press(52); // Пользователь зажал ноту Ми (64)
    
    // Команда: построй До мажор, но в ПЕРВОМ обращении (Ми-Соль-До)
    ChordCommand cMajor1stInv{0, 1, "major"}; 
    controller.press(cMajor1stInv);
    // Ожидаем: программа найдет уже зажатую Ми (64) как бас, достроит 67 и 72. 
    // На экране: горят 64, 67, 72. Аккорд: major, Тоника: 0, Бас: 64.

    return 0;
}