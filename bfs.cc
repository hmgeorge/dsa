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

   from CLRs - adj matrix might be better for dense graphs
   for unweighted graphs, adj matrix only needs 1 bit per edge.
*/

void bfs(vector<list<int> > &graph)
{
    bool done = false;
    list<int> to_visit;
    int32_t start_i = 0;
    vector<bool> visited;
    int32_t count = graph.size();
    for (size_t i = 0; i < count; i++) {
        visited.push_back(false);
    }
    int32_t i = start_i;
    while (!done) {
        to_visit.push_back(i);
        while (!to_visit.empty()) {
            int32_t v = *to_visit.begin();
            to_visit.erase(to_visit.begin());
            if (visited.at(v))
                continue;

            visited.at(v) = true;
            cout << v << " ";
            list<int> &edges = graph.at(v);
            list<int>::iterator iter = edges.begin();
            while (iter != edges.end()) {
                to_visit.push_back(*iter);
                iter++;
            }
        }

        i = (i + 1) % count;
        //looped around. we are done
        if (start_i == i) {
            done = true;
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
    bfs(graph);
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
    bfs(graph);
}

int main(int argc, char **argv)
{
    test2();
}
