#include "../include/AudioPlayer.h"
#include <algorithm>
#include <bitset>

AudioPlayer::AudioPlayer(AppController& controller, std::mutex& appMutex)
    : controller(controller)
    , appMutex(appMutex)
    , nextEventIndex(0)
    , baseTrackOffset(0.0)
    , totalDuration(0.0)
    , isPlaying(false)
    , playStartSteady(std::chrono::steady_clock::now())
    , isRunning(true)
{
    workerThread = std::thread(&AudioPlayer::workerLoop, this);
}

AudioPlayer::~AudioPlayer()
{
    isRunning.store(false);
    if (workerThread.joinable())
    {
        workerThread.join();
    }
}

double AudioPlayer::computeCurrentTimeLocked() const
{
    if (!isPlaying)
    {
        return baseTrackOffset;
    }

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(now - playStartSteady).count();
    return baseTrackOffset + elapsed;
}

void AudioPlayer::workerLoop()
{
    while (isRunning.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::lock_guard<std::mutex> lock(appMutex);
        if (!isPlaying)
        {
            continue;
        }

        double currentTime = computeCurrentTimeLocked();
        while (nextEventIndex < timeline.size() && timeline[nextEventIndex].time <= currentTime)
        {
            const PlaybackEvent& ev = timeline[nextEventIndex];
            if (ev.isPress)
            {
                controller.press(ev.pitch);
            }
            else
            {
                controller.release(ev.pitch);
            }
            ++nextEventIndex;
        }

        if (nextEventIndex >= timeline.size())
        {
            isPlaying = false;
            baseTrackOffset = totalDuration;
            controller.releaseAll();
        }
    }
}

void AudioPlayer::load(const std::vector<NoteEvent>& notes)
{
    std::lock_guard<std::mutex> lock(appMutex);
    isPlaying = false;
    baseTrackOffset = 0.0;
    nextEventIndex = 0;
    totalDuration = 0.0;

    controller.releaseAll();

    timeline.clear();
    timeline.reserve(notes.size() * 2);

    for (const NoteEvent& note : notes)
    {
        timeline.push_back(PlaybackEvent{note.start, note.pitch, true});
        timeline.push_back(PlaybackEvent{note.end, note.pitch, false});
        if (note.end > totalDuration)
        {
            totalDuration = note.end;
        }
    }

    std::sort(timeline.begin(), timeline.end(), [](const PlaybackEvent& a, const PlaybackEvent& b)
    {
        if (a.time != b.time)
        {
            return a.time < b.time;
        }
        return !a.isPress && b.isPress;
    });
}

void AudioPlayer::play()
{
    std::lock_guard<std::mutex> lock(appMutex);
    if (isPlaying || timeline.empty())
    {
        return;
    }

    if (baseTrackOffset >= totalDuration || nextEventIndex >= timeline.size())
    {
        baseTrackOffset = 0.0;
        nextEventIndex = 0;
        controller.releaseAll();
    }

    playStartSteady = std::chrono::steady_clock::now();
    isPlaying = true;
}

void AudioPlayer::pause()
{
    std::lock_guard<std::mutex> lock(appMutex);
    if (!isPlaying)
    {
        return;
    }

    baseTrackOffset = computeCurrentTimeLocked();
    if (baseTrackOffset > totalDuration)
    {
        baseTrackOffset = totalDuration;
    }
    isPlaying = false;
}

void AudioPlayer::seek(double targetTime)
{
    std::lock_guard<std::mutex> lock(appMutex);
    if (targetTime < 0.0)
    {
        targetTime = 0.0;
    }
    else if (targetTime > totalDuration)
    {
        targetTime = totalDuration;
    }

    controller.releaseAll();

    std::bitset<128> activePitchBits;
    std::size_t idx = 0;
    for (; idx < timeline.size(); ++idx)
    {
        const PlaybackEvent& ev = timeline[idx];
        if (ev.time > targetTime)
        {
            break;
        }
        if (ev.pitch >= 0 && ev.pitch < 128)
        {
            if (ev.isPress)
            {
                activePitchBits.set(static_cast<std::size_t>(ev.pitch));
            }
            else
            {
                activePitchBits.reset(static_cast<std::size_t>(ev.pitch));
            }
        }
    }

    nextEventIndex = idx;
    for (int pitch = 0; pitch < 128; ++pitch)
    {
        if (activePitchBits.test(static_cast<std::size_t>(pitch)))
        {
            controller.press(pitch);
        }
    }

    baseTrackOffset = targetTime;
    if (nextEventIndex >= timeline.size())
    {
        isPlaying = false;
    }
    else if (isPlaying)
    {
        playStartSteady = std::chrono::steady_clock::now();
    }
}

bool AudioPlayer::getIsPlaying() const
{
    return isPlaying;
}

double AudioPlayer::getCurrentTime() const
{
    double current = computeCurrentTimeLocked();
    if (current > totalDuration)
    {
        return totalDuration;
    }
    if (current < 0.0)
    {
        return 0.0;
    }
    return current;
}

double AudioPlayer::getTotalDuration() const
{
    return totalDuration;
}

PlayerStatus AudioPlayer::getStatus() const
{
    return PlayerStatus{isPlaying, getCurrentTime(), totalDuration};
}
