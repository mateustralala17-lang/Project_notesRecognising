#pragma once
#include <string>

class State;

/// @brief Abstract base class for UI adapters that serialize application state to JSON.
class UIComponent
{
    public:
        /// @brief Virtual destructor.
        virtual ~UIComponent() = default;

        /// @brief Retrieves the component's unique JSON key.
        /// @return Root object key string (e.g., "keyboard", "chords", "intervals").
        virtual std::string getKey() const = 0;

        /// @brief Serializes dynamic component state into a JSON fragment.
        /// @param state Current application state reference.
        /// @return Serialized JSON string fragment.
        virtual std::string serialize(const State& state) const = 0;

        /// @brief Generates static initialization data for client-side layout.
        /// @return JSON string containing layout and range configurations.
        virtual std::string getInitData() const = 0;
};
