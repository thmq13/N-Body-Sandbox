#pragma once

#include <cassert>
#include <cstddef>
#include <vector>

#include <raylib.h>

namespace NBody::Particle {
    struct ParticleSystem {
        std::vector<double> mass{};
        std::vector<double> positionX{}, positionY{}, positionZ{};
        std::vector<double> velocityX{}, velocityY{}, velocityZ{};
        std::vector<double> accelerationX{}, accelerationY{}, accelerationZ{};

        std::vector<float> radius{};
        std::vector<Color> color{};

        [[nodiscard]] std::size_t getSize() const noexcept {
            assert(mass.size() == positionX.size());
            assert(mass.size() == positionY.size());
            return mass.size();
        }

        void resize(std::size_t newSize) {
            mass.resize(newSize);

            positionX.resize(newSize); positionY.resize(newSize); positionZ.resize(newSize);
            velocityX.resize(newSize); velocityY.resize(newSize); velocityZ.resize(newSize);
            accelerationX.resize(newSize); accelerationY.resize(newSize); accelerationZ.resize(newSize);

            radius.resize(newSize);
            color.resize(newSize);
        }

        void reserve(std::size_t capacity) {
            mass.reserve(capacity);

            positionX.reserve(capacity); positionY.reserve(capacity); positionZ.reserve(capacity);
            velocityX.reserve(capacity); velocityY.reserve(capacity); velocityZ.reserve(capacity);
            accelerationX.reserve(capacity); accelerationY.reserve(capacity); accelerationZ.reserve(capacity);

            radius.reserve(capacity);
            color.reserve(capacity);
        }

        [[nodiscard]] std::size_t enlargeSize(std::size_t particleCount) {
            std::size_t oldSize = getSize();
            std::size_t newSize = oldSize + particleCount;
            resize(newSize);
            return oldSize;
        }

        void append(const ParticleSystem& other) {
            if (other.getSize() == 0) {
                return;
            }

            mass.insert(mass.end(), other.mass.begin(), other.mass.end());

            positionX.insert(positionX.end(), other.positionX.begin(), other.positionX.end());
            positionY.insert(positionY.end(), other.positionY.begin(), other.positionY.end());
            positionZ.insert(positionZ.end(), other.positionZ.begin(), other.positionZ.end());

            velocityX.insert(velocityX.end(), other.velocityX.begin(), other.velocityX.end());
            velocityY.insert(velocityY.end(), other.velocityY.begin(), other.velocityY.end());
            velocityZ.insert(velocityZ.end(), other.velocityZ.begin(), other.velocityZ.end());

            accelerationX.insert(accelerationX.end(), other.accelerationX.begin(), other.accelerationX.end());
            accelerationY.insert(accelerationY.end(), other.accelerationY.begin(), other.accelerationY.end());
            accelerationZ.insert(accelerationZ.end(), other.accelerationZ.begin(), other.accelerationZ.end());

            radius.insert(radius.end(), other.radius.begin(), other.radius.end());
            color.insert(color.end(), other.color.begin(), other.color.end());
        }
    };
}
