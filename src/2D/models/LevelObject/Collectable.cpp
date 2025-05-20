//
// Created by Paul McGinley on 18/05/2025.
//

#include "Collectable.h"

#include "managers/GameManager.h"

Collectable::Collectable() = default;
Collectable::Collectable(const Collectable &other) {
        // Copy constructor
        Library = other.Library;
        startIndex = other.startIndex;
        endIndex = other.endIndex;
        animationTick = other.animationTick;
        position = other.position;
        collisionBox = other.collisionBox;

        // Copy the animation sequences
        SetAnimationSequences(other.GetAnimationSequences());

        ChangeAnimation(AnimationType::ANIMATION_IDLE, true);
}
void Collectable::Draw(sf::RenderWindow &window, GameTime gameTime) {
        // Use the IDraw interface to draw the collectable by library name and index
        IDraw::Draw(window, Library, GetTextureDrawIndex(), position);

        // Debug: Collision box
        if (GameManager::getInstance().ShowCollisions()) {
                sf::RectangleShape debugRect(sf::Vector2f(collisionBox.getPosition().x, collisionBox.getPosition().y));
                debugRect.setFillColor(sf::Color(0, 0, 0, 150));
                debugRect.setPosition(GetPosition());
                window.draw(debugRect);
        }
}
void Collectable::Update(GameTime gameTime) { /* Nothing to update */ }
void Collectable::LateUpdate(GameTime gameTime) { TickAnimation(gameTime); }
void Collectable::Deserialize(const pugi::xml_node &node) {
        // There should only be one graphic node in the XML
        // Find the first Graphic node in Images
        pugi::xml_node graphicNode = node.child("OLibrary").child("Images").child("Graphic");
        if (!graphicNode) {
                std::cerr << "Collectable: Graphic node not found in XML." << std::endl;
                return; // Graphic node is required
        }

        // Extract graphic information
        Library = graphicNode.child("BackImageLibrary").text().as_string();
        startIndex = graphicNode.child("BackIndex").text().as_int();
        endIndex = graphicNode.child("BackEndIndex").text().as_int();
        animationTick = graphicNode.child("BackAnimationSpeed").text().as_int();

        // Get position from X and Y in the Graphic node
        position.x = static_cast<float>(graphicNode.child("X").text().as_int());
        position.y = static_cast<float>(graphicNode.child("Y").text().as_int());

        // There should only be one Boundary node in the XML
        pugi::xml_node boundaryNode = node.child("OLibrary").child("BoundaryGroups").child("BoundaryGroup").child("Boundaries").child("Boundary");

        if (boundaryNode) {
                // Set collision box from boundary information
                collisionBox.left = static_cast<float>(boundaryNode.child("X").text().as_int());
                collisionBox.top = static_cast<float>(boundaryNode.child("Y").text().as_int());
                collisionBox.width = static_cast<float>(boundaryNode.child("Width").text().as_int());
                collisionBox.height = static_cast<float>(boundaryNode.child("Height").text().as_int());
        }

        SetAnimationSequences(
                {{AnimationType::ANIMATION_IDLE, {startIndex, endIndex - startIndex, animationTick}},
                {AnimationType::ANIMATION_STATIC, {startIndex, 1, animationTick}}}
        );
}
sf::FloatRect Collectable::GetCollisionBox() const { return collisionBox; }
void Collectable::SetPosition(const float x, const float y) {
        // Set the position of the collectable
        position.x = x;
        position.y = y;

        // Update the collision box position
        collisionBox.left += x;
        collisionBox.top += y;
}
sf::Vector2f Collectable::GetPosition() const { return position; }
