#pragma once

#include <vector>
#include <cstddef>
#include <cassert>

#include <raylib.h>

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

    [[nodiscard]] std::size_t prepareAppend(std::size_t particleCount) {
        std::size_t oldSize = getSize();
        std::size_t newSize = oldSize + particleCount;
        resize(newSize);
        return oldSize;
    }
};