#include "../include/httplib.h"
#include "../include/AppController.h"
#include "../include/UIComponent.h"
#include "../include/PianoKeyboard.h"
#include "../include/ChordMatrix.h"
#include "../include/IntervalMatrix.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

std::string buildCombinedJson(
    const std::vector<std::unique_ptr<UIComponent>>& components,
    const State& state)
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
        buildCombinedJson(components, controller.getState()),
        "application/json");
}

int main()
{
    AppController controller;
    std::mutex appMutex;

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
    svr.Get("/state", [&controller, &appMutex, &components](const httplib::Request&, httplib::Response& res)
    {
        std::lock_guard<std::mutex> lock(appMutex);
        res.set_content(
            buildCombinedJson(components, controller.getState()),
            "application/json");
    });

    // POST /press — press a note, chord, or interval
    svr.Post("/press", [&controller, &appMutex, &components](const httplib::Request& req, httplib::Response& res)
    {
        handleAction(req, res, controller, appMutex, components, true);
    });

    // POST /release — release a note, chord, or interval
    svr.Post("/release", [&controller, &appMutex, &components](const httplib::Request& req, httplib::Response& res)
    {
        handleAction(req, res, controller, appMutex, components, false);
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
