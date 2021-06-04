# Dynamic Graphs
## About
This project was worked on as part of the Algorithmic Graph Theory course (CMPS 396AC) in collaboration with Hadi Kotaich and Jihad Hanna.\
Dynamic connectivity structures aim to solve problems related to queries on graphs and trees that involves addition and removal of edges. In our project we focused on trees mainly and tackled a special case on graphs (offline queries).\
We first tried to understand how it is solved on trees, we then implemented two solutions namely the link cut tree which was invented by Sleator and Tarjan and the Euler tour tree which was invented by Henzinger and King.\
We tested correctness on online judges and then tested speed locally. Once we were done with trees, we tackled the problem on graphs and implemented a solution with the restriction that queries are offline.
## Motivation
We were motivated to work on this problem due to many reasons: 
* It is very useful in competitive programming to know about such data structures. 
* Although we might find some implementations online, understanding the algorithm is crucial to use it and be able to change it according to the given problem. 
* What we did will improve the prewritten code repertoire we have as AUB competitive programmers and it will hopefully be useful for future generations. 

This is why it was important for us to implement and test well what we wrote and not only understand the algorithm.

## Formal Problem Statement
We are given an acyclic undirected graph G = (V, E) and multiple updates/queries that we should maintain/answer:
* cut(u, v): Assuming there is at this point an edge between nodes u & v, remove that edge.
* link(u, v): Assuming there is at this point no path between nodes u & v, add an edge between u & v.
* isConnected(u, v): Check if nodes u & v are in the same connected component, i.e. check if there’s currently a path from u to v.

We relaxed the general problem to make sure that throughout all computation G is an undirected forest.
This problem can be easily solved in O(1) update time and O(|V| + |E|) query time using BFS/DFS but in our work we’re aiming for better, namely something in the order of O(log n) for both queries and updates
