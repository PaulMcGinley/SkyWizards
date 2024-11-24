//
// Created by Paul McGinley on 07/10/2024.
//

#ifndef HEALTH_H
#define HEALTH_H

#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"
#include "managers/AssetManager.cpp"

#include <SFML/Graphics/Sprite.hpp>

class Health final : public IDraw, public IUpdate {
public:
    Health(const float _scale, const sf::Vector2f _position) {
        scale = _scale;
        position = _position;
    }

    int maxHealth = 80;         // Maximum health
    int heartCount() const {
        return static_cast<int>(std::ceil(static_cast<float>(maxHealth) / 4));
    }
    int currentHealth = 80;     // Current health
    int targetHealth = 0;       // Target health to animate to
    float nextUpdateTime = 0;   // Next time the health should be updated

    void Damage(const int amount) {
        if (targetHealth < amount)
            targetHealth = 0;
        else
            targetHealth -= amount;
    }

    void Heal(const int amount) {
        if (targetHealth + amount > maxHealth)
            targetHealth = maxHealth;
        else
            targetHealth += amount;
    }

    void Update(const GameTime gameTime) override {
        if (targetHealth == currentHealth)
            return;

        if (gameTime.TimeElapsed(nextUpdateTime)) {
            if (targetHealth > currentHealth)
                currentHealth++;
            else
                currentHealth--;

            nextUpdateTime = gameTime.NowAddMilliseconds(200);
        }
    }

    void LateUpdate(GameTime gameTime) override {
        // DEV CODE
        if (currentHealth == 0)
            targetHealth = maxHealth;

        if (currentHealth == maxHealth)
            targetHealth = 0;
    }

    void Draw(sf::RenderWindow &window, GameTime gameTime) override {

        // Calculate heart texture size, used for positioning and scaling
        // TODO: Refactor this into the constructor
        const sf::Vector2f heartSize = {
            static_cast<float> (assetManager.GetHeartImage_ptr(0)->texture.getSize().x),
            static_cast<float> (assetManager.GetHeartImage_ptr(0)->texture.getSize().y)
        };

        // Do the math
        const int hearts = heartCount();

        // Loop through each heart in reverse order to determine its state (full, three-quarters, half, quarter, or empty).
        // The `heartIndex` is set based on the current health, where each heart represents 4 health points.
        // If the current health is greater than or equal to the value of the heart's position (i * 4 + n),
        // the `heartIndex` is set to the corresponding value (4, 3, 2, or 1).
        // TODO: Maybe there is a better way to do this
        for (int i = hearts - 1; i >= 0; i--) {
            int heartIndex = 0;
            if (i * 4 + 4 <= currentHealth)
                heartIndex = 4;
            else if (i * 4 + 3 <= currentHealth)
                heartIndex = 3;
            else if (i * 4 + 2 <= currentHealth)
                heartIndex = 2;
            else if (i * 4 + 1 <= currentHealth)
                heartIndex = 1;

            sf::Sprite heartSprite;
            heartSprite.setTexture(assetManager.GetHeartImage_ptr(heartIndex)->texture);
            heartSprite.setScale(scale, scale);

            // Apply opacity gradient only to empty hearts (heartIndex == 0)
            if (heartIndex == 0) {
                // Calculate opacity gradient
                float opacityPercentage = minOpacity + (maxOpacity - minOpacity) * (float(i) / (hearts - 1));
                int alphaValue = static_cast<int>((opacityPercentage / 100.0f * 255.0f));  // Convert to alpha (0-255)

                heartSprite.setColor(sf::Color(255, 255, 255, 255 - alphaValue));  // Set gradient opacity
            } else {
                heartSprite.setColor(sf::Color(255, 255, 255, 255));  // Full opacity for non-empty hearts
            }

            // Position heart based on row and column (max 10 per row)
            const int row = i / heartsPerRow;
            const int col = i % heartsPerRow;
            heartSprite.setPosition(position.x + col * heartSize.x * scale,
                                    position.y - (row * 5 * scale) + row * heartSize.y * scale);

            window.draw(heartSprite);
        }
    }

private:
    const int heartsPerRow = 10;        // Limit to 10 hearts per row

    // Define opacity range for gradient
    const float minOpacity = 5.0f;      // 5% opacity
    const float maxOpacity = 90.0f;     // 20% opacity

    float scale = 1;
    sf::Vector2f position = {0, 0};
};

#endif //HEALTH_H
