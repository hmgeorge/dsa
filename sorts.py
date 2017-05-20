import sys

#first try.
def qsort(array):
    def qsort_l(a, left, right) :
        if left >= right:
            return

        l = left + 1
        r = right

        #base case. always think of a base case!
        if l == r : #i.e 2 elements
            a[left], a[right] = min(a[left], a[right]), max(a[left], a[right])
            return

        pivot = a[left]
        while l <= r:
            if a[l] <= pivot:
                l += 1
                continue

            #need to find a suitable element from the right
            #to replace the left element with
            found = False
            if r == l:
                continue

            while r >= l : # l <= r
                if a[r] >= pivot:
                    r -= 1
                    continue

                found = True
                a[l], a[r] = a[r], a[l]
                break

            if not found:
                #no element was found
                #retract left one step
                #to go back to the last known element < pivot
                #pivot can be swapped with this element
                #and still maintain qsort invariant [1..] <= pivot <= [..N]
                break

        if l > r:
            l -= 1

        if left != l :
            a[left], a[l] = a[l], a[left]

        qsort_l(a, left, l-1)
        qsort_l(a, l+1, right)

    qsort_l(array, 0, len(array) - 1)
    return array

"""
flow:
set pivot to leftmost element
intent is to find position in this array where pivot can be set

iterate through the list (starting at l = left + 1) and jump over all elements <= pivot
once we reach an element > pivot, pause
    do a reverse iteration (starting at r = right) until we reach l
    if an element was found :
        swap a[l] and a[r] and continue the main iteration
    else:
         retract one step in l (since we are standing on a > pivot element)
         and break since no element can be found again

continue the mainloop until l exceeds r
swap left (pivot) with the element at position l
repeat recursively on the sub lists
"""
def qsort3(array):
    def qsort_l(a, left, right) :
        if left >= right:
            return

        l = left + 1
        r = right

        #base case. always think of a base case!
        if l == r : #i.e 2 elements
            a[left], a[right] = min(a[left], a[right]), max(a[left], a[right])
            return

        pivot = a[left]
        while True:
            if a[l] > pivot:
                found = False
                while r > l:
                    if a[r] >= pivot:
                        r -=1
                    else:
                        found = True
                        a[l], a[r] = a[r], a[l]
                        break

                if not found :
                    if l != r:
                        sys.stderr.write("assert l == r here... debug\n");
                        sys.exit(-1)
                    l -= 1
                    break
            else:
                if l + 1 > r :
                    break
                l += 1

        if l < left :
            #note: l == left only if
            #pivot p <= q for all q in [left+1, right]
            sys.stderr.write("assert left >= l here... debug (%d %d)\n" % (left, right));
            sys.exit(-1)

        a[left], a[l] = a[l], a[left]
        qsort_l(a, left, l-1)
        qsort_l(a, l+1, right)

    qsort_l(array, 0, len(array) - 1)
    return array

if __name__ == "__main__":
    sys.stderr.write('%s\n' % (' '.join(qsort3(sys.argv[1:]))))
