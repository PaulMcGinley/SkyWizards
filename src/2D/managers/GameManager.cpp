//
// Created by Paul McGinley on 26/01/2025.
//
class GameManager {
public:
        // Method to get the instance of the singleton
        static GameManager& getInstance() {
                static GameManager instance; // Guaranteed to be destroyed.
                // Instantiated on first use.
                return instance;
        }

        // Delete copy constructor and assignment operator to prevent copies
        GameManager(const GameManager&) = delete;
        void operator=(const GameManager&) = delete;

        // Get the ratio of the screen resolution
        [[nodiscard]] float resolutionRatio() const {
                return static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        }

        [[nodiscard]] int getScreenWidth() const {
                return screenWidth;
        }

        [[nodiscard]] int getScreenHeight() const {
                return screenHeight;
        }

        void setScreenWidth(const int width) {
                screenWidth = width;
        }

        void setScreenHeight(const int height) {
                screenHeight = height;
        }

        void setResolution(const int width, const int height) {
                screenWidth = width;
                screenHeight = height;
        }

        void toggleFullscreen() {
                fullscreen = !fullscreen;
        }

        [[nodiscard]] bool isFullscreen() const {
                return fullscreen;
        }

        void toggleExclusiveFullscreen() {
                exclusiveFullscreen = !exclusiveFullscreen;
        }

        [[nodiscard]] bool isExclusiveFullscreen() const {
                return exclusiveFullscreen;
        }

        void toggleVsync() {
                vsync = !vsync;
        }

        [[nodiscard]] bool isVsync() const {
                return vsync;
        }



private:
        // Private constructor to prevent instancing
        GameManager() = default;

        int screenWidth = 1920;
        int screenHeight = 1080;

        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool vsync = true;
        bool showFPS = false;
        bool showDebug = false;
        bool showMemory = false;
        bool showCursor = true;
};