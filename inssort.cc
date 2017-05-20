#include "common.h"

/*
 * arr[1..N]
 * to sort elt i
 * find slot of i in sort(1..i-1)
 */

void sort(vector<int> &nums)
{
    for (int i = 1; i < nums.size(); i++) { //for i in 1..N
        for (int j = i; j > 0; j--) { //search the sorted sub-list
            if (nums.at(j) >= nums.at(j-1)) {
                break;
            }
            /*
              rationale for swap:
              insertion involves moving the sub-array > the current
              element right by one position.  same effect can be
              achieved by swapping.
            */
            swap(nums.at(j), nums.at(j-1));
        }
    }
}


// selection sort is close related to insertion sort.
// in selection sort, the iteration goes from 0-N creating a sorted sub list
// and at each step the objective is to find the suitable element
// for that position

// apparently, insertion is good enough for upto 10-15 elements.
int main(int argc, char **argv ) {
    vector<int> nums;
    for (int i = 1; i < argc; i++) {
        nums.push_back(atoi(argv[i]));
    }

    sort(nums);

    for (int i = 0; i < nums.size(); i++) {
        cout << nums.at(i) << " ";
    }
    cout << "\n";
}
