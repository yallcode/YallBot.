#pragma once
#include <Geode/Geode.hpp>
#include <vector>
#include <string>

using namespace geode::prelude;

struct MacroInput {
    int frame;
    int button;
    bool player2;
    bool down;
};

enum class BotState {
    Idle,
    Recording,
    Playing
};

class MacroManager {
public:
    static MacroManager& get();

    BotState state = BotState::Idle;
    std::vector<MacroInput> inputs;
    int currentFrame = 0;
    int playbackIndex = 0;
    bool ignoreInputs = true;

    void startRecording();
    void stopRecording();
    void startPlayback();
    void stopPlayback();
    void clearMacro();

    void onFrameAdvance(GJBaseGameLayer* layer);
    void onInput(int button, bool player2, bool down);

    bool saveMacro(const std::string& name);
    bool loadMacro(const std::string& name);
    bool deleteMacro(const std::string& name);
    std::vector<std::string> getReplayList();
    std::string getSaveDir();

private:
    MacroManager() = default;
};
