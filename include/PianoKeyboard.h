#pragma once
#include "UIComponent.h"

/// @brief UI component representing an 88-key piano keyboard.
class PianoKeyboard : public UIComponent
{
    public:
        static constexpr int MIN_PITCH = 21;  ///< Lowest supported MIDI pitch (A0).
        static constexpr int MAX_PITCH = 108; ///< Highest supported MIDI pitch (C8).

        /// @brief Default constructor.
        PianoKeyboard() {};

        /// @brief Returns the JSON identifier for keyboard state.
        /// @return String "keyboard".
        std::string getKey() const override;

        /// @brief Serializes active MIDI notes within the keyboard range to a JSON array.
        /// @param state Current application state.
        /// @return JSON array string of active pitch numbers.
        std::string serialize(const State& state) const override;

        /// @brief Generates boundary layout data for the piano keyboard.
        /// @return JSON string containing minPitch and maxPitch.
        std::string getInitData() const override;
};
