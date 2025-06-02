//
// Created by Paul McGinley on 13/05/2025.
//

#ifndef LOADINGSCENE_H
#define LOADINGSCENE_H
#include <string>
#include <vector>

#include "interfaces/IScene.h"
#include "models/MapObject/WMap.h"

struct LibIndex {
        std::string library;
        std::vector<int> indices;
};

class LoadingScene final : public IScene {
public:
        LoadingScene();
        ~LoadingScene();

        void BuildAssetQueue(const std::string& mapName);

        void Update(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void DestroyScene() override;
        void OnScene_Activate() override;
        void OnScene_Deactivate() override;

private:
        static constexpr int ASSET_BATCH_SIZE = 50; // Amount of assets to load at once per frame tick
        std::string nextMapName; // The name of the map to load
        int nextSceneTime; // This gives us a delay before we switch scenes (helps stabilize the delta time)
        int CurrentValue = 0;
        int TargetValue = 0;
        std::queue<LibIndex> AssetQueue;
        sf::VertexArray backgroundQuad{sf::Quads, 4};
        sf::VertexArray frameQuad = sf::VertexArray(sf::Quads, 4);
        sf::VertexArray progressQuad = sf::VertexArray(sf::Quads, 4);
        sf::Texture frame;
        sf::Texture progress;
};



#endif //LOADINGSCENE_H
