//
// Created by Paul McGinley on 30/05/2025.
//

#ifndef UICOIN_H
#define UICOIN_H
#include "interfaces/IAnimate.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"


class UICoin  : public IAnimate, public IDraw, public IUpdate {
public:
        UICoin();
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void UpdatePercent(float value);
        void SetPosition(const sf::Vector2f& position);

        bool visible = true;

private:
        sf::Vector2f position;
        const sf::Vector2f cellMiddle = {50, 50}; // Middle of the coin cell
        float percent;
        int frameStartIndex;
        static constexpr int animationLength = 6; // Number of frames in the animation
};



#endif //UICOIN_H
