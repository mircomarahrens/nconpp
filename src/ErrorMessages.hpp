// Copyright 2023 Mirco Marahrens

#ifndef NCONPP_INCLUDE_ERRORMESSAGES_H_
#define NCONPP_INCLUDE_ERRORMESSAGES_H_

// Graph.h
#define ERROR_VERTEXID_PRESENT "Vertex identifier already present."
#define ERROR_VERTEXID_NOTPRESENT "Vertex identifier not present."
#define ERROR_EDGE_PRESENT "Edge already present."
#define ERROR_EDGEID_PRESENT "Edge identifier already present."
#define ERROR_EDGEID_NOTPRESENT "Edge identifier not present."
#define ERROR_PARALLEL_EDGE_PRESENT "Parallel edge already present."
#define ERROR_SOURCEID_NOTPRESENT "Source vertex identifier not present."
#define ERROR_DESTID_NOTPRESENT "Destination vertex identifier not present."

// TensorNetwork.h
#define ERROR_CONSTRAINT_LEGPAIRS "Only pairs of legs are allowed."
#define ERROR_CONSTRAINT_INVALIDLEG "0 is not a valid leg id."
#define ERROR_CONSTRAINT_UNIQUELEGS \
  "Only unique dangling leg indices are allowed by convention."
#define ERROR_OUT_OF_SIZE \
  "The position to split is not within the range of legs."

#endif  // NCONPP_INCLUDE_ERRORMESSAGES_H_
