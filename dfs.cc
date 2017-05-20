#include <list>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <stdio.h>

using namespace std;

void usage()
{
    cerr << "./a.out vertices seed\n";
}


/*
   graph is represented as a vector of vertices with the adjacency list
   represented as a list of integers indexing into the vector. this can
   be a problem if say the vector were deleted .. maybe a list of vertices
   is better?
*/

void dfs(vector<list<int> > &graph)
{
  bool done;
  list<int> to_visit;
  vector<bool> visited;
  int32_t count = graph.size();
  for (size_t i = 0; i < count; i++) {
    visited.push_back(false);
  }

  // as mentioned in CLRS, the way they define
  // DFS, it can be used to generate forest of connected
  // components (undirected). extended the base version
  // of DFS to support it.
  for (size_t i = 0; i < count; i++) {
    if (visited.at(i))
      continue;

    to_visit.push_back(i);
    while (!to_visit.empty()) {
      int32_t v = *to_visit.begin();
      to_visit.erase(to_visit.begin());
      if (visited.at(v))
        continue;

      visited.at(v) = true;

      // a thought on adding color to each node.
      // suppose when a node is first visited, it is marked
      // as gray. looking at the recursive version, the
      // node can be marked black only after visiting
      // (depthwise) all it's adjacent nodes. To get that
      // effect, allocate another stack and push to it
      // the node you want to mark as black & push_front
      // a special node (like -10). When poppsed at the
      // beginning of the loop, if this special node is observed
      // pop from the second stack and mark that node as black.
      cout << v << " ";
      list<int> &edges = graph.at(v);
      list<int>::iterator iter = edges.begin();
      while (iter != edges.end()) {
        //XXX check this - bfs(push_back), dfs(push_front) ???
        to_visit.push_front(*iter);
        iter++;
      }
    }
  }
  cout << "\n";
}

void print(vector<list<int> > &graph)
{
    for (size_t i = 0; i < graph.size(); i++) {
        list<int> &adj_list = graph.at(i);
        list<int>::iterator iter = adj_list.begin();
        cout << "V " << i << " - ";
        while (iter != adj_list.end()) {
            cout << *iter << " ";
            iter++;
        }
        cout << "\n";
    }
}


void test1(int argc, char **argv)
{
    if (argc < 3) {
        usage();
        exit(-1);
    }

    //without this srand call, the same graph was being generated
    //in every iteration.
    srand(atoi(argv[2]));
    int vertices = atoi(argv[1]);
    vector<list<int> > graph;
    for (size_t i = 0; i < vertices; i++) {
        list<int> edges;
        graph.push_back(edges);
    }

    for (size_t i = 0; i < graph.size(); i++) {
        list<int> &adj_list = graph.at(i);
        for (int32_t j = 0; j < graph.size(); j++) {
            if (j == i) {
                continue;
            }
            int32_t flip = rand() % 2;
            if (flip) {
                adj_list.push_back(j);
            }
        }
    }

    print(graph);
    dfs(graph);
}

void test2()
{
    /*
      0 <- 1 <-
               \
                ---> 2 <- 3
    */
    vector<list<int> > graph;
    list<int> edges;
    //v 0 has 0 edges
    graph.push_back(edges);

    edges.push_back(0);
    edges.push_back(2);
    //v 1 has 2 edges
    graph.push_back(edges);
    edges.clear();

    edges.push_back(1);
    //v 2 has 1 edge
    graph.push_back(edges);
    edges.clear();

    edges.push_back(2);
    //v 3 has 1 edge.
    graph.push_back(edges);
    print(graph);
    dfs(graph);
}

int main(int argc, char **argv)
{
    test2();
}
