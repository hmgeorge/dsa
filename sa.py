import sys

def heapsort(l):
    heap = []
    count = 0
    for s in l:
        heap.append(s)
        i = count
        count += 1

        #now heapify upwards
        done = False
        while not done :
            if i % 2 == 0:
                parent_i = i / 2
            else:
                parent_i = (i-1)/2

            if i == parent_i:
                done = True
                continue

            #compare i and parent_i and swap
            if (heap[i] < heap[parent_i]):
                heap[parent_i], heap[i] = heap[i], heap[parent_i]
                i = parent_i
                #continue with more heapify
            else:
                done = True

    #remove element at 0 by swapping with the last element
    #and heapify downwards

    sorted_l = []
    #optimization: we could avoid using this extra space and
    #put the min element at the end but do not delete it
    #        1
    #      2 1
    #    3 2 1
    #  4 3 2 1
    # heapify happens in the existing heap space.
    # return as is for sort in descending order
    # return reversed for sort in ascending order
    while len(heap) > 0 :
        sorted_l.append(heap[0])

        #swap last element with 0 and delete last element
        heap[0] = heap[-1]
        del heap[-1]

        i = 0
        done = False
        #the last element must be bigger than atleast one of them
        #else there is an assertion failure
        while not done:
            l_idx = 2*i + 1

            if l_idx >= len(heap):
                #there cannot be a right elt
                done = True
                continue

            r_idx = 2*i + 2

            if r_idx >= len(heap):
                child_i = l_idx
            else:
                if heap[l_idx] < heap[r_idx] :
                    child_i = l_idx
                else:
                    child_i = r_idx

            #heapify down the appropriate heap
            if heap[i] > heap[child_i]:
                heap[i], heap[child_i] = heap[child_i], heap[i]
                i = child_i
            else:
                done = True

    return sorted_l

#naive method to generate suffix array
def mksa(s):
    def lcp(s_array) :
        #note: with a suffix tree, the lcp between any pair of strings can be
        #calculated easily
        l = []
        l.append(0) #for the first suffix
        for i in range(1, len(s_array)) :
            s1 = s_array[i]
            s2 = s_array[i-1]
            j = 0
            while j < len(s1) and j < len(s2) :
                if s1[j] != s2[j] :
                    l.append(j)
                    break
                j += 1
        return l

    l = []
    i = 0
    t = s + '$'
    [l.append(t[i:]) for i in range(len(t))]
    s_array = heapsort(l)
    return s_array, lcp(s_array)

def search(sa, pattern):
    if len(sa) == 0:
        print "not found"
        return

    left = 0
    right = len(sa) - 1
    i = (left + right) / 2
    if sa[i].startswith(pattern):
        print "found in ", sa[i]
        return

    if pattern < sa[i]:
        return search(sa[:i], pattern)
    else:
        return search(sa[i+1:], pattern)

#binary search!
def search(sa, lcp, pattern, all=True):
    def rsearch(sa, left, right, pattern):
        if left > right :
            #print "not found"
            return -1

        i = (left + right) / 2
        if sa[i].startswith(pattern):
            #print "found in ", sa[i]
            return i

        if pattern < sa[i]:
            return rsearch(sa, left, i-1, pattern)
        else:
            return rsearch(sa, i+1, right, pattern)

    i = rsearch(sa, 0, len(sa) - 1, pattern)
    matches = []
    if i < 0 :
        return ''
    #'$'              0
    #'e$'             0
    #'ense$'          1
    #'nonsense$'      0   
    #'nse$'           1
    #'nsense$'        3
    #'onsense$'       0
    #'se$'            0
    #'sense$'         2
    if all :
        #start from i and check lcp for i + 1
        #if it is not zero, that means there is some shared prefix
        #check if i+1 starts with pattern
        matches.append(sa[i])
        i += 1
        while i < len(sa):
            if lcp[i] == 0 :
                break

            if lcp[i] < len(pattern) :
                continue

            matches.append(sa[i])
            i += 1
    return matches

#heap ADT is usually implemented using an array
#for node n left child is 2n+1 and right child is 2n+2
#0,1,2,3,4,5 .....

if __name__ == "__main__":
    sa, lcp = mksa(sys.argv[1])
    print sa, lcp
    #TBD print all occurrences of this substring
    #i think thats where LCP comes into play
    #[sys.stdout.write(m+'\n') for m in search(sa, lcp, sys.argv[2])]
    
