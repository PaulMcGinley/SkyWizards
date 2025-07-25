cmake_minimum_required(VERSION 3.29)
project(LoSW___BftS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(SFML_DIR "/opt/homebrew/opt/sfml@2/lib/cmake/SFML")
# Find SFML package
find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

add_executable(LoSW___BftS main.cpp
        Game.h
        Game.cpp
        interfaces/IAnimate.cpp
        interfaces/IAnimate.h
        interfaces/IUpdate.h
        interfaces/IDraw.h
        models/AniSequence.h
        models/GameTime.h
        models/TextureEntry.h
        models/TextureLibrary.h
        objects/Player.cpp
        objects/Player.h
        Enumerators/AniType.h
        Enumerators/FaceDirection.h
        managers/AssetManager.h
        managers/AssetManager.cpp
        objects/mobs/ChestMonster.cpp
        objects/mobs/ChestMonster.h
        models/Health.h
        interfaces/IScene.h
        scenes/SplashScreen.cpp
        scenes/SplashScreen.h
        managers/SceneManager.cpp
        Enumerators/SceneType.h
        os/GetExecutionDirectory.h
        os/PlatformPaths.h
        managers/InputManager.cpp
        scenes/MainMenu.cpp
        scenes/MainMenu.h
        managers/GameManager.cpp
        managers/IOManager.cpp
        managers/IOManager.h
        IO/GameSettingsFile.cpp
        IO/GameSettingsFile.h
        3rdParty/pugixml/pugiconfig.hpp
        3rdParty/pugixml/pugixml.cpp
        models/LevelObject/Boundary.h
        models/LevelObject/BoundaryGroup.h
        models/LevelObject/Graphic.h
        models/LevelObject/OLibrary.h
        models/MapObject/WMObject.h
        models/MapObject/WMap.h
        models/LevelObject/OLibrary.cpp
        models/LevelObject/Boundary.cpp
        models/LevelObject/BoundaryGroup.cpp
        models/LevelObject/Graphic.cpp
        models/MapObject/WMap.cpp
        models/MapObject/WMObject.cpp
        managers/SceneManager.h
        managers/GameManager.h
        scenes/GameScene.cpp
        scenes/GameScene.h
        models/TextureLibrary.cpp
        scenes/Overlays/DebugOverlay.cpp
        scenes/Overlays/DebugOverlay.h
        models/MapObject/WMMob.cpp
        models/MapObject/WMMob.h
        objects/Mob.cpp
        objects/Mob.h
        scenes/LoadingScene.cpp
        scenes/LoadingScene.h
        models/Projectile.h
        models/Projectile.cpp
        models/Projectiles/FireBall.cpp
        models/Projectiles/FireBall.h
        interfaces/IDraw.cpp
        scenes/SettingsScene.cpp
        scenes/SettingsScene.h
        models/MapObject/WMCollectable.cpp
        models/MapObject/WMCollectable.h
        models/LevelObject/Collectable.cpp
        models/LevelObject/Collectable.h
        managers/InputManager.h
        models/Health.cpp
        objects/mobs/EyeBall.cpp
        objects/mobs/EyeBall.h
        objects/mobs/HappyMushroom.cpp
        objects/mobs/HappyMushroom.h
        objects/mobs/SlimeMonster.cpp
        objects/mobs/SlimeMonster.h
        objects/mobs/Cactus.cpp
        objects/mobs/Cactus.h
        objects/mobs/AngryMushroom.cpp
        objects/mobs/AngryMushroom.h
        models/Score.h
        objects/BigCoin.cpp
        objects/BigCoin.h
        scenes/Overlays/EndOfLevel.cpp
        scenes/Overlays/EndOfLevel.h
        Enumerators/Align.h
        scenes/CreditsScene.cpp
        scenes/CreditsScene.h
        objects/UICoin.cpp
        objects/UICoin.h
        scenes/Overlays/SubmitScore.cpp
        scenes/Overlays/SubmitScore.h
        models/Leaderboard.cpp
        models/Leaderboard.h
        scenes/LeaderboardScene.cpp
        scenes/LeaderboardScene.h
        scenes/Overlays/GiveUpOverlay.cpp
        scenes/Overlays/GiveUpOverlay.h
)


include_directories(${CMAKE_SOURCE_DIR}) # Allows me to add code from the root dir

# Link SFML libraries
target_link_libraries(LoSW___BftS sfml-graphics sfml-window sfml-system sfml-audio) # Add SFML libraries

