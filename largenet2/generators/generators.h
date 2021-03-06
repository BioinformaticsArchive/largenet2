/**
 * @file generators.h
 * @date 12.10.2010
 * @author gerd
 */

#ifndef GENERATORS_H_
#define GENERATORS_H_

#include <largenet2.h>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <utility>
#include <functional>
#include <stdexcept>

#ifndef NDEBUG
#include <iostream>
#endif

namespace largenet
{
/**
 * graph generators
 */
namespace generators
{

namespace util
{
/**
 * Get random element from container
 * @param container Random access container
 * @param rnd Random number generator
 * @return random element in @p container
 */
template<class U, class V, class RandomGen>
U& random_from(V& container, RandomGen& rnd)
{
	typedef typename V::size_type sz_t;
	sz_t from = 0, to = container.size() - 1;
	sz_t i = rnd.IntFromTo(from, to);
	return container[i];
}

/**
 * Get random element from range
 * @param begin iterator pointing to beginning of range
 * @param end iterator pointing to (past the) end of range
 * @param rnd random number generator
 * @return random iterator within range or @p end if empty range
 */
template<class _Iter, class RandomGen>
_Iter random_from(_Iter begin, _Iter end, RandomGen& rnd)
{
	if (begin == end)
		return end;
	typedef typename std::iterator_traits<_Iter>::difference_type d_t;
	d_t from = 0, to = std::distance(begin, end) - 1;
	d_t i = rnd.IntFromTo(from, to);
	std::advance(begin, i);
	assert(begin != end);
	return begin;
}

/**
 * Get random element from range
 * @param range iterator range
 * @param rnd random number generator
 * @return random iterator within @p range or @p range.second if empty range
 */
template<class _Iter, class RandomGen>
_Iter random_from(std::pair<_Iter, _Iter> range, RandomGen& rnd)
{
	return random_from(range.first, range.second, rnd);
}
}

/**
 * Random graph G(N,M), naive implementation
 * @param g
 * @param numNodes
 * @param numEdges
 * @param rnd
 * @param directed
 */
template<class RandomGen>
void randomGnmSlow(Graph& g, node_size_t numNodes, edge_size_t numEdges,
		RandomGen& rnd, bool directed = false)
{
	node_size_t max_edges =
			directed ?
					numNodes * (numNodes - 1) : numNodes * (numNodes - 1) / 2;
	if (numEdges > max_edges)
		throw std::out_of_range(
				"Cannot create graph with more than O(N^2) edges");
	g.clear();
	while (g.numberOfNodes() < numNodes)
		g.addNode();
	while (g.numberOfEdges() < numEdges)
	{
		Graph::NodeIterator n1 = util::random_from(g.nodes(), rnd), n2 =
				util::random_from(g.nodes(), rnd);
		if (n1.id() == n2.id())
			continue;
		if (g.isEdge(n1.id(), n2.id()))
			continue;
		else
			g.addEdge(n1.id(), n2.id(), directed);
	}
}

/**
 * Random graph G(N, M), fast implementation
 * @param g
 * @param numNodes
 * @param numEdges
 * @param rnd
 * @param directed
 * @see Phys. Rev. E 71, 036113 (2005)
 */
template<class RandomGen>
void randomGnm(Graph& g, node_size_t numNodes, edge_size_t numEdges,
		RandomGen& rnd, bool directed = false)
{
	if (directed)
	{
		randomGnmSlow(g, numNodes, numEdges, rnd, directed);
		return;
	}
	g.clear();
	typedef std::pair<node_id_t, node_id_t> edge_t;
	// hash table for edges
	typedef boost::unordered_set<edge_t> edge_set;

	while (g.numberOfNodes() < numNodes)
		g.addNode();

	if (numNodes < 1)
		return;
	// FIXME this algorithm does not do what it is expected to do when creating directed edges??
	edge_id_t max_edges = numNodes * (numNodes - 1) / 2; // undirected
	assert(numEdges <= max_edges);

	/*
	 * efficient G(n,m) from Phys. Rev. E 71, 036113 (2005)
	 */
	edge_set edges;
	edge_t current_edge;
	for (edge_size_t i = 0; i < numEdges; ++i)
	{
		while (true)
		{
			edge_id_t from = 0, to = max_edges - 1;
			edge_id_t edge_index = rnd.IntFromTo(from, to);
			current_edge.first = 1
					+ static_cast<node_id_t>(std::floor(
							std::sqrt(0.25 + 2.0 * edge_index) - 0.5));
			current_edge.second = static_cast<node_id_t>(edge_index
					- current_edge.first * (current_edge.first - 1) / 2);
			if (current_edge.first == current_edge.second) // disallow self-loops
				continue;
			if (edges.find(current_edge) == edges.end())
			{
				edges.insert(current_edge);
				g.addEdge(current_edge.first, current_edge.second, directed);
				break;
			}
		}
	}
}

/**
 * Create a random Erdős-Rényi network with link probability @p p.
 *
 * Links are created with probability @p p in such a way that the expected (average)
 * number of links is @f$ \frac{p}{2} N(N-1) @f$.
 * @param[out] g Graph object to store network (will be cleared first)
 * @param[in] numNodes number @f$ N @f$ of nodes
 * @param[in] edgeProb edge creation probability @f$ p @f$.
 * @param[in] rnd random number generator
 * @todo implement
 */
template<class RandomGen>
void randomGnp(Graph& g, node_size_t numNodes, double edgeProb, RandomGen& rnd)
{
	// FIXME this seems broken
	throw std::runtime_error("Not yet implented");
	/*
	 * efficient G(n,p) from Phys. Rev. E 71, 036113 (2005)
	 */
	//	assert(edgeProb > 0.0);
	//	assert(edgeProb < 1.0);
	//
	//	g.clear();
	//	while (g.numberOfNodes() < numNodes)
	//		g.addNode();
	//
	//	long int w = -1;
	//	Graph::NodeIteratorRange iters = g.nodes();
	//	for (Graph::NodeIterator v = iters.first; v != iters.second; ++v)
	//	{
	//		double r = rnd.Uniform01();
	//		w = 1 + w + static_cast<long int> (std::floor(std::log(1.0 - r)
	//				/ std::log(1.0 - edgeProb)));
	//		while ((w >= v->id()) && (v != iters.second))
	//		{
	//			w -= v->id();
	//			++v;
	//		}
	//		if (v != iters.second)
	//			g.addEdge(v->id(), w, false); // undirected
	//	}
}

/**
 * Barabási-Albert preferential attachment
 * @param g Graph object to hold BA graph, must be empty
 * @param numNodes Total number of nodes of final graph
 * @param m Number of edges attached to each new node
 * @param rnd Random number generator providing uniform IntFromTo(int low, int high)
 */
template<class RandomGen>
void randomBA(Graph& g, node_size_t numNodes, edge_size_t m, RandomGen& rnd)
{
	/*
	 * efficient BA(n,m) from Phys. Rev. E 71, 036113 (2005)
	 */
	if (g.numberOfNodes() != 0)
		throw std::invalid_argument("Need empty graph in randomBA");

	typedef std::vector<node_id_t> node_id_v;
	node_id_v nodes(2 * numNodes * m, 0);

	for (size_t v = 0; v < numNodes; ++v)
	{
		for (size_t i = 0; i < m; ++i)
		{
			size_t from = 0;
			size_t ind = 2 * (v * m + i);
			nodes[ind] = v;
			size_t r = rnd.IntFromTo(from, ind);
			nodes[ind + 1] = nodes[r];
		}
	}

	while (g.numberOfNodes() < numNodes) // this relies on an freshly initialized graph!
		g.addNode();

	for (size_t i = 0; i < numNodes * m; ++i)
		g.addEdge(nodes[2 * i], nodes[2 * i + 1], false); // undirected
}

/**
 * Create directed graph with power law out-degree distribution with exponent @p exponent
 * and Poisson in-degree distribution.
 * @param g Graph object to hold graph, must be empty
 * @param numNodes Total number of nodes of final graph
 * @param exponent power law exponent in out-degree distribution @f$ p(k) = k^(-exponent) @f$
 * @param rnd Random number generator providing uniform IntFromTo(int low, int high)
 */
template<class RandomGen>
void randomOutDegreePowerlaw(Graph& g, node_size_t numNodes, double exponent,
		RandomGen& rnd)
{
	if (g.numberOfNodes() != 0)
		throw std::invalid_argument(
				"Need empty graph in randomOutDegreePowerlaw");

	double normalization = 0;
	for (int i = numNodes - 1; i >= 1; --i)
	{
		normalization += pow(i, -exponent);
	}
	normalization = 1.0 / normalization;

	std::vector<node_size_t> degdist(numNodes, 0);
	double remain = 0;
	for (size_t i = numNodes - 1; i >= 1; --i)
	{
		degdist[i] = 0;
		remain += numNodes * normalization * pow(i, -exponent);
		degdist[i] = static_cast<node_size_t>(floor(remain));
		remain -= degdist[i];
	}
	degdist[0] = static_cast<node_size_t>(round(remain));

#ifndef NDEBUG
	node_size_t degsum = 0;
	double mdeg = 0;
	for (size_t k = 0; k < degdist.size(); ++k)
	{
		degsum += degdist[k];
		mdeg += k * degdist[k];
	}
	std::cout << "Sum of N[k] is " << degsum << "\n";
	std::cout << "Mean degree is " << mdeg / degsum << "\n";
#endif

	while (g.numberOfNodes() < numNodes)
		g.addNode();

	std::vector<node_id_t> nodes(numNodes, 0);
	Graph::NodeIteratorRange nd = g.nodes();
	std::transform(nd.first, nd.second, nodes.begin(),
			std::mem_fun_ref(&Node::id));

	for (size_t i = numNodes - 1; i > 0; --i)
	{
		for (node_size_t n = degdist[i]; n > 0; --n)
		{
			node_id_t cur_id = nodes.back();
			nodes.pop_back();
			for (node_size_t k = 0; k < i; ++k)
			{
				Graph::NodeIterator nit = util::random_from(g.nodes(), rnd);
				while (g.isEdge(cur_id, nit.id()) || cur_id == nit.id()) // disallow double edges and self-loops
					nit = util::random_from(g.nodes(), rnd);
				g.addEdge(cur_id, nit.id(), true);
			}
		}
	}
}

/**
 * Undirected Watts-Strogatz small-world graph
 * @param g
 * @param numNodes
 * @param numNeighbors
 * @param rewProb
 * @param rnd
 */
template<class RandomGen>
void wattsStrogatzGraph(Graph& g, node_size_t numNodes,
		node_size_t numNeighbors, double rewProb, RandomGen& rnd)
{
	/// @see implementation in Python Networkx (http://networkx.lanl.gov)
	if (numNeighbors >= numNodes / 2)
		throw std::out_of_range("Number of neighbors must be less than N/2.");

	g.clear();
	while (g.numberOfNodes() < numNodes)
		g.addNode();

	typedef std::vector<node_id_t> node_id_v;
	node_id_v nodes(numNodes, 0);
	Graph::NodeIteratorRange nd = g.nodes();
	std::transform(nd.first, nd.second, nodes.begin(),
			std::mem_fun_ref(&Node::id));
	assert(nodes.size() == g.numberOfNodes());
	for (node_size_t i = 1; i < numNeighbors / 2 + 1; ++i)
	{
		node_id_v targets(numNodes, 0);
		// targets = nodes[i:] + nodes[0:i]
		std::rotate_copy(nodes.begin(), nodes.begin() + i, nodes.end(),
				targets.begin());
		for (size_t k = 0; k < numNodes; ++k)
			g.addEdge(nodes[k], targets[k], false);
	}

	assert(g.numberOfEdges() == numNodes * numNeighbors / 2);

	// TODO now, rewire each edge with probability p
	typedef std::vector<edge_id_t> edge_id_v;
	edge_id_v edges;
	edges.reserve(
			2 * static_cast<size_t>(std::ceil(rewProb * g.numberOfEdges())));
	BOOST_FOREACH(Edge& e, g.edges())
	{
		if (rnd.Chance(rewProb))
			edges.push_back(e.id());
	}
	BOOST_FOREACH(edge_id_t eid, edges)
	{
		Edge* e = g.edge(eid);
		Graph::NodeIterator nit = util::random_from(g.nodes(), rnd);
		// no self-loops or double edges
		while ((nit.id() == e->source()->id())
				|| g.adjacent(e->source()->id(), nit.id()))
			nit = util::random_from(g.nodes(), rnd);
		g.addEdge(e->source()->id(), nit.id(), false);
		g.removeEdge(eid);
	}
}

}
}

#endif /* GENERATORS_H_ */
