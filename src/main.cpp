#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include "MacroManager.hpp"

using namespace geode::prelude;

// ── YallBot Popup ─────────────────────────────────────────────────────────────

class YallBotPopup : public geode::Popup<> {
protected:
    CCLabelBMFont* m_replayLabel = nullptr;
    CCLabelBMFont* m_disabledLbl = nullptr;
    CCLabelBMFont* m_recordLbl = nullptr;
    CCLabelBMFont* m_playbackLbl = nullptr;
    CCMenuItemToggler* m_ignoreToggle = nullptr;
    int m_replayIndex = 0;

    bool setup() {
        this->setTitle("YallBot.");

        auto sz = m_mainLayer->getContentSize();
        float cx = sz.width / 2;
        float top = sz.height - 30.f;

        // ── Mode buttons ──────────────────────────────────────────────────
        auto modeMenu = CCMenu::create();
        modeMenu->setPosition({ cx, top - 20.f });
        m_mainLayer->addChild(modeMenu);

        m_disabledLbl = CCLabelBMFont::create("Disabled", "bigFont.fnt");
        m_disabledLbl->setScale(0.4f);
        auto disabledBtn = CCMenuItemLabel::create(
            m_disabledLbl, static_cast<CCObject*>(this),
            menu_selector(YallBotPopup::onDisabled)
        );

        m_recordLbl = CCLabelBMFont::create("Record", "bigFont.fnt");
        m_recordLbl->setScale(0.4f);
        auto recordBtn = CCMenuItemLabel::create(
            m_recordLbl, static_cast<CCObject*>(this),
            menu_selector(YallBotPopup::onRecord)
        );

        m_playbackLbl = CCLabelBMFont::create("Playback", "bigFont.fnt");
        m_playbackLbl->setScale(0.4f);
        auto playbackBtn = CCMenuItemLabel::create(
            m_playbackLbl, static_cast<CCObject*>(this),
            menu_selector(YallBotPopup::onPlayback)
        );

        modeMenu->addChild(disabledBtn);
        modeMenu->addChild(recordBtn);
        modeMenu->addChild(playbackBtn);
        modeMenu->alignItemsHorizontallyWithPadding(14.f);
        this->updateModeColors();

        // ── Replay label ──────────────────────────────────────────────────
        auto navLabel = CCLabelBMFont::create("Replays", "bigFont.fnt");
        navLabel->setScale(0.4f);
        navLabel->setPosition({ cx - 60.f, top - 55.f });
        m_mainLayer->addChild(navLabel);

        // Replay navigator bg
        auto navBg = CCScale9Sprite::create("square02_small.png");
        navBg->setContentSize({ 150.f, 26.f });
        navBg->setOpacity(80);
        navBg->setPosition({ cx + 45.f, top - 55.f });
        m_mainLayer->addChild(navBg);

        auto navMenu = CCMenu::create();
        navMenu->setPosition({ cx + 45.f, top - 55.f });
        m_mainLayer->addChild(navMenu);

        auto leftSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        leftSpr->setScale(0.4f);
        auto leftBtn = CCMenuItemSpriteExtra::create(
            leftSpr, static_cast<CCObject*>(this),
            menu_selector(YallBotPopup::onPrev)
        );

        m_replayLabel = CCLabelBMFont::create(
            this->currentReplayName().c_str(), "chatFont.fnt"
        );
        m_replayLabel->setScale(0.5f);
        m_replayLabel->limitLabelWidth(90.f, 0.5f, 0.1f);
        auto replayLabelItem = CCMenuItemLabel::create(m_replayLabel, nullptr, nullptr);

        auto rightSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        rightSpr->setFlipX(true);
        rightSpr->setScale(0.4f);
        auto rightBtn = CCMenuItemSpriteExtra::create(
            rightSpr, static_cast<CCObject*>(this),
            menu_selector(YallBotPopup::onNext)
        );

        navMenu->addChild(leftBtn);
        navMenu->addChild(replayLabelItem);
        navMenu->addChild(rightBtn);
        navMenu->alignItemsHorizontallyWithPadding(4.f);

        // ── Ignore Inputs toggle ──────────────────────────────────────────
        auto ignoreMenu = CCMenu::create();
        ignoreMenu->setPosition({ cx, top - 88.f });
        m_mainLayer->addChild(ignoreMenu);

        m_ignoreToggle = CCMenuItemToggler::createWithStandardSprites(
            static_cast<CCObject*>(this),
            menu_selector(YallBotPopup::onIgnoreInputs),
            0.65f
        );
        m_ignoreToggle->toggle(MacroManager::get().ignoreInputs);

        auto ignoreLbl = CCLabelBMFont::create("Ignore Inputs", "bigFont.fnt");
        ignoreLbl->setScale(0.38f);
        auto ignoreLblItem = CCMenuItemLabel::create(
            ignoreLbl, static_cast<CCObject*>(this),
            menu_selector(YallBotPopup::onIgnoreInputsLabel)
        );

        ignoreMenu->addChild(m_ignoreToggle);
        ignoreMenu->addChild(ignoreLblItem);
        ignoreMenu->alignItemsHorizontallyWithPadding(4.f);

        // ── Info button for Ignore Inputs ─────────────────────────────────
        auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        infoSpr->setScale(0.5f);
        auto infoBtn = CCMenuItemSpriteExtra::create(
            infoSpr, static_cast<CCObject*>(this),
            menu_selector(YallBotPopup::onIgnoreInfo)
        );
        auto infoMenu = CCMenu::create();
        infoMenu->addChild(infoBtn);
        infoMenu->setPosition({ sz.width - 18.f, top - 88.f });
        m_mainLayer->addChild(infoMenu);

        // ── Action buttons ────────────────────────────────────────────────
        auto btnMenu = CCMenu::create();
        btnMenu->setPosition({ cx, top - 130.f });
        m_mainLayer->addChild(btnMenu);

        btnMenu->addChild(this->makeBtn("New",    menu_selector(YallBotPopup::onNew)));
        btnMenu->addChild(this->makeBtn("Save",   menu_selector(YallBotPopup::onSave)));
        btnMenu->addChild(this->makeBtn("Load",   menu_selector(YallBotPopup::onLoad)));
        btnMenu->addChild(this->makeBtn("Delete", menu_selector(YallBotPopup::onDelete)));
        btnMenu->alignItemsHorizontallyWithPadding(5.f);

        return true;
    }

