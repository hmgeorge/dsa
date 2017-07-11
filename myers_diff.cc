#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <functional>
#include <cmath>
#include <climits>
#include <cassert>

using namespace std;

/*
   diagram as per https://blog.jcoglan.com/2017/02/12/the-myers-diff-algorithm-part-1/

   A  B  C  D  E
  o--o--o--o--o--o
D |  |  |  |\ |  | <-- diag, consider each edge as edit operation.so no edit at 0,0
  |  |  |  | \|  |     therefore diag at (3,0) to (4,1) can be interpreted
  o--o--o--o--o--o     as delete D from src (i.e go downward) and insert D from
A |\ |  |  |  |  |     dst (i.e go forward)
  | \|  |  |  |  |
  o--o--o--o--o--o
C |  |  |\ |  |  |
  |  |  | \|  |  |
  o--o--o--o--o--o
F |  |  |  |  |  |
  |  |  |  |  |  |
  o--o--o--o--o--o

  node (x,y) is connected to
  1) (x+1,y) - insert
  2) (x, y+1) - delete
  3) (x+1, y+1) if diag exists i.e if src[x] == dst[y]

  end loop when y co-ord of cur node == len(dst) - 1.
  this means we have edited the src to become dst.
  print
  if parent(x,y) is x-1,y -> print - src[x-1]
  if parent(x,y) is x,y-1 -> print + dst[y-1]
  else                    -> print C src[x-1] (or dont print anything)

       e.g.
          A  B  C  D
        o--o--o--o--o
      A |# |  |  |  |
        | #|  |  |  |
        o--o--o--o--o
      B |  |# |  |  |
        |  | #|  |  |
        o--o--o--o--o
      C |  |  |# |  |
        |  |  | #|  |
        o--o--o--o##o

        edit script is just "- D"
        We must take shortest path from 0,0 to (len(src)-1, len(dst)-1) to fully
        edit src->dst
cost
----
diagonal  - 1
deletions (move along x) - 2
insertions (move along y) - 3
*/

vector<string> readlines(char *filename)
{
    string line;
    ifstream infile(filename);
    vector<string> lines;
    while (getline(infile, line)) {
        lines.push_back(line+"\n");
    }
    return lines;
}

using Node = pair<int, int>;

float operator-(const Node& thiz, const Node& other) {
  return sqrt(pow(thiz.first-other.first,2) +
	      pow(thiz.second-other.second,2));
}

struct NodeHash {
    size_t operator()(Node const& n) const
    {
        return n.first;
    }
};

struct NodeEq {
    bool operator()(Node const &l, Node const &r) const {
        return l.first == r.first && l.second == r.second;
    }
};

