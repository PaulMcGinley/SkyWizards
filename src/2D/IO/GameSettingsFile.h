//
// Created by Paul McGinley on 26/01/2025.
//

#ifndef GAMESETTINGSFILE_H
#define GAMESETTINGSFILE_H



class GameSettingsFile {
public:
        GameSettingsFile();

        void LoadSettings();
        void SaveSettings() const;

        int GetResolutionWidth() const;
        int GetResolutionHeight() const;
        int GetFrameRateLimit() const;
        bool IsFullscreenMode() const;
        bool IsVsyncEnabled() const;

private:
        constexpr static const char* SETTINGS_FILE_PATH = "settings.ini";

        int resolutionWidth;
        int resolutionHeight;
        int frameRateLimit;
        bool fullscreenMode;
        bool vsyncEnabled;
};



#endif //GAMESETTINGSFILE_H
