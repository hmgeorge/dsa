#!/usr/bin/python
def top_sort(d):
    graph = dict(d)
    sorted_list = []
    while graph:
        to_remove = []
        for v in graph.iterkeys():
            if len(graph[v]) == 0 :
                sorted_list.append(v)
                to_remove.append(v)

        for e in graph.itervalues():
            for v in to_remove:
                if v in e:
                    e.remove(v)

        for v in to_remove:
            del graph[v]

        if (len(to_remove) == 0) :
            print "bad graph or loop detected"
            return []

    return sorted_list

if __name__ == "__main__":
    graph = dict()
    graph[1] = [2,3]
#    graph[2] = [4]
#    graph[3] = [4]
#    graph[4] = [1]
    print graph
    print top_sort(graph)

