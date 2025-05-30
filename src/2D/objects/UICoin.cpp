//
// Created by Paul McGinley on 30/05/2025.
//

#include "UICoin.h"

UICoin::UICoin()
        : percent(0.f) {
        SetAnimationSequences({
                {AnimationType::ANIMATION_BRONZE_COIN, {6, 6, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_SILVER_COIN, {30, 6, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_GOLD_COIN, {54, 6, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_GOLD_STAR_COIN, {66, 6, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_BLUE_STAR_COIN, {90, 6, 100, nullptr, nullptr, nullptr}},
        });

        ChangeAnimation(AnimationType::ANIMATION_BRONZE_COIN, true);
}
void UICoin::Update(GameTime gameTime) {
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
        else {
                ChangeAnimation(AnimationType::ANIMATION_BRONZE_COIN, true);
        }
}
void UICoin::LateUpdate(GameTime gameTime) { TickAnimation(gameTime); }
void UICoin::Draw(sf::RenderWindow &window, GameTime gameTime) {
        if (!visible)
                return;

        if (percent < 50.0f)
                IDraw::DrawBlend(window, "coins", GetTextureDrawIndex(), position - cellMiddle, sf::BlendAdd);
        else
                IDraw::Draw(window, "coins", GetTextureDrawIndex(), position - cellMiddle);
}
void UICoin::UpdatePercent(float value) { percent = value; }
void UICoin::SetPosition(const sf::Vector2f &position) { this->position = position; }
