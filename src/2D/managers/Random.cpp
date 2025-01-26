//
// Created by Paul McGinley on 09/12/2024.
//
#pragma once

#include <random>
#include <array>

class Random {
public:
        static Random& getInstance() {
                static Random instance;
                return instance;
        }

        // Delete copy constructor and assignment operator to prevent copies
        Random(const Random&) = delete;
        void operator=(const Random&) = delete;

        // Returns a random signed int between min and max
        int Int32(const int min, const int max) {
                std::uniform_int_distribution distribution(min, max);
                return distribution(generator);
        }

        // Returns a random signed int between INT_MIN and INT_MAX
        int Int32() {
                return Int32(INT_MIN, INT_MAX);
        }

        // Returns a random signed int between 0 and max
        int Int32(const int max) {
                return Int32(0, max);
        }

        // Returns a random unsigned int between min and max
        unsigned int UInt32(const unsigned int min, const unsigned int max) {
                std::uniform_int_distribution distribution(min, max);
                return distribution(generator);
        }

        // Returns a random unsigned int between 0 and UINT_MAX
        unsigned int UInt32() {
                return UInt32(0, UINT_MAX);
        }

        // Returns a random unsigned int between 0 and max
        unsigned int UInt32(const unsigned int max) {
                return UInt32(0, max);
        }

        // Returns a random float between min and max
        float Float(float min, float max) {
            std::uniform_real_distribution<float> distribution(min, max);
            return distribution(generator);
        }

        // Returns a random float between 0.0 and 1.0
        float Float() {
            return Float(0.0f, 1.0f);
        }

        // Returns a random float between 0.0 and max
        float Float(float max) {
            return Float(0.0f, max);
        }

        // Returns a random byte (unsigned char) between min and max
        unsigned char Byte(const unsigned char min, const unsigned char max) {
                std::uniform_int_distribution distribution(min, max);
                return distribution(generator);
        }

        // Returns a random byte (unsigned char) between 0 and 255
        unsigned char Byte() {
                return Byte(0, 255);
        }

        // Returns a random bool
        bool Bool() {
                std::uniform_int_distribution distribution(0, 1);
                return distribution(generator) == 1;
        }

        // Returns a random RGB color as an array of 3 bytes
        std::array<unsigned char, 3> RGB() {
                return { Byte(), Byte(), Byte() };
        }

        // Returns a random RGBA color as an array of 4 bytes
        std::array<unsigned char, 4> RGBA() {
                return { Byte(), Byte(), Byte(), Byte() };
        }

private:
        // Private constructor to prevent instancing
        Random() = default;
        std::random_device randomDevice;
        std::mt19937 generator = std::mt19937(randomDevice());
};