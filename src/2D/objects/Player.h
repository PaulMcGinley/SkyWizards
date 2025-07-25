//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "Enumerators/FaceDirection.h"
#include "interfaces/IAnimate.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"
#include "models/GameTime.h"
#include "models/Health.h"
#include "models/LevelObject/Boundary.h"
#include "models/TextureEntry.h"

class AssetManager;
class GameScene;

class Player final : public IAnimate, public IUpdate, public IDraw {
public:
        Player(GameScene* game_scene);

        std::string robeLibrary = "RobeBlue";
        sf::VertexArray robeQuad{sf::Quads, 4};

        std::string staffLibrary = "Staff1";
        sf::VertexArray staffQuad{sf::Quads, 4};

        void CalculatePhysicsState(std::vector<Boundary> boundaries, GameTime gameTime);
        void Update(GameTime gameTime) override;
        void UpdateQuads();
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void TickAnimation(GameTime gameTime) override;

        FaceDirection faceDirection = FaceDirection::FACE_DIRECTION_RIGHT_PLAYER;
        sf::Vector2f position = {0, 0};

        // The player has multiple face directions so this needs to be factored in
         int DrawFrame() {
                return GetTextureDrawIndex() + faceDirection;
         }

        Health health = Health(4.0f, {25, 25});

        [[nodiscard]] bool IsFalling() const { return isFalling; }
        void SetIsFalling(const bool falling) { isFalling = falling; }

        bool GetIsDead() const { return isDead; }
        void SetIsDead(bool dead) { isDead = dead; }

        bool GetIsScreaming() const {return isScreaming; }
        void SetIsScreaming(bool screaming) { isScreaming = screaming; }

        const sf::IntRect collisionBox = {225, 200, 50, 150};
        sf::Vector2f collisionOffset() const { return position + sf::Vector2f(collisionBox.left, collisionBox.top); }
        float feetPosition() const { return position.y + collisionBox.top + collisionBox.height; }

        void IncrementCoins(int amount);
        int GetCoins() const { return coins; }

        void UpdateScore(const std::string &levelName, int score) ;
        int GetScore(const std::string &levelName);
        int GetTotalScore();
        void ClearScores() { scores.clear(); }
        void BounceUp(float amount);
        void KnockBack(sf::Vector2f amount);

        // Velocity
        sf::Vector2f velocity = {0, 0};
        sf::Vector2f acceleration = {0, 0};
        sf::Vector2f deceleration = {0, 0};
        sf::Vector2f maxVelocity = {1000, 2000};

        bool visible = true;

        void TakeDamage(int amount);

private:
        GameScene* gameScene;
        const int WALKING_SPEED = 128;
        const int RUNNING_SPEED = 800;
        const int FALLING_SPEED = 1200;
        const int JUMPING_SPEED = 700;
        const int JUMPING_HEIGHT = 20000;

        float nextMagicTime = 0;

        bool isFalling = false;
        bool isJumping = false;
        bool isDead = false;
        bool isScreaming = false;

        sf::Vector2f shadowDrawPosition;

        void CastMagic(GameTime gameTime);

        int coins = 0;
        std::map<std::string, int> scores; // levelname, score

        float nextStepSoundTime = 0.f;
        bool oddStepSound = false;
        bool oddLandStep = false;
};



#endif //PLAYER_H
