#include <Geode/Bindings.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <random>
#include <string>

using namespace geode::prelude;

int g_pinsKnockedDown = 0;
bool g_isStrikeAwarded = false;
std::string g_myRoomID = "00000";
bool g_isInPrivateRoom = false;

void syncPinDropWithGlobed(int pinID);
void awardStrike();

class BowlingPin : public GameObject {
public:
    int pinID;
    bool isKnockedOver = false;

    static BowlingPin* create(int id) {
        auto ret = new BowlingPin();
        if (ret && ret->initWithSpriteFrameName("square01_001.png")) {
            ret->pinID = id;
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void knockOver() {
        if (!isKnockedOver) {
            isKnockedOver = true;
            g_pinsKnockedDown++;
            this->setVisible(false);
            log::info("Pin #{} knocked down! Total: {}", pinID, g_pinsKnockedDown);
        }
    }
};

namespace CosmicRoomManager {
    std::string generateRoomCode() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(10000, 99999);
        return std::to_string(distr(gen));
    }

    void hostNewRoom() {
        g_myRoomID = generateRoomCode();
        g_isInPrivateRoom = true;
        log::info("Hosted a new cosmic bowling room! Invite Code: {}", g_myRoomID);
    }
}

namespace GlobedAPI {
    size_t getRoomPlayerCount() {
        return 0;
    }
}

class $modify(CosmicBowlingManager, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontRunOnRestart) {
        if (!PlayLayer::init(level, useReplay, dontRunOnRestart)) {
            return false;
        }

        g_pinsKnockedDown = 0;
        g_isStrikeAwarded = false;

        if (level->m_levelID == 12345678) {
            if (!g_isInPrivateRoom) {
                CosmicRoomManager::hostNewRoom();
            }

            std::string displayText = "ROOM: " + g_myRoomID;
            auto* roomLabel = CCLabelBMFont::create(displayText.c_str(), "goldFont.fnt");
            
            roomLabel->setScale(0.5f);
            roomLabel->setOpacity(180); 
            
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            roomLabel->setPosition({ 60, winSize.height - 40 });
            roomLabel->setZOrder(999); 
            
            this->addChild(roomLabel);

            size_t currentLoungePlayers = GlobedAPI::getRoomPlayerCount();
            if (currentLoungePlayers > 8) {
                auto alert = FLAlertLayer::create("Lounge Full", "This cosmic bowling lane is limited to 8 players!", "Okey");
                alert->show();
                this->onQuit(); // FIX: Removed 'nullptr' argument to match Geode v5 signature
                return false;
            }
        }
        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        auto* player = m_player1;
        if (!player) return;

        CCRect playerBox = player->boundingBox();
        auto* objects = m_objects;
        
        // FIX: Replaced legacy CCARRAY_FOREACH macro with modern CCArrayExt iteration
        for (auto obj : CCArrayExt<cocos2d::CCObject*>(objects)) {
            auto* currentPin = dynamic_cast<BowlingPin*>(obj);
            if (currentPin && !currentPin->isKnockedOver) {
                CCRect pinBox = currentPin->boundingBox();
                if (playerBox.intersectsRect(pinBox)) {
                    currentPin->knockOver();
                    syncPinDropWithGlobed(currentPin->pinID);
                }
            }
        }

        if (g_pinsKnockedDown == 10 && !g_isStrikeAwarded) {
            g_isStrikeAwarded = true;
            awardStrike();
        }
    }
};

void syncPinDropWithGlobed(int pinID) {
    if (Loader::get()->isModLoaded("dankmeme.globed2")) {
        std::string message = "PIN_DROP:" + std::to_string(pinID) + "|ROOM:" + g_myRoomID;
        auto* targetMod = Loader::get()->getLoadedMod("dankmeme.globed2");
        if (targetMod) {
             log::info("Broadcasting Intermod Packet: {}", message);
        }
    }
}

void awardStrike() {
    log::info("STRIKE! Awarding synchronized cosmic lane points.");
    auto* alert = FLAlertLayer::create("❌ STRIKE! ❌", "You cleared the deck in the Cosmic Lounge!", "Boom!");
    alert->show();
}

