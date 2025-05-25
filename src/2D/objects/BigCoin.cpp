//
// Created by Paul McGinley on 24/05/2025.
//

#include "BigCoin.h"
BigCoin::BigCoin()
        : percent(0.f) {
        SetAnimationSequences({
                {AnimationType::ANIMATION_BRONZE_COIN, {102, 6, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_SILVER_COIN, {126, 6, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_GOLD_COIN, {150, 6, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_GOLD_STAR_COIN, {162, 6, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_BLUE_STAR_COIN, {186, 6, 100, nullptr, nullptr, nullptr}},
        });

        ChangeAnimation(AnimationType::ANIMATION_BRONZE_COIN, true);
}
void BigCoin::Update(GameTime gameTime) {
        if (percent > 100.f) {
                ChangeAnimation(AnimationType::ANIMATION_BLUE_STAR_COIN, true);
        }
        else if (percent > 99.9f) {
                ChangeAnimation(AnimationType::ANIMATION_GOLD_STAR_COIN, true);
        }
        else if (percent > 80.f) {
                ChangeAnimation(AnimationType::ANIMATION_GOLD_COIN, true);
        }
        else if (percent > 60.f) {
                ChangeAnimation(AnimationType::ANIMATION_SILVER_COIN, true);
        }
}
void BigCoin::LateUpdate(GameTime gameTime) { TickAnimation(gameTime); }
void BigCoin::Draw(sf::RenderWindow &window, GameTime gameTime) {
        if (!visible) return;
        // Draw shadow
        IDraw::Draw(window, "PrgUse", 8, position - cellMiddle + sf::Vector2f(125, 400));

        if (percent < 50.0f)
                IDraw::DrawBlend(window, "coins", GetTextureDrawIndex(), position - cellMiddle, sf::BlendAdd);
        else
                IDraw::Draw(window, "coins", GetTextureDrawIndex(), position - cellMiddle);
}
void BigCoin::UpdatePercent(float value) { percent = value; }
void BigCoin::SetPosition(const sf::Vector2f &position) { this->position = position; }
