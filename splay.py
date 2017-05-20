"""
Splay is a BST with self-balancing properties
"""
import bst
import sys

class SplayTree(bst.BST):
    def __init__(self) :
        super(SplayTree, self).__init__()

    def splay(self, node) :
        """
        initial idea: rotate node and parent all the way to root causes perf problems
        e.g. bst
        1
          2
            3
              4
                5
        access(5) - rotate all the way to root
        access(4) - rotate all the way to root
        ...
        ...
        access(1) - rotate and we will be back at the beginning.

        instead follow the splaying rules..
        """
        #ref page 15 of http://web.stanford.edu/class/cs166/lectures/08/Small08.pdf
        while node.parent != None :
            #am i a left child or my parent (zig, zig-zig, zig-zag)
            if node.parent.left == node:
                #which mode am i?
                if node.parent.parent == None : #zig case, node.parent is root
                    bst.left_rotate(node)
                elif node.parent.parent.left == node.parent : #zig-zig
                                                              #node.parent is left of node.grandparent
                    bst.left_rotate(node.parent)
                    bst.left_rotate(node)
                else:                                         #zig-zag
                                                              #node.parent is right of node.grandparent
                    bst.left_rotate(node)
                    bst.right_rotate(node)
            else:
                if node.parent.parent == None: #zag case, node.parent is root
                    bst.right_rotate(node)
                elif node.parent.parent.right == node.parent: #zag-zag
                                                              #node.parent is right of node.grandparent
                    bst.right_rotate(node.parent)
                    bst.right_rotate(node)
                else:                                         #zag-zig
                                                              #node.parent is left of node.grandparent
                    bst.right_rotate(node)
                    bst.left_rotate(node)

            #in all the above cases, node was moved upwards.
            #no need to set node to node.parent


    """
    In addition to inserting in the tree, we need to splay the node that was inserted
    """
    def insert(self, key):
        node = super(SplayTree, self).insert(key)
        self.splay(node)

    def lookup(self, key):
        node = super(SplayTree, self).search(key)
        if node == None:
            #the pdf says splay the last visited node... thats TBD
            #needs changes in bst.py search to return middle if middle.left
            #or middle.right is None. Not sure how much this will affect
            #the performance
            return False
        self.splay(node)
        return True

    def join(self, t1, t2): #all keys in t1 are less than t2
        max_node = t1
        while max_node.right != None:
            max_node = max_node.right

        self.splay(max_node)
        max_node.right = t2

    def delete(self, key):
        node = super(SplayTree, self).search(key)
        if node == None:
            return False

        self.splay(node)
        t1 = node.left
        t2 = node.right
        self.join(t1, t2)

    def split(self, key):
        node = super(SplayTree, self).search(key)
        succ_node = bst.successor(node)
        self.splay(succ_node)
        succ_node.left = None

def mksplay(elements):
    s = SplayTree()
    for e in elements :
        s.insert(e)
    return s

def xmain():
    s = mksplay(['1', '2', '3', '4', '5', '6'])
    bst.viz_dump(s, 'graph-initial.js')
    s.delete(4)
    s.delete(6)
    s.delete(2)
    bst.viz_dump(s, 'graph-after-delete.js')
    s.lookup(5)
    s.lookup(1)

if __name__ == "__main__":
    xmain()

