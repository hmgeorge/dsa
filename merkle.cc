#include <vector>
#include <iterator>
#include <iostream>
#include <string>
#include <cmath> // int(ceil(log2(doc.size()*1.0))) also gives height

/*
  Below function creates a complete tree as defined in
  http://web.cecs.pdx.edu/~sheard/course/Cs163/Doc/FullvsComplete.html

  A complete binary tree is a tree which is full at all levels except
  for maybe the last level and all nodes are as far left as possible.

  The above property makes complete binary tree implementation
  easy using an array (sort of similar to how a heap is implemented).

  In the below example the leaves H, I, J, K, L for the base
  Additional nodes are created to maintain the complete tree property,
  including one empty node (G' as it has no children).
        +-----A------+
        |            |
        B            C
       / \          / \
      /   \        /   \
     D     E      F    G'
    / \   / \    /
   H   I J   K  L

                  +---- leftmost
                  |
                  v
   0 1 2 3 4 5 6  7 8 9 10 11
   A B C D E F G' H I J K  L
 */
int create_complete_tree() {
  std::vector<std::string> doc{"H", "I", "J", "K", "L"};
  //std::vector<std::string> doc{"H", "I", "J", "K"};
  int tree_height = 0;

  int i=0;
  while (i <= doc.size()) i=1<<(++tree_height);
  size_t left_most=(1<<tree_height)-1;
  int tree_size = left_most + doc.size();
  std::vector<std::string> hashes;
  for (size_t i=0; i<tree_size; i++) {
    hashes.emplace_back("-");
  }

  std::cout << tree_height<< " "
            << tree_size << " "
            << hashes.size() << " "
            << left_most << "\n";
  // copy doc at index 2<<height-1
  for (const auto &s : doc) {
    size_t idx = left_most;
    hashes[idx] = s;
    // std::cout << idx << " " << s << " ";
    while (idx > 0) {
      ssize_t new_idx = (idx-1)/2;
      if (hashes[new_idx] != "-") break;
      hashes[new_idx] = hashes[idx];
      hashes[new_idx][0]= int(hashes[idx][0]-1-'A')/2+'A';
      // std::cout << new_idx << " " << hashes[new_idx] << " ";
      idx=new_idx;
    }
    ++left_most;
    // std::cout << "\n";
  }
  // std::cout << left_most << "\n";

  std::copy(hashes.begin(),
            hashes.end(),
            std::ostream_iterator<std::string>(std::cout, " "));
  std::cout << "\n";
  return 0;
}

int main() {
  create_complete_tree();
  return 0;
}

/*
  Another complete tree

             A
            /  \
           /    \
          B      C <-- full level
         / \    /
        D   E  F <-- doc is D, E, F
 */
