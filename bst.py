import sys
import collections

class Node :
    def __init__(self, key):
        self.key = key
        self.left = None
        self.right = None
        self.parent = None

def walk(tree, visit_fn):
    def walk_l(tree, depth) :
        if tree == None:
            return
        walk_l(tree.left, depth + 1)
        if visit_fn != None :
            visit_fn(tree, depth)
        else :
            sys.stderr.write(tree.key + " ")
        walk_l(tree.right, depth + 1)

    walk_l(tree, 0)

"""
       swap node and node.right while maintaining bst property
                5
               / \
              2   7
                 / \
                6   8 

                ||
                \/

                7
               / \
              5   8
             / \
            2   6 

"""
def left_rotate(node):
    if node is None or node.right is None:
        return
    m = Node(-1)
    m.left = node.left
    m.right = node.right.left
    m.key = node.key
    node.key = node.right.key
    node.right = node.right.right
    node.left = m
    #at this point node.right should be a dangling node 

"""swap node and node.left while maintaining bst property"""
def right_rotate(node):
    if node is None or node.left is None:
        return
    m = Node(-1)
    m.right = node.right
    m.left = node.left.right
    m.key = node.key
    node.key = node.left.key
    node.left = node.left.left
    node.right = m
    #at this point node.right should be a dangling node 

def insert(tree, node):
    def insert_l(middle, node) :
        if node.key <= middle.key :
            if middle.left == None:
                middle.left = node
                node.parent = middle
                return node
            insert_l(middle.left, node)
        else :
            if middle.right == None:
                middle.right = node
                node.parent = middle
                return node
            insert_l(middle.right, node)

    return insert_l(tree, node)

def search(tree, key):
    def search_l(middle, key):
        if middle == None:
            return None

        if middle.key == key :
            return middle
        elif key < middle.key :
            return search_l(middle.left, key)
        else :
            return search_l(middle.right, key)

    return search_l(tree, key)

def predecessor(node) :
    #return max element on the left tree of node
    tree = node.left
    if tree == None :
        #go up the parent chain until you find an element
        #< node or reach root
        p = node
        while p.parent != None :
            if p.parent.key < node.key :
                break
            p = p.parent

        return p.parent
        

    while tree.right != None :
        tree = tree.right

    return tree

def successor(node) :
    #return min element on the right tree of node
    tree = node.right
    if tree == None :
        #go up the parent chain until you find an element
        #> node or reach root
        p = node
        while p.parent != None :
            if p.parent.key > node.key :
                break
            p = p.parent

        return p.parent

    while tree.left != None :
        tree = tree.left

    return tree


def delete(root, key):
    def delete_l(node) :
        #deal with leaf nodes
        if node.left == None and node.right == None:
            #delete me from my parent
            if node.parent.left == node :
                node.parent.left = None
            else :
                node.parent.right = None
            return

        succ = successor(node)

        if succ != None :
            node.key = succ.key
            return delete_l(succ)

        pred = predecessor(node)

        if pred != None :
            node.key = pred.key
            return delete_l(pred)

    #first search for the element
    node = search(root, key)
    if node == None:
        sys.stderr.write('Couldnt find elt to delete\n')
        return
    delete_l(node)

class BST :
    def __init__(self):
        self.root = None

    def insert(self, key):
        node = Node(key)
        if self.root == None:
            self.root = node
        else :
            insert(self.root, node)

        return node

    def walk(self, visit_fn = None):
        walk(self.root, visit_fn)
        if visit_fn == None :
            sys.stdout.write('\n')

    def delete(self, key):
        if self.root == None:
            return

        # don't think the node has to be replaced
        # just replacing the key should suffice?
        delete(self.root, key)

    def search(self, key):
        return search(self.root, key)

    def predecessor(self, key):
        node = self.search(key)
        if node == None :
            return None

        return predecessor(node)

    def successor(self, key):
        node = self.search(key)
        if node == None :
            return None

        return successor(node)

    def left_rotate(self, key):
        node = self.search(key)
        if node == None :
            return None
        return left_rotate(node)

def mkbst(elements):
    bst = BST()
    for e in elements:
        bst.insert(e)
    return bst

def test1() :
    bst = mkbst(['3', '5', '1', '0', '2', '8', '7', '9'])
    viz_dump(bst, 'graph-before-delete.js')

    #bst.walk()
    #n = bst.search('5')

    #print bst.predecessor('8').key
    #print bst.successor('7').key
    bst.delete('5')
    viz_dump(bst, 'graph-after-delete-1.js')

    #bst.walk()
    bst.delete('3')
    viz_dump(bst, 'graph-after-delete-2.js')

    #bst.walk()

def viz_dump(bst, path):
    nodes = []
    edges = []
    def visit_node(n, d) :
        nodes.append(n.key)
        if n.left != None :
            edges.append([n.key, n.left.key, "'l'"])
        if n.right != None :
            edges.append([n.key, n.right.key, "'r'"])

    bst.walk(visit_node)
    with open(path, 'w+') as f :
        f.write('var nodes = [\n')
        for n in nodes :
            f.write("{id: %s, label: '%s'},\n" % (n, n))
        f.write('];\n')

        f.write('var edges = [\n')
        for e in edges :
            f.write("{from: %s, to: %s, label: %s},\n" % (e[0], e[1], e[2]))
        f.write('];\n')
        f.write("var graph = {nodes:nodes,edges:edges,dir:'UD',type:'tree'}\n");
        f.close()

def test2() :
    bst = mkbst(['5', '2', '7', '8', '6'])
    bst.walk()
    levels = collections.defaultdict(list)
    bst.walk(lambda node, depth : levels[depth].append(node.key))
    for d in sorted(levels.keys()):
        sys.stderr.write('%s\n' % ('  '.join(levels[d])))

def test3():
    bst = mkbst(['5', '2', '7', '8', '6'])
    viz_dump(bst, 'graph-before-rotate.js')
    bst.left_rotate('7')
    viz_dump(bst, 'graph-after-rotate.js')

def xmain():
    test1()

if __name__ == "__main__":
    xmain()
