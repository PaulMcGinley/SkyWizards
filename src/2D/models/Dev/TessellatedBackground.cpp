//
// Created by Paul McGinley on 05/10/2024.
//

#include "TessellatedBackground.h"

#include <SFML/Graphics/Shader.hpp>

TessellatedBackground::TessellatedBackground() {
//load maintex from file
    mainTex.texture.loadFromFile("resources/tessellation.png");
    mainTex.CalculateQuads();
    mainSprite.setTexture(mainTex.texture);
    //mainSprite.setPosition(position);
    mainSprite.setColor(sf::Color(100,50,25));
    highlightTex.texture.loadFromFile("resources/tessellation_glow.png");
    highlightSprite.setColor(sf::Color(255,255,255,25));
   // highlightTex.CalculateQuads();
}

void TessellatedBackground::Draw(sf::RenderWindow& window, GameTime gameTime) {


    // sf::Shader shader;
    // shader.loadFromFile("resources/shaders/blur.frag", sf::Shader::Fragment);
    //
    // shader.setUniform("texture", sf::Shader::CurrentTexture);
    // shader.setUniform("blur_radius", 5.0f);


    window.draw(mainSprite);
    window.draw(highlightSprite);
}

void TessellatedBackground::Update(GameTime gameTime) {
   // auto c =  clampToPixel(mainSprite.getPosition());
   // mainSprite.setPosition(c);
   // highlightSprite.setPosition(c);
}

void TessellatedBackground::LateUpdate(GameTime gameTime) {
}

sf::Vector2f TessellatedBackground::clampToPixel(sf::Vector2f position) {
    return sf::Vector2f(std::round(position.x), std::round(position.y));
}

