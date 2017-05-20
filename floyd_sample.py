import sys
import random

# M samples from integers in the range [1..N] w/o duplicates
def sample(M, N) :
    s = set()
    def sample_(m, n):
        if m == 0:
            return
        sample_(m-1, n-1)
        j = random.randint(1, n)
        if j in s:
            s.add(n)
        else :
            s.add(j)

    sample_(M, N)
    return s

# 7 2 9 1 5
# there is only one way this sequence could have been generated.
# run the steps backward. all random sequences are equally likely. (1/N!)
# therefore this algorithm generates a random permutation
# when n=10, rand returned 7, since 10 isn't present in the list, add 7 at 0 [2 9 1 5]
# when n=9,  rand returned 2, since 2 is present in the list, add 9 after 2 [2 1 5]
# when n=8,  rand returned 2, since 8 isn't present in the list, add 2 at 0 [1 5]
# when n=7,  rand returned 1, since 7 isn't present in the list, add 1 at 0 [5]
# when n=6,  rand returned 5, since 6 isn't present in the list, add 5 at 0 [] <-- list state before this iteration
# based off http://fermatslibrary.com/s/a-sample-of-brilliance
def permutation(M, N):
    l = []
    def perm_(m, n):
        if m == 0:
            return
        perm_(m-1, n-1)
        j = random.randint(1, n)
        try :
            index = l.index(j)
            j = n
        except:
            index = -1

        l.insert(index+1, j)

    perm_(M, N)
    return l

if __name__ == "__main__":
    sample(sys.argv[1], sys.argv[2])
