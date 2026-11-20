#pragma once
#include "UIComponent.h"
#include "ChordRecognizer.h"

/// @brief UI adapter that formats and serializes recognized chords to JSON.
class ChordMatrix : public UIComponent
{
    private:
        ChordRecognizer chordRecognizer; ///< Recognizer instance providing available chord templates.

    public:
        /// @brief Default constructor.
        ChordMatrix() {};

        /// @brief Returns the JSON identifier for chords state.
        /// @return String "chords".
        std::string getKey() const override;

        /// @brief Serializes all active recognized chords to a JSON array.
        /// @param state Current application state.
        /// @return JSON array string of chord objects.
        std::string serialize(const State& state) const override;

        /// @brief Generates static chord type and inversion options for UI controls.
        /// @return JSON string describing available chord templates and inversions.
        std::string getInitData() const override;
};
