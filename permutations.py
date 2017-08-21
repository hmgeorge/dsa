# On permutations and subsets

#1) Basic permutation algorithm
# note that this assumes all distinct chars. Case with duplicating chars is given later
def perm(s):
	def _perm(s, o, rem):
		if rem == 0:
			print o
		for c in s:
			if c not in o:
				_perm(s, o+c, rem-1)
	_perm(s, '', len(s))

#2)Basic subset algorithm
#each element passes two sets downstream
#1) pass whatever was sent to it as is
#2) add self to whatever was sent to it
# this goes well with the binary way of thinking.
# it also ensures that every subset is added just once to the powerset
def subset(s):
	powerset=set()
	def subs_each(i, a_set):
		if i==len(s):
			if a_set in powerset:
				print a_set, "already in power set!" # observe: this should never be printed!
			else:
				powerset.add(a_set)
			return
		#case 1
		subs_each(i+1, a_set)
		#case 2
		#2.1: if there are duplicate elements in the string, then we need to pass down only the last
		#     occurence of that element. rest can be dropped as they do not contribute to the
		#     power set
		if i == len(s)-1 or s[i+1] != s[i] :		
			subs_each(i+1, a_set+s[i])
	subs_each(0, '')
	return powerset

#>>> subset('1123')
#set(['', '13', '12', '23', '1', '3', '2', '123'])
#>>> subset('112333')
#set(['', '13', '12', '23', '1', '3', '2', '123'])
#>>> subset('123')
#set(['', '13', '12', '23', '1', '3', '2', '123'])
#>>> subset('11123')
#set(['', '13', '12', '23', '1', '3', '2', '123'])
#>>> 

#3) Print all lexicographic permutations of a string
# http://www.techiedelight.com/find-lexicographic-permutations-string/
# i.e print permutations with repetitions allowed.
# e.g.
# >>> lex_perm('AB')
# set(['AA', 'AB', 'BA', 'BB'])
#
# this should work even with duplicates.
# e.g.
# >>> perm('AAB')
# would return 'AAA' eight times.
def lex_perm(s):
	lex_set=set()
	def _lex_perm(s, o, rem):
		if rem == 0:
			if o in lex_set:
				print o, "already in lex_set" # observe: should not get printed
			else :
				lex_set.add(o)
			return
		for i in range(len(s)):
			# notice similarity between this and the set problem.
			# only the last occurence of a char needs to be sent downstream to
			# avoid printing the same char (at different indices multiple times for the same depth).
			# Therefore sorting the chars helps.
			# Cannot use higher index technique used for perm as there we use higher index
			# to limit duplication across depths.. not within the same depth.
			if i == len(s)-1 or s[i] != s[i+1]:
				_lex_perm(s, o+s[i], rem-1)
	_lex_perm(s, '', len(s))
	print lex_set

#4) Permutations with duplicates
#idea: same char is allowed but with higher index only
#
#so for aaaabc
#
#1 2 3 4 <-- so only this order is accepted in the final output.. all other orders will get dropped off at some point
#
#note: s doesn't have to be sorted by char, like in the problem with sets or lex_perm
def perm(s):
	o=[]
	perm_set=set()
	def _perm(rem):
		if rem == 0:
			p = ''.join(s[i] for i in o)
			if p in perm_set :
				print p, "already in perm_set" # observe: should never happen
			else :
				perm_set.add(p)
			return
		for i in range(len(s)):
			ordered=True
			for j in o:
				if s[i] != s[j]:
					continue
				#else s[i] == s[j]
				if i <= j : # not higher order
					ordered=False
					break
			if ordered:
				o.append(i)
				_perm(rem-1)
				o.pop()
	_perm(len(s))
	print perm_set
