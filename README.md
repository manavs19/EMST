EMST
====

This solves an instance of the classic Euclidean Minimum Spanning Tree Problem using Rajasekaran's Algorithm.

Our task is to minimize the length of gas pipes in a city while maintaining connectivity to every house.
This means that we now need to solve an instance of the Euclidean minimum spanning tree problem.

The brute-force approach is to compute the distances between all pairs of houses,
and run any MST algorithm on this fully connected graph.
However, there are Θ(n2) edges in the graph (where n is the number of houses).
This leads to Ω(n2) running time. If we look at the problem geometrically,
we can arrive at O(n log n)-time algorithms. The basic observation is that
if two houses are distant from one another, it is likely that the edge connecting
these two houses will not be present in the MST. In other words, it suffices to
look at only the neighborhood of each house, thereby restricting the connectivity graph
to have only O(n) edges. 
So I implemented Rajasekaran's randomized algorithm which has an expected running time of O(n log n),
assuming that the houses are uniformly randomly distributed in a square.
This algorithm generates subgraphs with gradually growing neighborhood size.
Such a subgraph need not be connected. However, a minimum spanning forest of the subgraph
can be computed using any MST algorithm. When there is only one tree in the forest,
we have obtained the desired Euclidean MST.
