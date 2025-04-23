//
// Created by Paul McGinley on 09/12/2024.
//

#include "Random.h"
#include <limits>

Random& Random::getInstance() {
        static Random instance;
        return instance;
}

int Random::Int32(const int min, const int max) {
        std::uniform_int_distribution distribution(min, max);
        return distribution(generator);
}

int Random::Int32() {
        return Int32(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
}

int Random::Int32(const int max) {
        return Int32(0, max);
}

unsigned int Random::UInt32(const unsigned int min, const unsigned int max) {
        std::uniform_int_distribution distribution(min, max);
        return distribution(generator);
}

unsigned int Random::UInt32() {
        return UInt32(0, std::numeric_limits<unsigned int>::max());
}

unsigned int Random::UInt32(const unsigned int max) {
        return UInt32(0, max);
}

float Random::Float(const float min, const float max) {
        std::uniform_real_distribution distribution(min, max);
        return distribution(generator);
}

float Random::Float() {
        return Float(0.0f, 1.0f);
}

float Random::Float(const float max) {
        return Float(0.0f, max);
}

unsigned char Random::Byte(const unsigned char min, const unsigned char max) {
        std::uniform_int_distribution distribution(min, max);
        return distribution(generator);
}

unsigned char Random::Byte() {
        return Byte(0, 255);
}

bool Random::Bool() {
        std::uniform_int_distribution distribution(0, 1);
        return distribution(generator) == 0;
}

std::array<unsigned char, 3> Random::RGB() {
        return { Byte(), Byte(), Byte() };
}

std::array<unsigned char, 4> Random::RGBA() {
        return { Byte(), Byte(), Byte(), Byte() };
}
