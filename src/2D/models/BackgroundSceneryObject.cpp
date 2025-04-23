//
// Created by Paul McGinley on 10/10/2024.
//

#include "BackgroundSceneryObject.h"

BackgroundSceneryObject::BackgroundSceneryObject(TextureLibrary& library, const int start_index, const int length, const int tick, const sf::Vector2f position)
        : library(library) {

        // Set the position of the object in the world
        this->position = position;

        // Set the idle animation
        // Background objects will only use idle for their animation sequence as it is the default animation
        sequences =  {
                {AnimationType::ANIMATION_IDLE, {start_index, length, tick, nullptr, nullptr, nullptr}}
        };

        // Set the current animation of the object to idle
        ChangeAnimation(AnimationType::ANIMATION_IDLE, GameTime(), true);

        // Load the shader
        if (blurShader.loadFromFile("resources/shaders/blur.frag", sf::Shader::Fragment)) {
                shaderLoaded = true;
        }

        // Set the shader properties
        blurShader.setUniform("texture", sf::Shader::CurrentTexture); // Set the texture to for the shader to apply to
        blurShader.setUniform("blur_radius", blurRadius); // Set the blur radius property of the shader
}

void BackgroundSceneryObject::update(GameTime game_time) {

        // Calculate the frame index
        const int frame_index = sequences[current_animation].startFrame + current_animation_frame;

        // Calculate the position of the object in the world
        const float drawX = position.x + library.entries[frame_index].xOffset;
        const float drawY = position.y + library.entries[frame_index].yOffset;

        // Set the position of the sprite
        sprite.setPosition(sf::Vector2f(drawX, drawY));

        // Set the texture of the sprite
        sprite.setTexture(library.entries[frame_index].texture, true);

        // Call the base class update method which handles the tick of the animation
        IAnimate::TickAnimation(game_time);
}

void BackgroundSceneryObject::lateUpdate(GameTime gameTime) {
        // Late update the background scenery object
}

void BackgroundSceneryObject::Draw(sf::RenderWindow& window, GameTime gameTime) {

        // Check if the shader has been loaded and draw based on that
        if (shaderLoaded) {
                // Draw the sprite with the blur shader
                window.draw(sprite, &blurShader);
        } else {
                // Draw the sprite without the blur shader
                window.draw(sprite);
        }
}

