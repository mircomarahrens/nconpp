// Copyright 2023 Mirco Marahrens

#ifndef SRC_ERRORMESSAGES_HPP_
#define SRC_ERRORMESSAGES_HPP_

namespace ErrorMessages
{
const char *const ERROR_CONSTRAINT_LEGPAIRS = "Only pairs of legs are allowed.";
const char *const ERROR_CONSTRAINT_INVALIDLEG = "0 is not a valid leg id.";
const char *const ERROR_CONSTRAINT_UNIQUELEGS = "Only unique dangling leg indices are allowed by convention.";
const char *const ERROR_OUT_OF_SIZE = "The position to split is not within the range of legs.";
const char *const ERROR_VERTEXID_PRESENT = "Vertex identifier already present.";
const char *const ERROR_VERTEXID_NOTPRESENT = "Vertex identifier not present.";
const char *const ERROR_EDGE_PRESENT = "Edge already present.";
const char *const ERROR_EDGEID_PRESENT = "Edge identifier already present.";
const char *const ERROR_EDGEID_NOTPRESENT = "Edge identifier not present.";
const char *const ERROR_PARALLEL_EDGE_PRESENT = "Parallel edge already present.";
const char *const ERROR_SOURCEID_NOTPRESENT = "Source vertex identifier not present.";
const char *const ERROR_DESTID_NOTPRESENT = "Destination vertex identifier not present.";
} // namespace ErrorMessages

#endif // SRC_ERRORMESSAGES_HPP_
