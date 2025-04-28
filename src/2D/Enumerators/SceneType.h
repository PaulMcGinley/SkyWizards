//
// Created by Paul McGinley on 10/10/2024.
//

#ifndef SCENEHELPER_H
#define SCENEHELPER_H

enum class SceneType {
        SCENE_LOADER = 0,
        SCENE_SPLASH = 1,
        SCENE_VIDEO = 2,
        SCENE_MAIN_MENU = 3,
        SCENE_OPTIONS = 4,
        SCENE_CREDITS = 5,
        SCENE_GAME = 6,
        SCENE_PAUSE = 7,
        SCENE_GAME_OVER = 8,
        SCENE_STATS = 9,
        SCENE_EXIT = 10,
        SCENE_DEBUG_OVERLAY = 254,
        SCENE_DEV = 255
};

#endif //SCENEHELPER_H
