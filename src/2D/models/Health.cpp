//
// Created by Paul McGinley on 20/05/2025.
//

#include "Health.h"

Health::Health(const float _scale, const sf::Vector2f _position)
        : scale(_scale)
        , position(_position)
        , maximumHealth(4)
        , currentHealth(4)
        , targetHealth(4)
        , nextUpdateTime(0)


{ }

int Health::GetHeartCount() const {
    return static_cast<int>(std::ceil(static_cast<float>(maximumHealth) / 4)); // Divide by 4 to get the number of hearts, ceil(ing) to round up
}

int Health::GetCurrentHealth() const {
    return currentHealth;
}

int Health::GetTargetHealth() const {
    return targetHealth;
}

void Health::ResetHealth(GameTime gameTime) {
    currentHealth = 1;
    targetHealth = maximumHealth;
    nextUpdateTime = gameTime.NowAddMilliseconds(UPDATE_INTERVAL);
}

void Health::damage(const int amount) {
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
    // If the target health is equal to the current health, return
    if (targetHealth == currentHealth)
        return;

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

void Health::LateUpdate(GameTime gameTime) {
    // DEV CODE
    // if (current_health == 0)
    //     target_health = maximum_health;
    //
    // if (current_health == maximum_health)
    //     target_health = 0;
}

void Health::Draw(sf::RenderWindow &window, GameTime gameTime) {
    // Calculate heart texture size, used for positioning and scaling
    const sf::Vector2f heart_size = {
        static_cast<float>(assetManager.TextureLibraries["hearts"].get()->entries[0].texture.getSize().x),
        static_cast<float>(assetManager.TextureLibraries["hearts"].get()->entries[0].texture.getSize().y)
    };

    // Do the math
    const int heart_count = GetHeartCount();

    // Loop through each heart in reverse order to determine its state (full, three-quarters, half, quarter, or empty)
    for (int i = heart_count - 1; i >= 0; i--) {
        int heart_index = 0;
        if (i * 4 + 4 <= currentHealth)
            heart_index = 4;
        else if (i * 4 + 3 <= currentHealth)
            heart_index = 3;
        else if (i * 4 + 2 <= currentHealth)
            heart_index = 2;
        else if (i * 4 + 1 <= currentHealth)
            heart_index = 1;

        sf::Sprite heart_sprite;
        heart_sprite.setTexture(assetManager.TextureLibraries["hearts"].get()->entries[heart_index].texture);
        heart_sprite.setScale(scale, scale);

        // Apply opacity gradient only to empty hearts (heartIndex == 0)
        if (heart_index == 0) {
            // Calculate opacity gradient
            float opacity_percentage = MINIMUM_OPACITY + (MAXIMUM_OPACITY - MINIMUM_OPACITY) * (float(i) / (heart_count - 1));
            int alpha_value = static_cast<int>((opacity_percentage / 100.0f) * 255.0f);  // Convert to alpha (0-255)

            heart_sprite.setColor(sf::Color(255, 255, 255, 255 - alpha_value));  // Set gradient opacity
        } else {
            heart_sprite.setColor(sf::Color(255, 255, 255, 255));  // Full opacity for non-empty hearts
        }

        // Position heart based on row and column (max 10 per row)
        const int row_count = i / HEARTS_PER_ROW;
        const int column_count = i % HEARTS_PER_ROW;
        heart_sprite.setPosition(position.x + column_count * heart_size.x * scale - ((i*1)%HEARTS_PER_ROW) * scale,
                               position.y - (row_count * 5 * scale) + row_count * heart_size.y * scale);

        window.draw(heart_sprite);
    }
}