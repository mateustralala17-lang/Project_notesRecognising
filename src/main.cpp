#include "../include/httplib.h"
#include "../include/AppController.h"
#include "../include/AudioPlayer.h"
#include "../include/UIComponent.h"
#include "../include/PianoKeyboard.h"
#include "../include/ChordMatrix.h"
#include "../include/IntervalMatrix.h"
#include "../include/AudioTranscriber.h"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

std::string buildCombinedJson(
    const std::vector<std::unique_ptr<UIComponent>>& components,
    const State& state,
    const PlayerStatus& playerStatus)
{
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& comp : components)
    {
        if (!first)
        {
            oss << ",";
        }
        oss << "\"" << comp->getKey() << "\":" << comp->serialize(state);
        first = false;
    }

    if (!first)
    {
        oss << ",";
    }
    oss << "\"player\":{"
        << "\"isPlaying\":" << (playerStatus.isPlaying ? "true" : "false") << ","
        << "\"currentTime\":" << playerStatus.currentTime << ","
        << "\"totalDuration\":" << playerStatus.totalDuration
        << "}";

    oss << "}";
    return oss.str();
}

std::string buildInitJson(
    const std::vector<std::unique_ptr<UIComponent>>& components)
{
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& comp : components)
    {
        if (!first)
        {
            oss << ",";
        }
        oss << "\"" << comp->getKey() << "\":" << comp->getInitData();
        first = false;
    }
    oss << "}";
    return oss.str();
}

void handleAction(
    const httplib::Request& req,
    httplib::Response& res,
    AppController& controller,
    AudioPlayer& player,
    std::mutex& appMutex,
    const std::vector<std::unique_ptr<UIComponent>>& components,
    bool isPress)
{
    std::lock_guard<std::mutex> lock(appMutex);
    std::string type = req.get_param_value("type");

    try
    {
        if (type == "note")
        {
            int pitch = std::stoi(req.get_param_value("pitch"));
            if (isPress)
            {
                controller.press(pitch);
            }
            else
            {
                controller.release(pitch);
            }
        }
        else if (type == "chord")
        {
            int root = std::stoi(req.get_param_value("root"));
            int inversion = std::stoi(req.get_param_value("inversion"));
            std::string chordType = req.get_param_value("chordType");
            ChordCommand cmd{root, inversion, chordType};
            if (isPress)
            {
                controller.press(cmd);
            }
            else
            {
                controller.release(cmd);
            }
        }
        else if (type == "interval")
        {
            int distance = std::stoi(req.get_param_value("distance"));
            std::string intervalType = req.get_param_value("intervalType");
            IntervalCommand cmd{distance, intervalType};
            if (isPress)
            {
                controller.press(cmd);
            }
            else
            {
                controller.release(cmd);
            }
        }
        else
        {
            res.status = 400;
            res.set_content("{\"error\":\"unknown type\"}", "application/json");
            return;
        }
    }
    catch (const std::exception&)
    {
        res.status = 400;
        res.set_content("{\"error\":\"invalid numeric parameter\"}", "application/json");
        return;
    }

    res.set_content(
        buildCombinedJson(components, controller.getState(), player.getStatus()),
        "application/json");
}