    CCMenuItemSpriteExtra* makeBtn(const char* text, SEL_MenuHandler sel) {
        auto bg = CCScale9Sprite::create("GJ_button_04.png");
        bg->setContentSize({ 65.f, 26.f });
        auto lbl = CCLabelBMFont::create(text, "bigFont.fnt");
        lbl->setScale(0.36f);
        lbl->setPosition(bg->getContentSize() / 2);
        bg->addChild(lbl);
        return CCMenuItemSpriteExtra::create(
            bg, static_cast<CCObject*>(this), sel
        );
    }

    std::string currentReplayName() {
        auto list = MacroManager::get().getReplayList();
        if (list.empty()) return "no replays";
        if (m_replayIndex >= (int)list.size()) m_replayIndex = 0;
        return list[m_replayIndex];
    }

    void updateModeColors() {
        auto& bot = MacroManager::get();
        ccColor3B grey   = { 150, 150, 150 };
        ccColor3B white  = { 255, 255, 255 };
        ccColor3B green  = { 100, 255, 100 };
        ccColor3B yellow = { 255, 220, 50  };
        m_disabledLbl->setColor(bot.state == BotState::Idle      ? white  : grey);
        m_recordLbl->setColor  (bot.state == BotState::Recording  ? green  : grey);
        m_playbackLbl->setColor(bot.state == BotState::Playing    ? yellow : grey);
    }

    void onDisabled(CCObject*) {
        MacroManager::get().stopRecording();
        MacroManager::get().stopPlayback();
        this->updateModeColors();
    }

    void onRecord(CCObject*) {
        MacroManager::get().startRecording();
        this->updateModeColors();
    }

    void onPlayback(CCObject*) {
        MacroManager::get().startPlayback();
        this->updateModeColors();
    }

