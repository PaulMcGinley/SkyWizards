cmake_minimum_required(VERSION 3.29)
project(LoSW___BftS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Find SFML package
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

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
        enums/AniType.h
        enums/FaceDirection.h
        managers/AssetManager.cpp
        models/ParallaxBackground.h
        objects/mobs/ChestMonster.cpp
        objects/mobs/ChestMonster.h
        models/Dev/TessellatedBackground.cpp
        models/Dev/TessellatedBackground.h
        models/Dev/TessellatedFloor.cpp
        models/Health.h
        models/ParallaxObject.h
        interfaces/IScene.h
        scenes/SplashScreen.cpp
        scenes/SplashScreen.h
        scenes/DevScene.cpp
        scenes/DevScene.h
        models/BackgroundSceneryObject.h
        models/BackgroundSceneryObject.cpp
        managers/SceneManager.cpp
        enums/SceneType.h
        os/GetExecutionDirectory.h
        os/PlatformPaths.h
        models/Island.h
        managers/Random.cpp
        managers/InputManager.cpp
        scenes/MainMenu.cpp
        scenes/MainMenu.h
        managers/GameManager.cpp
        managers/IOManager.cpp
        managers/IOManager.h
        IO/GameSettingsFile.cpp
        IO/GameSettingsFile.h
        models/Level.cpp
        models/Level.h
        Envir/SWS.cpp
        Envir/SWS.h
        3rdParty/pugixml/pugiconfig.hpp
        3rdParty/pugixml/pugixml.cpp
        models/LevelObject/Boundary.h
        models/LevelObject/BoundaryGroup.h
        models/LevelObject/Graphic.h
        models/LevelObject/OLibrary.h
        models/MapObject/WMObject.h
        models/MapObject/WMap.h
)


include_directories(${CMAKE_SOURCE_DIR}) # Allows me to add code from the root dir

# Link SFML libraries
target_link_libraries(LoSW___BftS sfml-graphics sfml-window sfml-system) # Add SFML libraries

# Copy SFML libraries to the build directory
add_custom_command(TARGET LoSW___BftS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SFML_DIR}/../../../lib $<TARGET_FILE_DIR:LoSW___BftS>/lib
)