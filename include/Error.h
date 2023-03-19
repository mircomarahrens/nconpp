#pragma once

#include <string>

namespace ERROR {
    const static std::string CONSTRAINT_LEGPAIRS = "Only pairs of legs are allowed.";
    const static std::string CONSTRAINT_INVALIDLEG = "0 is not a valid leg index by convention.";
    const static std::string CONSTRAINT_UNIQUELEGS = "Only unique leg indices are allowed by convention.";
    const static std::string DISCONNECTED_NETWORKS = "The network is not continuously connected.";
};
