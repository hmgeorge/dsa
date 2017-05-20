#include <set>
#include <iterator>
#include <iostream>
#include <algorithm>

// -std=c++1y
// limitation: if the underlying tree decides to rebalance
// we won't know and hence this algorithm might break as max
// isn't updated. The implementation in CLRS uses max to decide
// which path to take (left or right). But our search doesn't
// use it. (getting around rebalancing issues is a problem on it's own
// as we will then have to immplement the tree ourselves.)

struct interval {
  interval(int l, int h, int m=-1) : low(l), high(h), max(m) { }
  int low;
  int high;
  // max on the left sub-tree. this is different
  // from clrs where the max saved is max at the
  // tree sub-rooted at this interval. We use
  // max over left sub-tree to make the implementation
  // using std::unordered_set easier
  mutable int max; // read http://stackoverflow.com/questions/35315372/error-assignment-of-member-in-read-only-object
                   // for why mutable

  friend std::ostream& operator<<(std::ostream& os, const interval &a);

};

// looks like this cannot be defined inside as doing do
// throws away const qualifier for 'this'?
bool operator==(const interval &thiz, const interval &other) {
  return (thiz.low == other.low) && (thiz.high == other.high);
}

std::ostream& operator<<(std::ostream& out, const interval& a)
{
  return out << "(" << a.low << ", " << a.high << "  [" << a.max << "]" << ')';
}

// object to return when we can consider interval a is less than b
// as defined in CLRS page 312, key of an interval is the low endpoint.
// thus walk of the data structure lists the intervals in the sorted
// order by low end point
struct interval_compare {
  bool operator() (const interval &a, const interval &b) {
    return (a.low < b.low);
  }
};

using interval_tree = std::multiset<interval, interval_compare>;

void insert(interval_tree &tree, int low, int high)
{
  interval_tree::iterator it = tree.emplace(low, high, high);

  if (it == tree.begin() || std::next(it) == tree.end())
    return;

  auto prev = std::prev(it);
  auto next = std::next(it);
  (*it).max = (*prev).high;
  (*next).max = high;
}

interval_tree::iterator search_overlap(interval_tree &tree, int low, int high)
{
  interval i {low, high, high};
  // use in built operator to search for overlap (i think this internally
  // uses the less than operator
  return tree.find({low, high, high});
}

interval_tree::iterator search_eq(interval_tree &tree, int low, int high)
{
  interval i {low, high, high};
  return std::find(tree.begin(), tree.end(), i);
}

// this searches for an overlapping interval
bool search(interval_tree &tree, int low, int high)
{
  interval_tree::iterator it = search_overlap(tree, low, high);
  if (it != tree.end()) {
    std::cout << "found interval "
              << "[" << low << ", " << high << "] "
              << "at offset " << std::distance(tree.begin(), it) << "\n";
    return true;
  } else {
    std::cout << "search: failed to find interval " << "[" << low << ", " << high << "]\n";
    return false;
  }
}

int del(interval_tree &tree, int low, int high)
{
  interval_tree::iterator it = search_eq(tree, low, high);
  if (it == tree.end()) {
    std::cout << "del: failed to find interval " << "[" << low << ", " << high << "]\n";
    return -1;
  }

  auto next = std::next(it);

  if (next != tree.end()) {
    if (it != tree.begin()) {
      auto prev = std::prev(it);
      (*next).max = (*prev).high;
    }
  }
  tree.erase(it);
}

int main() {
  interval_tree tree;
  insert(tree, 8, 9);
  insert(tree, 0, 3);
  insert(tree, 5, 8);
  insert(tree, 6, 10);
  // copy from begin -> end to the range defined by the
  // third iterator... it happens to be and output stream iterator.
  std::copy(tree.begin(), tree.end(), std::ostream_iterator<interval>(std::cout, " "));
  std::cout << std::endl;
  search(tree, 8, 9);
  search(tree, 0, 4);
  search(tree, 5, 6);
  search(tree, 11, 14);
  del(tree, 0, 3);
  del(tree, 8, 9);
  del(tree, 5, 6);
  search(tree, 8, 9);
  search(tree, 0, 4);
}
