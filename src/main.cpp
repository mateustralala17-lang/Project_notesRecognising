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

    std::cout << "\n=== ТЕСТ 1: Визначення акорда по 3 нотам (До мажор) ===" << std::endl;
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
    controller.press(cMajor1stInv);

    return 0;
}