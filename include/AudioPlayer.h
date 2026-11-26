#pragma once
#include <atomic>
#include <chrono>
#include <cstddef>
#include <mutex>
#include <thread>
#include <vector>
#include "AppController.h"
#include "DataTypes.h"

/// @brief Represents a single press or release event on the flat playback timeline.
struct PlaybackEvent
{
    double time;
    int pitch;
    bool isPress;
};

/// @brief Snapshot of current playback state for status reporting.
struct PlayerStatus
{
    bool isPlaying;
    double currentTime;
    double totalDuration;
};

/// @brief Background event driver that plays NoteEvent timelines into AppController.
class AudioPlayer
{
    private:
        AppController& controller;
        std::mutex& appMutex;
        std::vector<PlaybackEvent> timeline;
        std::size_t nextEventIndex;
        double baseTrackOffset;
        double totalDuration;
        bool isPlaying;
        std::chrono::steady_clock::time_point playStartSteady;
        std::atomic<bool> isRunning;
        std::thread workerThread;

        void workerLoop();
        double computeCurrentTimeLocked() const;

    public:
        /// @brief Constructs the player and starts its background worker thread.
        /// @param controller Reference to the application controller.
        /// @param appMutex Shared mutex guarding controller and player state.
        AudioPlayer(AppController& controller, std::mutex& appMutex);

        /// @brief Stops the background worker thread and joins it cleanly.
        ~AudioPlayer();

        AudioPlayer(const AudioPlayer&) = delete;
        AudioPlayer& operator=(const AudioPlayer&) = delete;

        /// @brief Replaces the playback timeline from transcribed note events.
        /// @param notes Sequence of recognized NoteEvent structs.
        void load(const std::vector<NoteEvent>& notes);

        /// @brief Starts or resumes playback from the current track offset.
        void play();

        /// @brief Pauses playback while keeping currently active notes held in State.
        void pause();

        /// @brief Seeks to a target timestamp and synchronizes active notes in State.
        /// @param targetTime Target position in seconds.
        void seek(double targetTime);

        /// @brief Returns whether playback is active (caller must hold appMutex).
        bool getIsPlaying() const;

        /// @brief Returns the current playback position in seconds (caller must hold appMutex).
        double getCurrentTime() const;

        /// @brief Returns the total loaded track duration in seconds (caller must hold appMutex).
        double getTotalDuration() const;

        /// @brief Returns a status snapshot (caller must hold appMutex).
        PlayerStatus getStatus() const;
};
