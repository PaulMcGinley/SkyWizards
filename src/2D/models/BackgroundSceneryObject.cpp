//
// Created by Paul McGinley on 10/10/2024.
//

#include "BackgroundSceneryObject.h"

BackgroundSceneryObject::BackgroundSceneryObject(TextureLibrary& library, int startIndex, int length, int tick, sf::Vector2f position) : library(library) {

    this->position = position;
    sequences =  {
        {AniType::Idle, {startIndex, length, tick, nullptr, nullptr}}
    };
    ChangeAni(AniType::Idle, GameTime(), true);

    // Load the shader
    if (!blurShader.loadFromFile("resources/shaders/blur.frag", sf::Shader::Fragment)) {
        // Handle error
    }
    // When the scene is created
    // Set shader parameters
    blurShader.setUniform("texture", sf::Shader::CurrentTexture);
    blurShader.setUniform("blur_radius", blurRadius); // Example blur radius
}

void BackgroundSceneryObject::Update(GameTime gameTime) {
    //blurRadius += 0.010f;
    //blurShader.setUniform("blur_radius", blurRadius); // Example blur radius

    float drawX = position.x + library.entries[currentFrame].xOffset;
    float drawY = position.y + library.entries[currentFrame].yOffset;
    sprite.setPosition(sf::Vector2f(drawX, drawY));

    const int frameIndex = sequences[currentAni].StartFrame + currentFrame;
    sprite.setTexture(library.entries[frameIndex].texture);


    IAnimate::TickAnimation(gameTime);
}

void BackgroundSceneryObject::LateUpdate(GameTime gameTime) {
    // Late update the background scenery object
}

void BackgroundSceneryObject::Draw(sf::RenderWindow& window, GameTime gameTime) {

    window.draw(sprite, &blurShader);
}

