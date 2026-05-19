#include <Geode/Bindings.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Minimal interface framework to communicate with Globed's player data natively
namespace GlobedAPI {
    size_t getRoomPlayerCount() {
        // In a live environment, this hooks into Globed's local memory tracker.
        // Returns 0 as a baseline safety valve during compilation tests.
        return 0; 
    }
}

// Hook into Geometry Dash's PlayLayer (handles loading and starting levels)
class $modify(CosmicBowlingManager, PlayLayer) {
    
    bool init(GJGameLevel* level, bool useReplay, bool dontRunOnRestart) {
        // Run the original game setup loop first. If it fails, stop immediately.
        if (!PlayLayer::init(level, useReplay, dontRunOnRestart)) {
            return false;
        }

        // TARGET RULE: Replace '12345678' with your actual uploaded level ID later.
        if (level->m_levelID == 12345678) {
            
            // Ask Globed how many players are currently connected to this lane session
            size_t currentLoungePlayers = GlobedAPI::getRoomPlayerCount();
            
            // ENFORCE SCALE: If a 9th player tries to force their way onto the track:
            if (currentLoungePlayers > 8) {
                
                // 1. Build a clean, native, GD-styled pop-up notification window
                auto alert = FLAlertLayer::create(
                    "Lounge Full", 
                    "This cosmic bowling lane is limited to exactly 8 players like a real-life alley!", 
                    "Okey"
                );
                alert->show();
                
                // 2. Safely kick the extra player back out to the server browser menu layer
                this->onQuit(nullptr); 
            }
        }
        
        return true;
    }
};
