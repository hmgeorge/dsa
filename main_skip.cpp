#include <stdio.h>
#include <stdlib.h>
#include "list.h"

struct skip_node {
    int val;
    int height;
    struct listnode levels[0]; //membership in a bunch of lists
};

struct skip_list {
    float prob;
    struct skip_node * head;
};

static struct skip_node * mknode(int height)
{
    int node_size = sizeof(struct skip_node) + height*sizeof(struct listnode);
    struct skip_node * node = (struct skip_node *)calloc(node_size, 1);
    node->height = height;
    for (int i = 0; i < height; i++) {
        list_init(&node->levels[i]);
    }
    return node;
}

struct skip_list * mkskiplist(int height, float prob=.5)
{
    struct skip_list * l = (struct skip_list *)calloc(sizeof(struct skip_list),1);
    l->prob = prob;
    l->head=mknode(height);
    l->head->val=-99999; // some default min value
    return l;
}

struct skip_node * find_pred_node(struct skip_list *l, int val)
{
    // start at head node, and start at the top.
    // continue going through this level until a NULL is reached.
    // at this point go one level down.
    // repeat this until base is reached. this is where the node is
    // to inserted.
    struct listnode * base = NULL; 
    struct listnode * level = NULL;
    struct skip_node * node = NULL;
    struct skip_node * next_node = NULL;

    node = l->head;
    int height = node->height;
    while (height > 0) {
        level = &node->levels[height-1];
        if (list_head(level) == &(l->head->levels[height-1])) {
            --height;
            continue;
        }

        // fixed: pointer arithmetic mistake ..
        // did level->next - sizeof(listnode)*(height-1)
        next_node = node_to_item((level->next - (height-1)),
                                 struct skip_node, levels);

        //fprintf(stderr, "height %d, next_node->val %d\n", height, next_node->val);

        if (val > next_node->val) {
            node = next_node; // jumped. a better element can be found.
                              // starting from this node.
        } else {
            --height;
        }
    }

    return node;
}

bool insert_after(struct skip_node * node, struct skip_node * new_node)
{
    int max_height = new_node->height;
    int height = 1;
    while (height <= max_height) {
        while (height > node->height) {
            struct listnode * level = list_tail(&node->levels[(height-1)-1]);
            // go to this node's predecessor at this height
            // and insert it there
            if (level != NULL) {
                node = node_to_item((level - ((height-1)-1)), struct skip_node, levels);
            } // else shouldn't happen as that will only happen at head node
        }

        //fprintf(stderr, "add from %d -> %d at index %d\n", node->val,
        //new_node->val, height-1);
        list_add_head(&node->levels[height-1], &new_node->levels[height-1]);
        ++height;
    }
    return true;
}

bool skip_add(struct skip_list * l, int val)
{
    int height = 1;
    struct skip_node * node;
    while ((drand48() < l->prob) && (++height <= l->head->height));

    node = mknode(height);
    node->val = val;
    //fprintf(stderr, "new node %p height %d level %p\n", node, height, &node->levels[0]);
    struct skip_node * pred_node = find_pred_node(l, val);
    insert_after(pred_node, node);
    return true;
}

void skip_walk(struct skip_list *l)
{
    fprintf(stderr, "------------------------------\n");
    for (int i = 0; i < l->head->height; i++) {
        struct listnode *level, *temp;
        list_for_each_safe(level, temp, l->head->levels) {
            struct skip_node * node = node_to_item(level, struct skip_node, levels);
            //fprintf(stderr, "node %p node->val %d, node->height %d\n", node,
            //        node->val, node->height);
            if (i+1 > node->height) {
                fprintf(stderr, " %5c ", 'X');
            } else {
                fprintf(stderr, " %5d ", node->val);
            }
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "++++++++++++++++++++++++++++++\n");
}

struct skip_node * find_node(struct skip_list *l, int val)
{
    fprintf(stderr, "find_node %d\n", val);
    // start at head node, and start at the top.
    // continue going through this level until a NULL is reached.
    // at this point go one level down.
    // repeat this until base is reached. this is where the node is
    // to inserted.
    struct listnode * base = NULL; 
    struct listnode * level = NULL;
    struct skip_node * node = NULL;
    struct skip_node * next_node = NULL;

    node = l->head;
    int height = node->height;
    while (height > 0) {
        level = &node->levels[height-1];
        if (list_head(level) == &(l->head->levels[height-1])) {
            //fprintf(stderr,"new height %d\n", height-1);
            --height;
            continue;
        }

        // fixed: pointer arithmetic mistake ..
        // did level->next - sizeof(listnode)*(height-1)
        next_node = node_to_item((level->next - (height-1)),
                                 struct skip_node, levels);
#if 0
        fprintf(stderr, "level->next %p, offset %d, base %p, next_node %p\n",
                level->next,
                height-1,
                level->next - (height-1),
                next_node);
#endif
        if (val == next_node->val) {
//            fprintf(stderr, "found matching next_node %p val %d\n", next_node,val);
            return next_node;
        } else if (val > next_node->val) {
            node = next_node; // jumped. a better element can be found.
                              // starting from this node.
        } else {
            --height;
        }
    }
    fprintf(stderr, "couldn't find matching node val %d\n", val);
    return NULL;
}

static void del_node(struct skip_node * node)
{
    for (int i = 0; i < node->height; i++) {
        list_remove(&node->levels[i]);
    }

    free(node);
}

// delete first occurence of val
bool skip_del(struct skip_list *l, int val)
{
    struct skip_node * node = find_node(l, val);
    if (!node)
        return false;

    del_node(node);
    return true;
}

bool skip_free(struct skip_list *l)
{
    while (!list_empty(&l->head->levels[0])) {
        struct listnode *level = list_head(&l->head->levels[0]);
        struct skip_node * node = node_to_item(level, struct skip_node, levels);
        del_node(node);
    }

    free(l->head);
    free(l);
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(stderr, "%lx\n", sizeof(struct listnode));
        return -1;
    }
    srand48(atoi(argv[1]));
    struct skip_list * l = mkskiplist(10/*height*/);
    skip_add(l, 1);
    skip_walk(l);
    skip_add(l, 5);
    skip_walk(l);
    skip_add(l, 8);
    skip_walk(l);
    skip_add(l, 7);
    skip_add(l, 4);
    skip_walk(l);
    find_node(l, 10);
    skip_walk(l);
    find_node(l, 1);
    skip_del(l, 1);
    skip_walk(l);
    skip_del(l, 7);
    skip_walk(l);
    skip_free(l);
    return 0;
}
