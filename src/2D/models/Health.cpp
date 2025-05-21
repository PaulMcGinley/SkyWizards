//
// Created by Paul McGinley on 20/05/2025.
//

#include "Health.h"

Health::Health(const float scale, const sf::Vector2f position)
        : maximumHealth(8)
        , currentHealth(8)
        , targetHealth(8)
        , nextUpdateTime(0)
        , scale(scale)
        , position(position) { /* Nothing in the constructor */ }

int Health::GetHeartCount() const {
        // Divide by 4 to get the number of hearts, ceil(ing) to round up
        return static_cast<int>(std::ceil(static_cast<float>(maximumHealth) / 4));
}
int Health::GetCurrentHealth() const { return currentHealth; }
int Health::GetTargetHealth() const { return targetHealth; }
// Called on player respawn
void Health::ResetHealth(GameTime gameTime) {
        currentHealth = 1;
        targetHealth = maximumHealth;
        nextUpdateTime = gameTime.NowAddMilliseconds(UPDATE_INTERVAL);
}
void Health::Damage(const int amount) {
        // Deduct the damage from the target health
        targetHealth -= amount;

        // Clamp the targetHealth between 0 and the maximum health
        targetHealth = std::clamp(targetHealth, 0, maximumHealth);
}
void Health::Heal(const int amount) {
        // Add the healing to the target health
        targetHealth += amount;

        // Clamp the targetHealth between 0 and the maximum health
        targetHealth = std::clamp(targetHealth, 0, maximumHealth);
}
void Health::Update(const GameTime gameTime) {
        if (targetHealth == currentHealth)
                return; // Nothing to do

        // If the time has elapsed, update the health
        if (gameTime.TimeElapsed(nextUpdateTime)) {
                if (targetHealth > currentHealth) {
                        currentHealth++;
                } else {
                        currentHealth--;
                }

                nextUpdateTime = gameTime.NowAddMilliseconds(UPDATE_INTERVAL);
        }
}
void Health::LateUpdate(GameTime gameTime) { /* Nothing to do */ }
void Health::Draw(sf::RenderWindow &window, GameTime gameTime) {
        // Calculate heart texture size, used for positioning and scaling
        const sf::Vector2f heartSize = {
                static_cast<float>(assetManager.TextureLibraries["hearts"].get()->entries[0].texture.getSize().x),
                static_cast<float>(assetManager.TextureLibraries["hearts"].get()->entries[0].texture.getSize().y)
        };

        // Loop through each heart in reverse order to determine its state (full, three-quarters, half, quarter, or empty)
        for (int i = GetHeartCount() - 1; i >= 0; i--) {
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
                heartSprite.setTexture(assetManager.TextureLibraries["hearts"].get()->entries[heartIndex].texture);
                heartSprite.setScale(scale, scale);

                // Apply opacity gradient only to empty hearts (heartIndex == 0)
                if (heartIndex == 0) {
                        // Calculate opacity gradient
                        float opacityPercent = MINIMUM_OPACITY + (MAXIMUM_OPACITY - MINIMUM_OPACITY) * (static_cast<float>(i) / (GetHeartCount() - 1));
                        int alphaValue = static_cast<int>((opacityPercent / 100.0f) * 255.0f);          // Convert to alpha (0-255)

                        heartSprite.setColor(sf::Color(255, 255, 255, 255 - alphaValue));  // Set gradient opacity
                } else {
                        heartSprite.setColor(sf::Color(255, 255, 255, 255));            // Full opacity for non-empty hearts
                }

                // Position heart based on row and column (max 10 per row)
                const int rowCount = i / HEARTS_PER_ROW;
                const int columnCount = i % HEARTS_PER_ROW;
                heartSprite.setPosition(
                        position.x + columnCount * heartSize.x * scale - ((i*1)%HEARTS_PER_ROW) * scale,
                        position.y - (rowCount * 5 * scale) + rowCount * heartSize.y * scale
                );

                window.draw(heartSprite);
        }
}