import os
import sys
import json
import subprocess

broot = os.getenv('ANDROID_BUILD_TOP')

def croot():
    global broot
    if (broot is None):
        sys.stderr.write('setup build first\n')
        sys.exit(-1)

    os.chdir(broot)

def jsondecode(f):
    args = 'r+'
    if (not os.path.exists(f)) :
        sys.stderr.write('%s doesnt exist, creating now' % (f))
        args = 'w+'

    fd = open(f, args)
    res = json.load(fd)
    fd.close()

    return res 

def jsonencode(obj, f):
    args = 'r+'
    if (not os.path.exists(f)) :
        sys.stderr.write('%s doesnt exist, creating now' % (f))
        args = 'w+'

    fd = open(f, args)
    json.dump(obj, fd)
    fd.close()

def top_sort(d):
    graph = dict(d)
    sorted_list = []
    while graph:
        to_remove = []
        for v in graph.iterkeys():
            if len(graph[v]) == 0 :
                sorted_list.append(v)
                to_remove.append(v)

        for _, e in graph.iteritems():
            for v in to_remove:
                if v in e:
                    e.remove(v)

        for v in to_remove:
            del graph[v]

        if len(to_remove) == 0 :
            sys.stderr.write("bad graph or loop detected\n")
            return []

    return sorted_list

def execute(cmd) :
    return subprocess.Popen(cmd,
                            executable=None, stdin=None, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            preexec_fn=None, close_fds=False, shell=True)

def jsonparser(jstrings, filterfunc):
    for js in jstrings.splitlines():
        d = json.loads(js)
        if d is None:
            continue
        if (filterfunc(d)):
            yield d

def jsongenerator(outs, jfilter):
    o = jsonparser(outs, jfilter)
    while True:
        try:
            yield next(o)
        except StopIteration:
            pass

def gerrit_query(Id, opt_args=['--current-patch-set']) :
    base_cmd = 'ssh -p 29418 review-android.quicinc.com gerrit query --format=JSON'
    cmd = '%s %s %s' % (base_cmd, ''.join(arg + ' ' for arg in opt_args), Id)
    if DEBUG :
        sys.stderr.write("querying Id %s\n" % (Id))
    return execute(cmd).communicate()
