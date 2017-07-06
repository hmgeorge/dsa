#include <string>
#include <iostream>
#include <cassert>

// based on The_String-to-String_Correction_Problem by Walter F. Tichy
// page 5
using namespace std;

int main(int argc, char **argv) {
  string s, t;
  if (argc < 3) {
    s = "vwvwxy";
    t = "zvwxw";
  } else {
    s = argv[1]; //"vwvwxy"; // "shanghai";
    t = argv[2]; //"zvwxw"; // "sakhalin";
  }
  string T=t;
  while (T.length() != 0) {
    // if a prefix can be found, find the longest one
    // starting from beginning of S. for e.g
    // if T = vwx and S = vwvwx, then prefix returned
    // should be vwx for the longest even though there
    // is a (shorter) prefix vw
    int i=0,j=0;
    int max_len=0;
    int max_j=0;
    while (i<T.length() && (j+i)<s.length()) {
      if (T[i] != s[j+i]) {
	j=j+i;
	i=0; // start T from the beginning
	while (j<s.length() && T[i] != s[j]) j++;
      } else {
	i++;
	// i is the length of the current match
	if (i > max_len) {
	  max_len = i;
	  max_j = j;
	}
      }
    }
    if (max_len == 0) {
      // if no prefix can be found, this char must be added
      cerr << "A " << T[0] << "\n";
      T=T.substr(1);
    } else {
      cerr << "M " << max_j << ", " << max_len << "\n";
      T=T.substr(max_len);
    }
    // cerr << "* " << T << "\n";
  }
  // TODO - implement optimizations in page 7
  // esp
  // In  applying  the  block  move  algorithm  to
  // prose  or  program  text,  it  is  therefore
  // appropriate  to  choose  lines  as  the
  // atomic  symbols.  To  speed  up  comparisons,
  // the  program  should  use  hash  codes  for
  // lines  of  text  rather  than  performing
  // character-by-character  comparison
}
