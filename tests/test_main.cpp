#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "vendor/doctest.h"
#include "AppController.h"
#include <algorithm>

// Тест 1: Распознавание аккорда по нажатым нотам (C Major)
TEST_CASE("1. Chord recognition from pressed notes (C Major triad)") {
    AppController ctrl;
    ctrl.press(60); // C4
    ctrl.press(64); // E4
    ctrl.press(67); // G4

    const auto& state = ctrl.getState();
    REQUIRE(state.getPitches().size() == 3);
    REQUIRE(state.getChords().size() == 1);
    CHECK(state.getChords().begin()->rootNote == 0); // C
    CHECK(state.getChords().begin()->chordType == "major");
    CHECK(state.getChords().begin()->inversionType == 0);
}

// Тест 2: Распознавание 3 интервалов по трём нажатым нотам (C4, E4, G4 -> M3, P5, m3)
TEST_CASE("2. Interval recognition across three pressed notes") {
    AppController ctrl;
    ctrl.press(60); // C4
    ctrl.press(64); // E4
    ctrl.press(67); // G4

    const auto& state = ctrl.getState();
    // 3 ноты образуют C(3,2) = 3 пары: (C4,E4)=4 (M3), (C4,G4)=7 (P5), (E4,G4)=3 (m3)
    REQUIRE(state.getIntervals().size() == 3);

    auto hasDistance = [&](int targetDist) {
        return std::any_of(state.getIntervals().begin(), state.getIntervals().end(),
            [targetDist](const auto& iv) { return iv.distance == targetDist; });
    };

    CHECK(hasDistance(3)); // Minor third (E4 - G4)
    CHECK(hasDistance(4)); // Major third (C4 - E4)
    CHECK(hasDistance(7)); // Perfect fifth (C4 - G4)
}

// Тест 3: Распознавание правильного обращения аккорда (1-е обращение C Major: E4, G4, C5)
TEST_CASE("3. Chord inversion recognition (1st inversion C Major)") {
    AppController ctrl;
    ctrl.press(64); // E4 (терция в басу)
    ctrl.press(67); // G4
    ctrl.press(72); // C5

    const auto& state = ctrl.getState();
    REQUIRE(state.getChords().size() == 1);
    CHECK(state.getChords().begin()->rootNote == 0);        // Тоника всё равно C (0)
    CHECK(state.getChords().begin()->chordType == "major");
    CHECK(state.getChords().begin()->inversionType == 1);   // Первое обращение (секстаккорд)
}

// Тест 4: Построение аккорда поверх уже зажатой ноты в высоком регистре
TEST_CASE("4. Building a chord while a high note is already held") {
    AppController ctrl;
    ctrl.press(84); // C6 (уже зажата наверху клавиатуры)

    // Строим аккорд D minor (root = 2, type = "minor", inversion = 0 -> D4=62, F4=65, A4=69)
    ctrl.press(ChordCommand{2, 0, "minor"});

    const auto& state = ctrl.getState();
    // На клавиатуре должны быть зажаты 4 ноты: исходная C6 (84) + D4 (62), F4 (65), A4 (69)
    REQUIRE(state.getPitches().size() == 4);

    auto isPressed = [&](int pitch) {
        return state.getPitches().find(pitch) != state.getPitches().end();
    };

    CHECK(isPressed(84)); // Старая нота не сбросилась
    CHECK(isPressed(62)); // D4
    CHECK(isPressed(65)); // F4
    CHECK(isPressed(69)); // A4
}

// Тест 5: Полная синхронность состояния (нажал аккорд -> отпустил 1 ноту -> аккорд исчез, интервал остался)
TEST_CASE("5. Full state synchronization on chord press and single note release") {
    AppController ctrl;

    // Шаг 1: Нажимаем аккорд A minor (root = 9, "minor", inversion = 0 -> A4=69, C5=72, E5=76)
    ctrl.press(ChordCommand{9, 0, "minor"});
    auto state = ctrl.getState();

    REQUIRE(state.getPitches().size() == 3);
    REQUIRE(state.getChords().size() == 1);
    CHECK(state.getChords().begin()->rootNote == 9);
    CHECK(state.getChords().begin()->chordType == "minor");
    CHECK(state.getIntervals().size() == 3);

    // Шаг 2: Отпускаем верхнюю квинту E5 (76) — остаются только A4 (69) и C5 (72)
    ctrl.release(76);
    state = ctrl.getState();

    CHECK(state.getPitches().size() == 2);
    CHECK(state.getChords().empty());                      // Трезвучие распалось — аккордов больше 0
    REQUIRE(state.getIntervals().size() == 1);             // Остался ровно один интервал
    CHECK(state.getIntervals().begin()->distance == 3);    // Малая терция (m3) между A4 и C5
}
