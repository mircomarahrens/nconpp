#pragma once

#include <stdexcept>
#include <string>

namespace ERROR_MESSAGES {
    const static std::string CONSTRAINT_LEGPAIRS = "Only pairs of legs are allowed.";
    const static std::string CONSTRAINT_DISCONNECT = "Found disconnected components in network. You can connect them "
                                                     "trivially by running connectDisconnectedComponents().";
    const static std::string CONSTRAINT_INVALIDLEG = "0 is not a valid leg index by convention.";
    const static std::string CONSTRAINT_UNIQUELEGS = "Only unique leg indices are allowed by convention.";
};