    void onPrev(CCObject*) {
        auto list = MacroManager::get().getReplayList();
        if (list.empty()) return;
        m_replayIndex = (m_replayIndex - 1 + (int)list.size()) % (int)list.size();
        m_replayLabel->setString(list[m_replayIndex].c_str());
    }

    void onNext(CCObject*) {
        auto list = MacroManager::get().getReplayList();
        if (list.empty()) return;
        m_replayIndex = (m_replayIndex + 1) % (int)list.size();
        m_replayLabel->setString(list[m_replayIndex].c_str());
    }

    void onIgnoreInputs(CCObject*) {
        MacroManager::get().ignoreInputs = !MacroManager::get().ignoreInputs;
    }

    void onIgnoreInputsLabel(CCObject*) {
        MacroManager::get().ignoreInputs = !MacroManager::get().ignoreInputs;
        m_ignoreToggle->toggle(MacroManager::get().ignoreInputs);
    }

    void onIgnoreInfo(CCObject*) {
        FLAlertLayer::create(
            "Ignore Inputs",
            "Prevents your inputs from being registered when replaying.",
            "OK"
        )->show();
    }

    void onNew(CCObject*) {
        MacroManager::get().clearMacro();
        FLAlertLayer::create("YallBot.", "New macro created!", "OK")->show();
    }

    void onSave(CCObject*) {
        auto& bot = MacroManager::get();
        if (bot.inputs.empty()) {
            FLAlertLayer::create("YallBot.", "Nothing to save!", "OK")->show();
            return;
        }
        std::string name = "macro_" + std::to_string(bot.inputs.size());
        bot.saveMacro(name);
        if (m_replayLabel)
            m_replayLabel->setString(this->currentReplayName().c_str());
        FLAlertLayer::create("YallBot.", ("Saved: " + name).c_str(), "OK")->show();
    }

    void onLoad(CCObject*) {
        auto list = MacroManager::get().getReplayList();
        if (list.empty()) {
            FLAlertLayer::create("YallBot.", "No replays found!", "OK")->show();
            return;
        }
        MacroManager::get().loadMacro(list[m_replayIndex]);
        FLAlertLayer::create("YallBot.", ("Loaded: " + list[m_replayIndex]).c_str(), "OK")->show();
    }

    void onDelete(CCObject*) {
        auto list = MacroManager::get().getReplayList();
        if (list.empty()) return;
        std::string name = list[m_replayIndex];
        MacroManager::get().deleteMacro(name);
        m_replayIndex = 0;
        if (m_replayLabel)
            m_replayLabel->setString(this->currentReplayName().c_str());
    }

public:
    static YallBotPopup* create() {
        auto ret = new YallBotPopup();
        if (ret->init(310.f, 230.f)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

// ── Input hooks ───────────────────────────────────────────────────────────────

class $modify(GJBaseGameLayer) {
    void handleButton(bool push, int button, bool player1) {
        auto& bot = MacroManager::get();
        if (bot.state == BotState::Playing && bot.ignoreInputs) {
            GJBaseGameLayer::handleButton(push, button, player1);
            return;
        }
        bot.onInput(button, !player1, push);
        GJBaseGameLayer::handleButton(push, button, player1);
    }

    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
        MacroManager::get().onFrameAdvance(this);
    }
};

// ── Reset on death ────────────────────────────────────────────────────────────

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

// ── Pause menu button ─────────────────────────────────────────────────────────

class $modify(YallBotPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        if (!Mod::get()->getSettingValue<bool>("show-ui-button")) return;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->setPosition({ winSize.width / 2, 22.f });
        menu->setZOrder(10);
        this->addChild(menu);

        auto bg = CCScale9Sprite::create("GJ_button_04.png");
        bg->setContentSize({ 100.f, 26.f });
        auto lbl = CCLabelBMFont::create("YallBot.", "bigFont.fnt");
        lbl->setScale(0.4f);
        lbl->setPosition(bg->getContentSize() / 2);
        bg->addChild(lbl);

        auto btn = CCMenuItemSpriteExtra::create(
            bg, static_cast<CCObject*>(this),
            menu_selector(YallBotPauseLayer::onOpenYallBot)
        );
        menu->addChild(btn);
    }

    void onOpenYallBot(CCObject*) {
        YallBotPopup::create()->show();
    }
};
