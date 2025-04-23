//
// Created by Paul McGinley on 23/04/2025.
//

#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <array>

class Random {
public:
        static Random& getInstance();

        // Delete copy constructor and assignment operator to prevent copies
        Random(const Random&) = delete;
        void operator=(const Random&) = delete;

        // Returns a random signed int between min and max
        int Int32(int min, int max);

        // Returns a random signed int between INT_MIN and INT_MAX
        int Int32();

        // Returns a random signed int between 0 and max
        int Int32(int max);

        // Returns a random unsigned int between min and max
        unsigned int UInt32(unsigned int min, unsigned int max);

        // Returns a random unsigned int between 0 and UINT_MAX
        unsigned int UInt32();

        // Returns a random unsigned int between 0 and max
        unsigned int UInt32(unsigned int max);

        // Returns a random float between min and max
        float Float(float min, float max);

        // Returns a random float between 0.0 and 1.0
        float Float();

        // Returns a random float between 0.0 and max
        float Float(float max);

        // Returns a random byte (unsigned char) between min and max
        unsigned char Byte(unsigned char min, unsigned char max);

        // Returns a random byte (unsigned char) between 0 and 255
        unsigned char Byte();

        // Returns a random bool
        bool Bool();

        // Returns a random RGB color as an array of 3 bytes
        std::array<unsigned char, 3> RGB();

        // Returns a random RGBA color as an array of 4 bytes
        std::array<unsigned char, 4> RGBA();

private:
        // Private constructor to prevent instancing
        Random() = default;
        std::random_device randomDevice;
        std::mt19937 generator = std::mt19937(randomDevice());
};

#endif //RANDOM_H
