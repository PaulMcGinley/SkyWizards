//
// Created by Paul McGinley on 12/05/2025.
//

#include "Mob.h"
#include "Player.h"
Mob::Mob(Player *player, sf::Vector2f position, float viewRange, float moveSpeed, int health)
        : position(position)
        , walkSpeed(moveSpeed)
        , viewRange(viewRange)
        , health(health)
        , player(player)
{ /* Nothing in the constructor */}
sf::FloatRect Mob::GetCollisionBox() const { return collisionBox; }
void Mob::Damaged(int amount) {
        // Take no damage while hiding
        if (GetCurrentAnimation() == AnimationType::ANIMATION_STATIC)
                return;

        // take no damage if dead
        if (IsDead())
                return;

        health -= amount;
        if (health <= 0) {
                health = 0;
                ChangeAnimation(AnimationType::ANIMATION_DEATH, true);
        }
}
bool Mob::IsDead() { return health <= 0; }
int Mob::AwardScore() {
        if (scoreAwarded)
                return 0;

        scoreAwarded = true;
        return score;
}
