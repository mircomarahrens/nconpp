// Copyright 2023 Mirco Marahrens

#ifndef INCLUDE_NCONPP_RENORMALIZATION_HPP_
#define INCLUDE_NCONPP_RENORMALIZATION_HPP_

#include <complex>

#include "nconpp/LinAlg.hpp"

template <typename T = std::complex<double>> class Renormalization {
    Renormalization() {
        // 1. Init: Takes an initial TensorNetwork object
        // 2. Init: Takes SIMULATION PARAMETERS
        // 2. Checks: Checks for dangling legs and removes them?
        // 3. Init: Calculates a one-dimensional PATH through the underlying lattice of the TN
        // (Snake algorithm?)
        // 4. Init: Half the PATH in odd and even bonds
        // 5. performs a renormalization step on odd and even edges consecutive
    };

  private:
};

#endif // INCLUDE_NCONPP_RENORMALIZATION_HPP_
