/**
 * @file counts.h
 * @date 19.11.2010
 * @author gerd
 */

#ifndef COUNTS_H_
#define COUNTS_H_

#include "../base/Graph.h"
#include "../motifs/TripleMotif.h"
#include "../motifs/QuadStarMotif.h"

namespace largenet {
namespace measures {

size_t triples(const Graph& net);
size_t inTriples(const Graph& net);
size_t outTriples(const Graph& net);
size_t inOutTriples(const Graph& net);
size_t triples(const Graph& net, const motifs::TripleMotif& t);

size_t quadStars(const Graph& net);
size_t outQuadStars(const Graph& net);
size_t inQuadStars(const Graph& net);
size_t quadStars(const Graph& net, const motifs::QuadStarMotif& q);

}
}

#endif /* COUNTS_H_ */