int main(int argc, char* argv[])
{
    AppController controller;
    std::mutex appMutex;
    AudioPlayer player(controller, appMutex);
    AudioTranscriber transcriber(".venv\\Scripts\\python.exe scripts\\basicPitch.py");

    if (argc > 1)
    {
        std::string initialFilePath = argv[1];
        std::vector<NoteEvent> notes = transcriber.transcribe(initialFilePath);
        player.load(notes);
    }

    std::vector<std::unique_ptr<UIComponent>> components;
    components.push_back(std::make_unique<PianoKeyboard>());
    components.push_back(std::make_unique<ChordMatrix>());
    components.push_back(std::make_unique<IntervalMatrix>());

    httplib::Server svr;

    // GET / — serve web/index.html
    svr.Get("/", [](const httplib::Request&, httplib::Response& res)
    {
        std::ifstream file("web/index.html");
        if (file.is_open())
        {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        }
        else
        {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });

    // GET /init — static layout data
    svr.Get("/init", [&components](const httplib::Request&, httplib::Response& res)
    {
        res.set_content(buildInitJson(components), "application/json");
    });

    // GET /state — current combined state
    svr.Get("/state", [&controller, &player, &appMutex, &components](const httplib::Request&, httplib::Response& res)
    {
        std::lock_guard<std::mutex> lock(appMutex);
        res.set_content(
            buildCombinedJson(components, controller.getState(), player.getStatus()),
            "application/json");
    });

    // POST /press — press a note, chord, or interval
    svr.Post("/press", [&controller, &player, &appMutex, &components](const httplib::Request& req, httplib::Response& res)
    {
        handleAction(req, res, controller, player, appMutex, components, true);
    });

    // POST /release — release a note, chord, or interval
    svr.Post("/release", [&controller, &player, &appMutex, &components](const httplib::Request& req, httplib::Response& res)
    {
        handleAction(req, res, controller, player, appMutex, components, false);
    });

    // POST /play — start or resume playback
    svr.Post("/play", [&controller, &player, &appMutex, &components](const httplib::Request&, httplib::Response& res)
    {
        player.play();
        std::lock_guard<std::mutex> lock(appMutex);
        res.set_content(
            buildCombinedJson(components, controller.getState(), player.getStatus()),
            "application/json");
    });

    // POST /pause — pause playback while keeping active notes highlighted
    svr.Post("/pause", [&controller, &player, &appMutex, &components](const httplib::Request&, httplib::Response& res)
    {
        player.pause();
        std::lock_guard<std::mutex> lock(appMutex);
        res.set_content(
            buildCombinedJson(components, controller.getState(), player.getStatus()),
            "application/json");
    });

    // POST /seek — seek playback to a target timestamp in seconds
    svr.Post("/seek", [&controller, &player, &appMutex, &components](const httplib::Request& req, httplib::Response& res)
    {
        double targetTime = 0.0;
        try
        {
            if (!req.has_param("time"))
            {
                res.status = 400;
                res.set_content("{\"error\":\"missing time parameter\"}", "application/json");
                return;
            }
            targetTime = std::stod(req.get_param_value("time"));
        }
        catch (const std::exception&)
        {
            res.status = 400;
            res.set_content("{\"error\":\"invalid time parameter\"}", "application/json");
            return;
        }

        player.seek(targetTime);
        std::lock_guard<std::mutex> lock(appMutex);
        res.set_content(
            buildCombinedJson(components, controller.getState(), player.getStatus()),
            "application/json");
    });

    // POST /load — transcribe audio file and load timeline into AudioPlayer
    svr.Post("/load", [&controller, &player, &transcriber, &appMutex, &components](const httplib::Request& req, httplib::Response& res)
    {
        std::string filePath = req.get_param_value("file");
        if (filePath.empty())
        {
            filePath = req.body;
        }

        if (filePath.empty())
        {
            res.status = 400;
            res.set_content("{\"error\":\"missing file parameter\"}", "application/json");
            return;
        }

        std::vector<NoteEvent> notes = transcriber.transcribe(filePath);
        player.load(notes);

        std::lock_guard<std::mutex> lock(appMutex);
        res.set_content(
            buildCombinedJson(components, controller.getState(), player.getStatus()),
            "application/json");
    });

    // GET /events — Server-Sent Events stream of state and player status
    svr.Get("/events", [&controller, &player, &appMutex, &components](const httplib::Request&, httplib::Response& res)
    {
        res.set_header("Cache-Control", "no-cache");
        res.set_header("Connection", "keep-alive");

        res.set_chunked_content_provider(
            "text/event-stream",
            [&controller, &player, &appMutex, &components](std::size_t, httplib::DataSink& sink)
            {
                if (!sink.is_writable())
                {
                    return false;
                }

                std::string json;
                {
                    std::lock_guard<std::mutex> lock(appMutex);
                    json = buildCombinedJson(components, controller.getState(), player.getStatus());
                }

                std::string payload = "data: " + json + "\n\n";
                if (!sink.write(payload.data(), payload.size()))
                {
                    return false;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(40));
                return true;
            });
    });

    std::cout << "Starting server on http://localhost:8080" << std::endl;
    std::system("start http://localhost:8080");

    if (!svr.listen("0.0.0.0", 8080))
    {
        std::cerr << "Failed to start server on port 8080" << std::endl;
        return 1;
    }

    return 0;
}
