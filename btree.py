import sys
import collections

"""
This module creates a b-tree of order b. This tree has the following properties
1) All leaves are in the same level
2) A node has [b-1, 2b-1] keys
3) The root has atmost 2b-1 keys

based off: http://web.stanford.edu/class/archive/cs/cs166/cs166.1146/lectures/02/Small02.pdf
PS: Order is defined (slightly) differently. Other definitions define order
based on the number of children for a node.
"""
class BNode():
    def __init__(self, order, parent):
        self.keys = []
        self.ways = []
        self.parent = parent
        self.order = order
        self.debug = False

    def insert(self, key):
        i = 0
        #can be replaced by binary search
        while i < len(self.keys):
            if key < self.keys[i] :
                break
            i += 1
        self.keys.insert(i, key)
        return i

    def insert_way(self, idx, node):
        self.ways.insert(idx, node)

    def count(self):
        return len(self.keys)

    def isleaf(self):
        return len(self.ways) == 0

    #invalid to call immediately after a split
    def isroot(self):
        return self.parent == None

    def has_key(self, key):
        try :
            i = self.keys.index(key)
        except ValueError:
            i = -1
        return i >= 0, i

    def find_way(self, key):
        if self.isleaf():
            return None

        new_way = None
        for i in range(len(self.keys)) :
            if key <= self.keys[i] :
                return self.ways[i], i

        return self.ways[len(self.keys)], len(self.keys)

    def split(self):
        if len(self.keys) < 2 * self.order :
            return
        order = self.order
        mid = self.keys[order]
        r_node = BNode(order, self.parent)
        r_node.keys = self.keys[order+1:]
        #need to split the ways as well.
        #hypothesis: if a node is not a leaf node, it will have >= order + 1
        #ways because of the way a b-tree is constructed
        if not self.isleaf():
            nways = len(self.ways)
            if nways < (order + 1) :
                sys.stderr.write('assertion error: node needs split, has %d ways < order + 1\n' % (nways))
                sys.stderr.write('keys %s\n' % ('|'.join([str(k) for k in n.keys])))
                sys.stderr.write('ways\n')
                for w in self.ways:
                    sys.stderr.write(' way - %s' % (('|'.join([str(k) for k in w.keys]))))
                sys.stderr.write('exiting..\n')
                sys.exit(-1)
            r_node.ways = self.ways[order+1:]
            self.ways = self.ways[:order+1]
        self.keys = self.keys[:order]
        return mid, r_node

    def upper_limit(self):
        return 2 * self.order - 1

    def lower_limit(self):
        return self.order - 1

    def walk(self, visit_fn):
        if len(self.keys) == 0 :
            return

        visit_fn(self)
        for n in self.ways:
            n.walk(visit_fn)

    def remove_way_at(self, i):
        self.ways.pop(i)

    def remove_at(self, i):
        try :
            return self.keys.pop(i)
        except IndexError:
            return None

    def fix(self):
        #tree goes upwards. fix looks at current node
        #and splits this into two if the number of keys exceeds a limit.
        #A new node is created with the right half of the keys.
        #min from the right half is inserted into the parent and the new
        #node is added as a way from the parent. process is repeated at node's parent.
        if self.count() <= self.upper_limit() :
            return

        mid, r_node = self.split()
        if self.debug:
            print "after split ", mid, r_node.keys
            
        #special case when the root is split
        #create root node and assign that as parent
        #of both node and r_node
        if self.parent == None:
            if self.debug:
                    print "create new root"
            self.parent = BNode(self.order, None)
            self.parent.insert_way(0, self)
            r_node.parent = self.parent

        #insert mid in to the parent
        #insert r_node into parent's way list right after
        #node in the way list
        idx = self.parent.insert(mid)

        if self.debug:
            print "mid inserted at pos ", idx

        self.parent.insert_way(idx + 1, r_node)
        self.parent.fix()

    def child(self, w):
        try :
            return self.ways[w]
        except IndexError:
            return None

    def merge(self, other_node):
        for w in other_node.ways:
            w.parent = self
        self.keys.extend(other_node.keys)
        #only one call to fix() as we are fusing two child nodes each with
        #max 2b-1 keys. after the extend op, the left node can have max
        #4b - 2 keys. after split, left node will still be with 2b-1 while
        #the right node will have max 2b keys.
        #optimization : is the merge really needed? maybe min key can be picked
        #from right node directly.. but then what if the right way has just one
        #key .... by removing it, we need to delete the right node anyway.
        self.fix()

    def successor(self, key, way=0) :
        i = -1
        try:
            i = self.keys.index[key]
        except IndexError:
            return [None, None, None]

        n = self.child(i+1)
        while not n.isleaf():
            n = n.child(0)

        return [n[0], 0, 0]

    def replace(self, i, new_key):
        try :
            self.keys[i] = new_key
        except IndexError:
            return False
        return True

