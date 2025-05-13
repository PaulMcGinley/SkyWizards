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

class LoadingScene : public IScene {
public:
        LoadingScene();
        ~LoadingScene();

        void BuildAssetQueue(const std::string& mapName);

        void Update(const GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void DestroyScene() override;
        void OnScene_Active() override;
        void OnScene_Deactivate() override;
private:
        int CurrentValue = 0;
        int TargetValue = 0;
        WMap* map;
        std::queue<LibIndex> AssetQueue;
        int loadPerFrame = 50;
        sf::VertexArray backgroundQuad{sf::Quads, 4};
        sf::VertexArray frameQuad = sf::VertexArray(sf::Quads, 4);
        sf::VertexArray progressQuad = sf::VertexArray(sf::Quads, 4);


        sf::Texture frame;
        sf::Texture progress;
};



#endif //LOADINGSCENE_H