//#define PRIO_Q
int main(int argc, char **argv) {
    vector<string> src = readlines(argv[1]);
    vector<string> dst = readlines(argv[2]);

    // use an unordered map instead of a list as
    // we are aiming to avoid having to run DJ on
    // the entire graph.
    vector<unordered_map<int, int>> graph;
    unordered_map<Node, Node, NodeHash, NodeEq> parent;

    // we insert elements into the prio q based on
    // distance to the dest, closest to dist gets
    // prio if weights match
    const Node end = {src.size(), dst.size()};
#ifdef PRIO_Q
    auto cmp = [&](const Node &left, const Node &right) -> bool {
        auto w_l = graph[left.second][left.first];
	auto w_r = graph[right.second][right.first];
	if (w_l == w_r) {
	  return end-left > end-right;
	} else
	  return w_l > w_r;
    };
    // visit(cmp) and decltype stunt only to satisfy C++!
    // as apparently you can't send a func as comparer without
    // this stunt. only reason i need a func is because compare
    // takes max of distance to the end ..
    priority_queue<Node,
		   vector<Node> /*container*/,
		   decltype(cmp) /*compare, class or func*/> visit(cmp);
#else
    deque<Node> visit;
#endif

#ifdef DEBUG
    for (auto &s: src) {
        cout << s;
    }
    cerr << src.size() << " " << dst.size() << "\n";
#endif

    unordered_map<int, int> adj_map;
#ifdef PRIO_Q
    visit.emplace(0,0);
#else
    visit.emplace_back(0,0);
#endif
    graph.push_back(adj_map);
    graph.at(0)[0] = 0;
    while (!visit.empty()) {
#ifdef PRIO_Q
        Node n = visit.top();
        visit.pop();
#else
        Node n = *(visit.begin());
        visit.erase(visit.begin());
#endif
        if (n.first == src.size() &&
            n.second == dst.size()) {
#ifdef DEBUG
	    cerr << "reached end\n";
#endif
	    break; // this only works if a fifo is used
	           // but breaking reduces time spent considerably
	    /*
	      g++ -DPRIO_Q -std=c++14 myers_diff.cc -o mdiff
	      time ./mdiff /tmp/1.cc /tmp/2.cc
	        real	0m2.691s

	      v/s
	        real	0m9.934s
	     */
	}
	auto x = n.first;
	auto y = n.second;
	// relax diag if lines match
	if (x+1<=src.size() && y+1<=dst.size()) {
	    if (src[x] == dst[y]) {
                // add_row_if_needed_for(y+1);
	        if (graph.size() == (y+1)) {
		    graph.push_back(adj_map);
		}
	        auto &row = graph.at(y);
	        auto &row2 = graph.at(y+1);
		if (row2.find(x+1) == row2.end()) {
		    row2[x+1] = INT_MAX;
		}
		// assert this should be min
		if (row2[x+1] > row[x] + 1) {
		  row2[x+1] = row[x] + 1;
		  parent[make_pair(x+1, y+1)]=n;
#ifdef PRIO_Q
		  visit.emplace(x+1, y+1);
#else
		  visit.emplace_back(x+1, y+1);
#endif
		}
	    }
	}
	// relax bottom neighbor
	if (y+1 <= dst.size()) {
	    if (graph.size() == (y+1)) {
	        graph.push_back(adj_map);
	    }
	    auto &row1 = graph.at(y);
	    // assert(row1 != graph.end());
	    auto &row2 = graph.at(y+1);
	    if (row2.find(x) == row2.end()) {
	        row2[x] = INT_MAX;
	    }
	    if (row2[x] > row1[x] + 3/*insertion*/) {
	        row2[x] = row1[x] + 3;
		parent[make_pair(x,y+1)] = n;
#ifdef PRIO_Q
		visit.emplace(x, y+1);
#else
                visit.emplace_back(x, y+1);
#endif
	    }
	}
	// relax right neighbor
	if (x+1 <= src.size()) {
	   auto &row = graph.at(y);
	   if (row.find(x+1) == row.end()) {
	        row[x+1] = INT_MAX;
	   }
	   // assert(row != graph.end());
	   if (row[x+1] > row[x] + 2/*deletion*/) {
                row[x+1] = row[x] + 2;
		parent[make_pair(x+1, y)] = n;
#ifdef PRIO_Q
		visit.emplace(x+1, y);
#else
                visit.emplace_back(x+1, y);
#endif
	   }
	}
    }
    list<string> edits;
    Node n = end;
    while (n != make_pair(0,0)) {
        Node p = parent[n];
        if (p.first == n.first-1 && p.second == n.second) {
            edits.push_front("- " + src[p.first]);
        } else if (p.first == n.first && p.second == n.second - 1) {
            edits.push_front("+ " + dst[p.second]);
        } else if (p.first == n.first - 1 && p.second == n.second - 1) {
            // XXX print $CONTEXT lines before first non-diag
            // and after last non-diag
	    // edits.push_front(src[p.first]);
        } else {
            cerr << "unknown case during printing edit script\n";
	    cerr << "P " << p.first << ", " << p.second << "\n";
	    cerr << "N " << n.first << ", " << n.second << "\n";
            assert(0);
        }
        n = p;
    }
    for (auto &e: edits) { cerr << e; }
}
