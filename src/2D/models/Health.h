//
// Created by Paul McGinley on 07/10/2024.
//

#ifndef HEALTH_H
#define HEALTH_H

#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"
#include "managers/AssetManager.h"
#include <cmath>

#include <SFML/Graphics/Sprite.hpp>

class Health final : public IDraw, public IUpdate {
public:
        Health(float _scale, sf::Vector2f _position);

        [[nodiscard]] int GetHeartCount() const;
        [[nodiscard]] int GetCurrentHealth() const;
        [[nodiscard]] int GetTargetHealth() const;
        void ResetHealth(GameTime gameTime);
        void Damage(int amount);
        void Heal(int amount);

        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow &window, GameTime gameTime) override;

private:
        int maximumHealth;      // Maximum health
        int currentHealth;      // Current health
        int targetHealth;       // Target health to animate to
        float nextUpdateTime;   // Next time the health should be updated

        static constexpr int HEARTS_PER_ROW = 10;    // Limit to 10 hearts per row

        // Define opacity range for gradient
        static constexpr float MINIMUM_OPACITY = 5.0f;
        static constexpr float MAXIMUM_OPACITY = 90.0f;

        float scale = 1;        // scale of heath, always multiple of 2
        sf::Vector2f position;  // Screen space position

        static constexpr float UPDATE_INTERVAL = 75;    // Time between health updates (in ms)
};

#endif //HEALTH_H
