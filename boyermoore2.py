import collections
import sys

"""
      E X A M P L E           <-- P
            M P E X A M P L E <-- T
------------------------------------
      ---sf 5---E'X'A M P L E

#      "example"
suffix="abcpmex"
"""
def main():
    p=sys.argv[1]
    t=sys.argv[2]
    idx=collections.defaultdict(list)
    for i, c in enumerate(p) :
        idx[c].append(i)

    # this loop is same as the pi calculation
    # of KMP. KMP calculates pi for all chars.
    # this loop calculates for the last char.
    # better to check CLR for the optimized version
    # I think the value of pi[i] can be used in
    # calculating value of pi[i+1] .. pi[i] says
    # how much of the prefix matches the suffix until
    # that point of the pattern. we can skip matching
    # that many chars when calculating pi[i+1]
    pi = 0
    i=len(idx[p[0]])-1
    while i>=0 :
        j=idx[p[0]][i]
        k=0
        while j>k and j<len(p) and p[j]==p[k]:
            j+=1
            k+=1
        if k>pi:
            pi=k
        i-=1
    # print pi

    # Find longest suffix of Tq which is a prefix of P
    def lsuffix(Tq):
        max_i = -1
        max_j = 0
        begin = idx[Tq[-1]]
        j=0
        for i in begin:
            j=0
            while (i-j)>=0 and j<len(Tq):
                # print "P ", p[i-j], "S ", Tq[len(Tq)-j-1]
                if p[i-j] == Tq[len(Tq)-j-1] :
                    j+=1
                else:
                    break
            if j>max_j:
                max_i=i
                max_j=j
            # if max_i != -1 :
                # print p[max_i-max_j:max_i]
        return max_j

    i=0
    while len(t) - i >= len(p) :
        Tq = t[i:i+len(p)]
        s_len = lsuffix(Tq)
        if s_len == len(p):
            print "match found at ", i
        if s_len == 0 or s_len == len(p):
            # no matching suffix text or full text found, do full shift
            # while also considering longest suffix of pattern that is
            # a prefix of itself. this will allow with searching for
            # multiple occurrence of pattern like aba within ababa
            # number of matches == 2?
            shift = len(p) - pi
        else:
            # some suffix chars match, start next search
            # by taking into account the matched chars
            shift = len(p) - s_len
        i += shift

if __name__ == "__main__":
    main()
