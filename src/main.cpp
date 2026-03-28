#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include "MacroManager.hpp"

using namespace geode::prelude;

// ── YallBot Popup ─────────────────────────────────────────────────────────────

class YallBotPopup : public geode::Popup {
protected:
    CCLabelBMFont* m_replayLabel = nullptr;
    CCLabelBMFont* m_disabledLbl = nullptr;
    CCLabelBMFont* m_recordLbl = nullptr;
    CCLabelBMFont* m_playbackLbl = nullptr;
    CCMenuItemToggler* m_ignoreToggle = nullptr;
    int m_replayIndex = 0;

    bool init() {
        if (!Popup::init(310.f, 260.f)) return false;
        this->setTitle("YallBot.");

        // ── Mode buttons ──────────────────────────────────────────────────
        auto modeMenu = CCMenu::create();

        m_disabledLbl = CCLabelBMFont::create("Disabled", "bigFont.fnt");
        m_disabledLbl->setScale(0.4f);
        auto disabledBtn = CCMenuItemLabel::create(
            m_disabledLbl, this, menu_selector(YallBotPopup::onDisabled)
        );

        m_recordLbl = CCLabelBMFont::create("Record", "bigFont.fnt");
        m_recordLbl->setScale(0.4f);
        auto recordBtn = CCMenuItemLabel::create(
            m_recordLbl, this, menu_selector(YallBotPopup::onRecord)
        );

        m_playbackLbl = CCLabelBMFont::create("Playback", "bigFont.fnt");
        m_playbackLbl->setScale(0.4f);
        auto playbackBtn = CCMenuItemLabel::create(
            m_playbackLbl, this, menu_selector(YallBotPopup::onPlayback)
        );

        modeMenu->addChild(disabledBtn);
        modeMenu->addChild(recordBtn);
        modeMenu->addChild(playbackBtn);
        modeMenu->alignItemsHorizontallyWithPadding(14.f);
        modeMenu->setPosition({ 155.f, 220.f });
        m_mainLayer->addChild(modeMenu);
        this->updateModeColors();

        // ── Replays row ───────────────────────────────────────────────────
        auto replaysTitleLbl = CCLabelBMFont::create("Replays", "bigFont.fnt");
        replaysTitleLbl->setScale(0.4f);
        replaysTitleLbl->setPosition({ 52.f, 188.f });
        m_mainLayer->addChild(replaysTitleLbl);

        auto navBg = CCScale9Sprite::create("square02_small.png");
        navBg->setContentSize({ 148.f, 26.f });
        navBg->setOpacity(80);
        navBg->setPosition({ 220.f, 188.f });
        m_mainLayer->addChild(navBg);

        auto navMenu = CCMenu::create();
        navMenu->setPosition({ 220.f, 188.f });
        m_mainLayer->addChild(navMenu);

        auto leftSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        leftSpr->setScale(0.4f);
        auto leftBtn = CCMenuItemSpriteExtra::create(
            leftSpr, this, menu_selector(YallBotPopup::onPrev)
        );

        m_replayLabel = CCLabelBMFont::create(
            this->currentReplayName().c_str(), "chatFont.fnt"
        );
        m_replayLabel->setScale(0.5f);
        m_replayLabel->limitLabelWidth(85.f, 0.5f, 0.1f);
        auto replayLabelItem = CCMenuItemLabel::create(m_replayLabel, this, nullptr);

        auto rightSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        rightSpr->setFlipX(true);
        rightSpr->setScale(0.4f);
        auto rightBtn = CCMenuItemSpriteExtra::create(
            rightSpr, this, menu_selector(YallBotPopup::onNext)
        );

        navMenu->addChild(leftBtn);
        navMenu->addChild(replayLabelItem);
        navMenu->addChild(rightBtn);
        navMenu->alignItemsHorizontallyWithPadding(4.f);

        // ── Ignore Inputs row ─────────────────────────────────────────────
        auto ignoreMenu = CCMenu::create();
        ignoreMenu->setPosition({ 130.f, 155.f });
        m_mainLayer->addChild(ignoreMenu);

        m_ignoreToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(YallBotPopup::onIgnoreInputs), 0.65f
        );
        m_ignoreToggle->toggle(MacroManager::get().ignoreInputs);

        auto ignoreLbl = CCLabelBMFont::create("Ignore Inputs", "bigFont.fnt");
        ignoreLbl->setScale(0.38f);
        auto ignoreLblItem = CCMenuItemLabel::create(
            ignoreLbl, this, menu_selector(YallBotPopup::onIgnoreInputsLabel)
        );

        ignoreMenu->addChild(m_ignoreToggle);
        ignoreMenu->addChild(ignoreLblItem);
        ignoreMenu->alignItemsHorizontallyWithPadding(4.f);

        // Info button
        auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        infoSpr->setScale(0.5f);
        auto infoBtn = CCMenuItemSpriteExtra::create(
            infoSpr, this, menu_selector(YallBotPopup::onIgnoreInfo)
        );
        auto infoMenu = CCMenu::create();
        infoMenu->addChild(infoBtn);
        infoMenu->setPosition({ 288.f, 155.f });
        m_mainLayer->addChild(infoMenu);

        // ── Divider ───────────────────────────────────────────────────────
        auto divider = CCLayerColor::create({ 255, 255, 255, 30 }, 270.f, 1.f);
        divider->setPosition({ 20.f, 133.f });
        m_mainLayer->addChild(divider);

        // ── Action buttons ───────────────────────────────────