class BTree :
    def __init__(self, order):
        self.root = None
        self.order = order
        self.debug = False

    def insert(self, key):
        if self.root == None :
            self.root = BNode(self.order, None)
            self.root.insert(key)
            return

        node = self.root
        if self.debug :
            print node.keys
        while not node.isleaf() :
            node, _ = node.find_way(key)

        if self.debug:
            print "leaf node to insert", node.keys
        leaf = node
        node.insert(key)
        node.fix()

        #update root as it might have changed
        while self.root.parent != None:
            self.root = self.root.parent

    def find(self, key):
        i = -1
        w = -1
        found = False
        node = self.root
        while not node.isleaf() :
            found, i = node.has_key(key)
            if found:
                break
            node, w = node.find_way(key) 

        if not found:
            #could be that it is in the leaf
            found, i = node.has_key(key)

        if not found:
            sys.stderr.write("could not find key %d for delete\n" % (key))
            return [None, -1, -1]

        return [node, w, i]

    def delete(self, key):
        """
        find node where the key is
        if key is in a leaf node:
            remove key from leaf node
            if len(node.keys) < lower_limit:
                #need a replacement
                ? is it on an adjacent node <-- yes if that doesn't underflow
                  (note that it is easy to find an adjacent node since for a leaf
                  which is way i for a parent p .. just check way i + 1)
                  pull parent.key(i) down and replace it with min child from way i + 1
                ? if adjacent underflows, take parent down (self.parent.keys[i]) fuse
                  and fix
        else if key is in root:
            remove key
                fuse way[i] and way[i+1]
                fix the new child object
        else:
            #key was found on an internal node
            replace key with successor
            (found by right most element in right most leaf starting from way i + 1)
            remove successor from leaf
            fix(leaf) <- == deleting leaf node.
        """
        res = self.find(key)
        node = res[0]
        way = res[1]
        i = res[2]

        if node is None:
            return

        def delete_l(node, way, i) :
            #root is checked first as that won't have a way (i.e way = -1 even though
            #key was found)

            if node.isroot():
                node.remove_at(i)
                node_dst = node.child(i)
                node_src = node.child(i+1)

                if node.count() == 0 :
                    #this means there are two children for this node.
                    #make node_dst, the new root of the tree
                    node_dst.parent = None
                    node_src.parent = None
                    self.root = node_dst

                node.remove_way_at(src)
                node_dst.merge(node_src)
            elif node.isleaf():
                node.remove_at(i)
                if node.count() >= node.lower_limit():
                    return
                adj_node = node.parent.child(way + 1)
                if adj_node is None:
                    k = node.parent.remove_at(way - 1, False)
                    node.insert(k)
                    node.parent.remove_way_at(way)
                    node.parent(way-1).merge(node)
                elif adj_node.count() == adj_node.lower_limit() :
                    k = node.parent.remove_at(way, False)
                    node.insert(k)
                    node.parent.remove_way_at(way+1)
                    node.merge(adj_node)
                else :
                    #steal from adj_node
                    k = node.parent.remove_at(way, False)
                    node.insert(k)
                    k1 = adj_node.remove_at(0, False)
                    node.parent.insert(k1)
                    adj_node.remove_way_at(1)
                    adj_node.way[0].merge(adj_node.way[1])
            else:
                res = node.successor(key, way=i+1)
                s_node = res[0]
                s_key_idx = res[1]
                s_way = res[2]
                s_node.replace(s_key_idx, key)
                node.replace(i, k)
                delete_l(s_node, s_way, s_key_idx)

        delete_l(node, way, i)

    def walk(self, visit_fn):
        self.root.walk(visit_fn)

def viz_dump(btree, path):
    nodes = []
    edges = []
    def visit_node(n):
        key_str = '%s' % '|'.join([str(k) for k in n.keys])
        ### note use of id() as a uniq idenitifier for this node
        nodes.append((id(n), key_str))
        count = 0
        for w in n.ways:
            edges.append([id(n), id(w), count])
            count += 1

    btree.walk(visit_node)
    with open(path, 'w+') as f :
        f.write('var nodes =[\n')
        for n in nodes :
            f.write("{id : %s, label: '%s', shape: 'box'},\n" % (n[0], n[1]))
        f.write('];\n')

        f.write('var edges = [\n')
        for e in edges :
            f.write("{from : %s, to: %s, label: %d},\n" % (e[0], e[1], e[2]))

        f.write('];\n')
        f.write("var graph = {nodes:nodes,edges:edges,dir:'UD',type:'tree'}\n");
        f.close()

def mkbtree(order, elements):
    btree = BTree(order)
    for e in elements:
        btree.insert(e)
    return btree

def test1():
    btree = mkbtree(2, [5, 2, 7, 8, 6])
    viz_dump(btree, 'graph-btree.js')
    btree.debug = True
    [btree.insert(e) for e in [9, 10, 11, 1]]
    viz_dump(btree, 'graph-btree-2.js')
    btree.insert(3)
    viz_dump(btree, 'graph-btree-3.js')
    btree.insert(4)
    viz_dump(btree, 'graph-btree-4.js')

def xmain():
    test1()

if __name__ == "__main__":
    xmain()
