#pragma once
#include "UIComponent.h"
#include "IntervalRecognizer.h"

/// @brief UI adapter that formats and serializes recognized intervals to JSON.
class IntervalMatrix : public UIComponent
{
    private:
        IntervalRecognizer intervalRecognizer; ///< Recognizer instance providing available interval templates.

    public:
        /// @brief Default constructor.
        IntervalMatrix() {};

        /// @brief Returns the JSON identifier for intervals state.
        /// @return String "intervals".
        std::string getKey() const override;

        /// @brief Serializes all active recognized intervals to a JSON array.
        /// @param state Current application state.
        /// @return JSON array string of interval objects.
        std::string serialize(const State& state) const override;

        /// @brief Generates static interval dictionary data for UI controls.
        /// @return JSON string of available distances and descriptive names.
        std::string getInitData() const override;
};
