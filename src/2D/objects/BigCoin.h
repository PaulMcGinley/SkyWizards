//
// Created by Paul McGinley on 24/05/2025.
//

#ifndef BIGCOIN_H
#define BIGCOIN_H

#include "interfaces/IAnimate.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"

class BigCoin : public IAnimate, public IDraw, public IUpdate {
public:
        BigCoin();
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void UpdatePercent(float value);
        void SetPosition(const sf::Vector2f& position);

private:
        sf::Vector2f position;
        const sf::Vector2f cellMiddle = {250, 250}; // Middle of the coin cell
        float percent;
        int frameStartIndex;
        static constexpr int animationLength = 6; // Number of frames in the animation

};

#endif //BIGCOIN_H
