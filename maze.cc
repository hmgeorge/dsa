#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <string>

using namespace std;

using node = std::pair<int, int>;

node start_node;
node end_node;
std::vector<std::vector<char> > graph;
std::vector<std::vector<node> > parents;

void
mkgraph(char * filename)
{
  std::string line;
  std::ifstream infile(filename);
  int row = 0;
  int column = 0;
  while (std::getline(infile, line)) {
    std::cout << line << std::endl;
    std::vector<char> l;
    std::vector<node> p;
    for (std::string::iterator iter = line.begin();
         iter != line.end(); iter++) {
      if (*iter == 'X') {
        start_node.first = row;
        start_node.second = column;
        l.emplace_back(' ');
      } else if (*iter == 'O') {
        end_node.first = row;
        end_node.second = column;
        l.emplace_back(' ');
      } else {
        l.emplace_back(*iter);
      }
      p.emplace_back(-1,-1);
      ++column;
    }
    graph.emplace_back(l);
    parents.emplace_back(p);
    ++row;
    column = 0;
  }

  // std::cout << "start node " << start_node.first << "," << start_node.second << std::endl;
  // std::cout << "end node " << end_node.first << "," << end_node.second << std::endl;
}

bool
should_visit(int row, int col) {
  if (row < 0 || row >= graph.size())
    return false;

  if (col < 0 || col >= graph[0].size())
    return false;

  if (graph[row][col] != ' ') {
    return false;
  }

  node &p = parents[row][col];
  if (p.first == -1 && p.second == -1) { // i.e not visited
    return true;
  }
  return false;
}

// solve_maze using bfs.
// exit when the end node is seen for the first time
// as thats the smallest distance from start to end
void solve_maze() {
  std::cout << std::endl;
  std::list<node> to_visit;
  to_visit.push_back(start_node);

  while (!to_visit.empty()) {
    node n = *(to_visit.begin());
    to_visit.erase(to_visit.begin());

    if (n.first == end_node.first && n.second == end_node.second) {
      break; //  we are done
    }

    // check top
    if (should_visit(n.first-1, n.second)) {
      to_visit.emplace_back(n.first-1, n.second);
      parents[n.first-1][n.second] = n;
    }

    // check bottom
    if (should_visit(n.first+1, n.second)) {
      to_visit.emplace_back(n.first+1, n.second);
      parents[n.first+1][n.second] = n;
    }

    // check left
    if (should_visit(n.first, n.second-1)) {
      to_visit.emplace_back(n.first, n.second-1);
      parents[n.first][n.second-1] = n;
    }

    // check right
    if (should_visit(n.first, n.second+1)) {
      to_visit.emplace_back(n.first, n.second+1);
      parents[n.first][n.second+1] = n;
    }
  }

  // add + for each parent starting from end node to start node
  // tracing the parents
  node n = end_node;
  while (!(n.first == start_node.first && n.second == start_node.second)) {
    graph[n.first][n.second] = '+';
    n = parents[n.first][n.second];
  }

  graph[start_node.first][start_node.second] = '+';

  for (size_t i=0; i<graph.size(); i++) {
    std::vector<char> &adj = graph[i];
    for (std::vector<char>::iterator it=adj.begin(); it != adj.end(); it++) {
      std::cout << *it;
    }
    std::cout << std::endl;
  }
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    return -1;
  }
  mkgraph(argv[1]);
  solve_maze();
}

//example mazes.. save to a file and run
#if 0
#############################
#                   #       X
#                   #       #
#                   #       #
#   #############   #####   #
#   #       #   #           #
#   #       #   #           #
#   #       #   #           #
#####   #   #   #########   #
#       #           #       #
#       #           #       #
#       #           #       #
#####   #   #########   #####
#       #   #           #   #
#       #   #           #   #
#       #   #           #   #
#   #########   #########   #
#                           #
#                           #
O                           #
#############################

##########
X  #     #
#  #  #  #
#     #  O
##########

#############################
#                   #       #
#        #          #       #
#        #          #       #
#   ########## ##   #####   #
#   #       #   #           #
X   #       #   #           #
#   #       #   #           #
#####   #   #   #########   #
#       #           #       #
#       #           #       #
#       #           #       #
#####   #   ### #####   #####
#       #   #           #   #
#       #   #               #
#       #   #           #   #
#   #########   #########   O
#               #           #
#               #           #
#                           #
#############################


#endif
