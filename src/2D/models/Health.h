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
        Health(const float _scale, const sf::Vector2f _position)
                : scale(_scale)
                , position(_position) { }

        [[nodiscard]] int getHeartCount() const {
                return static_cast<int>(std::ceil(static_cast<float>(maximum_health) / 4)); // Divide by 4 to get the number of hearts, ceil(ing) to round up
        }

        [[nodiscard]] int getCurrentHealth() const {
                return current_health;
        }

        [[nodiscard]] int getTargetHealth() const {
                return target_health;
        }

        void ResetHealth(GameTime gameTime) {
                current_health = 1;
                target_health = maximum_health;
                next_update_time = gameTime.NowAddMilliseconds(update_interval);
        }

        // Apply damage
        void damage(const int amount) {

                // Deduct the damage from the target health
                target_health -= amount;

                // Clamp the targetHealth between 0 and the maximum health
                target_health = std::clamp(target_health, 0, maximum_health);
        }

        // Apply healing
        void heal(const int amount) {

                // Add the healing to the target health
                target_health += amount;

                // Clamp the targetHealth between 0 and the maximum health
                target_health = std::clamp(target_health, 0, maximum_health);
        }

        void Update(const GameTime gameTime) override {

                // If the target health is equal to the current health, return
                if (target_health == current_health)
                        return;

                // If the time has elapsed, update the health
                if (gameTime.TimeElapsed(next_update_time)) {

                        if (target_health > current_health) {
                                current_health++;
                        } else {
                                current_health--;
                        }

                        next_update_time = gameTime.NowAddMilliseconds(update_interval);
                }
        }

        void LateUpdate(GameTime gameTime) override {
                // DEV CODE
                // if (current_health == 0)
                //     target_health = maximum_health;
                //
                // if (current_health == maximum_health)
                //     target_health = 0;
        }

        void Draw(sf::RenderWindow &window, GameTime gameTime) override {

                // Calculate heart texture size, used for positioning and scaling
                const sf::Vector2f heart_size = {
                        static_cast<float>(assetManager.TextureLibraries["hearts"].get()->entries[0].texture.getSize().x),
                        static_cast<float>(assetManager.TextureLibraries["hearts"].get()->entries[0].texture.getSize().y)
                };

                // Do the math
                const int heart_count = getHeartCount();

                // Loop through each heart in reverse order to determine its state (full, three-quarters, half, quarter, or empty).
                // The `heartIndex` is set based on the current health, where each heart represents 4 health points.
                // If the current health is greater than or equal to the value of the heart's position (i * 4 + n),
                // the `heartIndex` is set to the corresponding value (4, 3, 2, or 1).
                // TODO: Maybe there is a better way to do this
                for (int i = heart_count - 1; i >= 0; i--) {
                        int heart_index = 0;
                        if (i * 4 + 4 <= current_health)
                                heart_index = 4;
                        else if (i * 4 + 3 <= current_health)
                                heart_index = 3;
                        else if (i * 4 + 2 <= current_health)
                                heart_index = 2;
                        else if (i * 4 + 1 <= current_health)
                                heart_index = 1;

                        sf::Sprite heart_sprite;
                        heart_sprite.setTexture(assetManager.TextureLibraries["hearts"].get()->entries[heart_index].texture);
                        heart_sprite.setScale(scale, scale);

                        // Apply opacity gradient only to empty hearts (heartIndex == 0)
                        if (heart_index == 0) {
                                // Calculate opacity gradient
                                float opacity_percentage = minimum_opacity + (maximum_opacity - minimum_opacity) * (float(i) / (heart_count - 1));
                                int alpha_value = static_cast<int>((opacity_percentage / 100.0f) * 255.0f);  // Convert to alpha (0-255)

                                heart_sprite.setColor(sf::Color(255, 255, 255, 255 - alpha_value));  // Set gradient opacity
                        } else {
                                heart_sprite.setColor(sf::Color(255, 255, 255, 255));  // Full opacity for non-empty hearts
                        }

                        // Position heart based on row and column (max 10 per row)
                        const int row_count = i / hearts_per_row;
                        const int column_count = i % hearts_per_row;
                        heart_sprite.setPosition(position.x + column_count * heart_size.x * scale - ((i*1)%hearts_per_row) * scale,
                                                position.y - (row_count * 5 * scale) + row_count * heart_size.y * scale);

                        window.draw(heart_sprite);
                }
        }

private:
        int maximum_health = 4;            // Maximum health
        int current_health = 4;       // Current health
        int target_health = 4;        // Target health to animate to
        float next_update_time = 0;     // Next time the health should be updated

        const int hearts_per_row = 10;    // Limit to 10 hearts per row

        // Define opacity range for gradient
        const float minimum_opacity = 5.0f;
        const float maximum_opacity = 90.0f;

        float scale = 1;
        sf::Vector2f position = {0, 0}; // Screen position of the health bar

        float update_interval = 75;     // Time between health updates (in ms)
};

#endif //HEALTH_H
