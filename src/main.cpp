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

void printTestHeader(const std::string& description) {
    std::cout << "\n========================================\n";
    std::cout << "ТЕСТ: " << description << "\n";
    std::cout << "========================================\n";
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    PianoKeyboard myPiano;
    AppController controller(&myPiano);

    /*std::cout << "\n=== ТЕСТ 1: Визначення акорда по 3 нотам (До мажор) ===" << std::endl;
    controller.press(60); 
    controller.press(64); 
    controller.press(67); 

    std::cout << "\n=== ТЕСТ 2: Забуття акорду при відтисканні однієї ноти ===" << std::endl;
    controller.release(64); 

    std::cout << "\n=== ТЕСТ 3: Очищення клавіатури ===" << std::endl;
    controller.release(60);
    controller.release(67);

    std::cout << "\n=== ТЕСТ 4: Побудова акорду в порожній октаві (Ре мінор) ===" << std::endl;
    ChordCommand dMinor{2, 0, "minor"};
    controller.press(dMinor);

    std::cout << "\n=== ТЕСТ 5: Вилучення всього акорду однією командою ===" << std::endl;
    controller.release(dMinor);


    std::cout << "\n=== ТЕСТ 6: Розумна побудова акорду з прив'язкою та зверненням ===" << std::endl;
    controller.press(52);
    
    ChordCommand cMajor1stInv{0, 1, "major"}; 
    controller.press(cMajor1stInv);*/

    // 1. Подсвечивание двух аккордов сразу в разных местах
    printTestHeader("1. Два независимых аккорда (C Major и F# Minor)");
    // C Major (До первой октавы)
    controller.press(60); 
    controller.press(64); 
    controller.press(67);
    // F# Minor (Фа-диез второй октавы)
    controller.press(78); 
    controller.press(81); 
    controller.press(85);
    // ОЖИДАЕМЫЙ РЕЗУЛЬТАТ:
    // Аккорды: 2 штуки (C Major, F# Minor).
    // Интервалы: Внутри C (M3, m3, P5), внутри F#m (m3, M3, P5), 
    // + "перекрестные" интервалы между нотами разных октав (например, 67 и 78 = 11 полутонов = major seventh).
    
    // Очистка (предположим, у нас есть метод для сброса, либо просто отжимаем)
    controller.release(60); controller.release(64); controller.release(67);
    controller.release(78); controller.release(81); controller.release(85);


    // 2. Два разных аккорда с совпадающими нотами (A Minor и C Major)
    printTestHeader("2. Пересекающиеся аккорды (Am и Cmaj имеют общие До и Ми)");
    controller.press(57); // Ля (A3)
    controller.press(60); // До (C4)
    controller.press(64); // Ми (E4)
    controller.press(67); // Соль (G4)
    // ОЖИДАЕМЫЙ РЕЗУЛЬТАТ:
    // Аккорды: 2 штуки одновременно. A Minor (57, 60, 64) и C Major (60, 64, 67).
    // Интервалы: 6 штук (все возможные пары из 4 нот, которые есть в словаре).


    // 3. Три аккорда в разных местах
    printTestHeader("3. Три независимых аккорда");
    // C Maj (Малая октава)
    controller.press(48); controller.press(52); controller.press(55); 
    // F Maj (Первая октава)
    controller.press(65); controller.press(69); controller.press(72);
    // G Maj (Вторая октава)
    controller.press(79); controller.press(83); controller.press(86);
    // ОЖИДАЕМЫЙ РЕЗУЛЬТАТ:
    // Аккорды: 3 мажорных трезвучия.
    // Интервалы: Множество интервалов (9 нот дадут 36 комбинаций, те, что совпадут по модулю 12 с шаблонами, подсветятся).
    
    controller.release(48); controller.release(52); controller.release(55); 
    controller.release(65); controller.release(69); controller.release(72);
    controller.release(79); controller.release(83); controller.release(86);


    // 4. Комбинация одиночного нажатия и команды buildChord
    printTestHeader("4. Построение аккорда от уже нажатой ноты");
    controller.press(67); // Нажимаем просто одиночную G4 (Соль)
    // Командуем построить G Major (root=7, inversion=0)
    ChordCommand cmdG = {7, 0, "major"}; 
    controller.press(cmdG);
    // ОЖИДАЕМЫЙ РЕЗУЛЬТАТ:
    // buildChord найдет ближайшую Соль (67) благодаря findAnchorMidi.
    // Вычислит и добавит Си (71) и Ре (74). 
    // Подсветятся ноты 67, 71, 74 + аккорд G Major + 3 интервала.
    
    controller.release(67); controller.release(71); controller.release(74);


    // 5. Интервалы в разных местах
    printTestHeader("5. Изолированные интервалы");
    controller.press(60); controller.press(67); // P5 (C4 - G4)
    controller.press(76); controller.press(80); // M3 (E5 - G#5)
    // ОЖИДАЕМЫЙ РЕЗУЛЬТАТ:
    // Аккорды: 0 (нет групп по 3-4 ноты, подходящих под шаблон).
    // Интервалы: P5, M3 + перекрестные (например, 67-76 = 9 полутонов = major sixth).
    
    controller.release(60); controller.release(67); controller.release(76); controller.release(80);


    // 6. Интервалы со смежными нотами (одна нота участвует в двух)
    printTestHeader("6. Смежные интервалы (C, E, G)");
    controller.press(60); 
    controller.press(64); 
    controller.press(67); 
    // ОЖИДАЕМЫЙ РЕЗУЛЬТАТ:
    // Интервалы: C-E (M3), E-G (m3), C-G (P5).
    // Аккорд: C Major.


    // 7. Вклинивание ноты между двумя зажатыми
    printTestHeader("7. Вклинивание ноты (Динамическое обновление)");
    controller.release(64); // Отжимаем Ми. Остается только 60 и 67.
    // ОЖИДАЕМЫЙ РЕЗУЛЬТАТ ПОСЛЕ ОТЖАТИЯ: Аккорд пропадает, остается только интервал P5.
    
    controller.press(63); // Нажимаем Ми-бемоль (вклиниваем)
    // ОЖИДАЕМЫЙ РЕЗУЛЬТАТ ПОСЛЕ НАЖАТИЯ: 
    // Появляется аккорд C Minor.
    // Интервалы обновляются: C-Eb (m3), Eb-G (M3), C-G (P5).
    
    controller.release(60); controller.release(63); controller.release(67);


    // 8. Проверка проблемного места (Удаление аккорда ломает соседей)
    printTestHeader("8. ПРОБЛЕМА: Удаление пересекающегося аккорда");
    // Строим Am и Cmaj (общие ноты C4 и E4)
    controller.press(57); controller.press(60); controller.press(64); controller.press(67);
    
    // Пытаемся отжать ТОЛЬКО A minor через объект команды
    ChordCommand cmdAm = {9, 0, "minor"};
    controller.release(cmdAm);
    
    // ОЖИДАЕМЫЙ РЕЗУЛЬТАТ И БАГ:
    // Метод release(ChordCommand) берет физические ноты аккорда Am (57, 60, 64) 
    // и тупо удаляет их из currentState.
    // В итоге ноты 60 и 64 удалятся. C Major СЛОМАЕТСЯ, останется одиноко гореть нота 67 (Соль).
    // Хотя по логике, если мы отменили Am, то C и E должны были остаться для C Major!

    return 0;
}