#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "MacroManager.hpp"

using namespace geode::prelude;

// ── Hook handleButton (GD 2.2+ API) ──────────────────────────────────────────

class $modify(GJBaseGameLayer) {
    void handleButton(bool push, int button, bool player1) {
        MacroManager::get().onInput(button, !player1, push);
        GJBaseGameLayer::handleButton(push, button, player1);
    }

    void processCommands(float dt) {
        GJBaseGameLayer::processCommands(dt);
        MacroManager::get().onFrameAdvance(this);
    }
};

// ── Reset on death/restart ────────────────────────────────────────────────────

class $modify(PlayLayer) {
    void resetLevel() {
        auto& bot = MacroManager::get();
        if (bot.state == BotState::Playing) {
            bot.currentFrame = 0;
            bot.playbackIndex = 0;
        } else if (bot.state == BotState::Recording) {
            bot.startRecording();
        }
        PlayLayer::resetLevel();
    }

    void onQuit() {
        MacroManager::get().stopPlayback();
        MacroManager::get().stopRecording();
        PlayLayer::onQuit();
    }
};

// ── Pause menu UI — name the class explicitly so menu_selector works ──────────

class $modify(YallBotPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        if (!Mod::get()->getSettingValue<bool>("show-ui-button")) return;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto label = CCLabelBMFont::create("YallBot.", "bigFont.fnt");
        label->setScale(0.5f);

        auto btn = CCMenuItemLabel::create(
            label, this,
            menu_selector(YallBotPauseLayer::onYallBotMenu)
        );

        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setPosition(winSize.width / 2, 20.f);
        this->addChild(menu, 10);
    }

    void onYallBotMenu(CCObject*) {
        auto& bot = MacroManager::get();

        std::string txt;
        switch (bot.state) {
            case BotState::Idle:
                txt = "<cr>Status: Idle</c>\n\nInputs: ";
                txt += std::to_string(bot.inputs.size());
                txt += "\n\nToggle to ";
                txt += bot.inputs.empty() ? "<cg>Record</c>" : "<cy>Playback</c>";
                break;
            case BotState::Recording:
                txt = "<cg>Recording...</c>\n\nFrame: ";
                txt += std::to_string(bot.currentFrame);
                txt += "\nInputs: ";
                txt += std::to_string(bot.inputs.size());
                txt += "\n\nToggle to <cr>Stop</c>";
                break;
            case BotState::Playing:
                txt = "<cy>Playing back...</c>\n\nFrame: ";
                txt += std::to_string(bot.currentFrame);
                txt += "\n\nToggle to <cr>Stop</c>";
                break;
        }

        auto alert = FLAlertLayer::create(
            this, "YallBot.", txt.c_str(), "Close", "Toggle", 300.f
        );
        alert->show();
    }

    void FLAlert_Clicked(FLAlertLayer*, bool btn2) {
        if (!btn2) return;
        auto& bot = MacroManager::get();
        switch (bot.state) {
            case BotState::Idle:
                if (!bot.inputs.empty()) bot.startPlayback();
                else bot.startRecording();
                break;
            case BotState::Recording:
                bot.stopRecording();
                bot.saveMacro("macro_" + std::to_string(bot.inputs.size()));
                break;
            case BotState::Playing:
                bot.stopPlayback();
                break;
        }
    }
};
