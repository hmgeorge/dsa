#include "common.h"

// first is the node number, second is the weight
using node_t = pair<int, int>;
using adj_list_t = list<node_t>;

// first is the distance to each node and second is it's parent
using node_dist_t = pair<long int, int>;

vector<adj_list_t> mkgraph(int num) { // N nodes
  vector<adj_list_t> graph;
  for (size_t i = 0; i < num; i++) {
      adj_list_t edges;
      cout << i << " - ";
      for (size_t j = 0; j < num; j++) {
        if (j == i) {
          continue;
        }

        // flip a coin
        if (rand() % 2) {
          /* edge is present */
          int weight = rand() % 20;
          edges.emplace_back(j, weight);
          cout << "(" << j << "," << weight  << ") ";
        }
      }

      graph.emplace_back(edges);
      // edges.clear() isn't necessary as the edge elements
      // are moved into the graph
      cout << "\n";
  }
  return graph;
  // compare length of this graph creation with C++03
  // construction in bellman.cc
}

vector<node_dist_t> dj(vector<adj_list_t> &graph, int start_node) {
  vector<node_dist_t> nd;
  list<int> s;

  for (int i=0; i<graph.size(); i++) {
    nd.emplace_back(INT_MAX, -1);
  }

  s.push_back(start_node);
  while (!s.empty()) {
    int u = *(s.begin());
    s.erase(s.begin());
    adj_list_t &edges = graph.at(u);
    for (adj_list_t::iterator iter=edges.begin(); iter != edges.end(); iter++) {
      int v = (*iter).first;
      int w = (*iter).second;
      if (nd[v].first > nd[u].first + w) {
	nd[v].first = nd[u].first + w;
	nd[v].second = u;
	s.push_back(v);
      }
    }
  }
  return nd;
}

void walk(vector<adj_list_t> &graph)
{
  for (size_t i=0; i<graph.size(); i++) {
    adj_list_t::iterator iter = graph.at(i).begin();
    std::cout << i << " - ";
    while (iter != graph.at(i).end()) {
      std::cout << "(" << (*iter).first << "," << (*iter).second << ") ";
      iter++;
    }
    std::cout << "\n";
  }
}

//https://mbevin.wordpress.com/2012/11/20/move-semantics/ - [1]
//http://stackoverflow.com/questions/3601602/what-are-rvalues-lvalues-xvalues-glvalues-and-prvalues - [2]
//values.cc for more experiments with lvalues, rvalues and xvalues
int main(int argc, char **argv) {
  if (argc < 3) {
    cerr << "./a.out num seed\n";
    return -1;
  }

  int num = atoi(argv[1]);
  srand(atoi(argv[2]));

  // see [1] for why returning graph from mkgraph doesn't need a copy
  // graph is a xvalue (eXpiring value) . resources can be transferred
  // using std::move. (which is done in the move ctor of vector)
  vector<adj_list_t> graph = mkgraph(num);
  walk(graph);
  vector<node_dist_t> nd = dj(graph, rand() % graph.size()); /* pick a random node as start */
}

/*
  Thoughts on the synchronous shopping problem from hackerrank
  The problem consisted of find the minimum distance that would
  have to be travelled to get all K types of fish.

  The solution consisted of finding the minimum distance and
  finding it with as few market visits. Since we do not know
  in advance the global order, the solution runs dijsktra for
  each market and for each of the 2^K states. In the relax stage,
  we update based on :
  1) distance from node u -> node v
  2) how many fish types are node u bringing to node v


  relax(v1, v2, u+w) .. v1, v2 are the state vars, u+w is the value to
  check for in the relax condition

  if dist[v1][v2] > u+w :
     dist[v1][v2] = u+w
     add <v1,v2> to S to relax all of it's neighbors
  
     // ith nbor of u     // accum fish types | avail fish types    // dist to reach node        + dist to reach
     //                   //    at u          |  at ith nbor        // node u with vm fish types   nbor i from u
 relax(   adj[u][i]      ,      vm            | a[adj[u][i]]      , dist[u][vm]                  + adj[u][i].second);

 By doing relax for all node/mask pairs, this is essentially running # of dijkstra
 and then globally optimal soln is selected at end.. (nodes that maximize dist[][])

*/
