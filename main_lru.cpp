#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#define LRU_CAPACITY 4

struct lru_item {
    struct listnode entry; //membership in lru_list/free_list
    int item;
    bool valid;
};

struct lru_cache {
    int size;
    struct listnode lru_list;
    struct listnode free_list;
    struct lru_item items[LRU_CAPACITY];
};

struct lru_cache * mklru()
{
    struct lru_cache * l = (struct lru_cache *)(calloc(sizeof(struct lru_cache), 1));
    list_init(&l->lru_list);
    list_init(&l->free_list);
    int i;
    for (i = 0; i < LRU_CAPACITY; i++) {
        list_add_tail(&l->free_list, &(l->items[i].entry));
    }
    return l;
}

bool lru_add(struct lru_cache * cache, int item)
{
    struct listnode * lru_list = &cache->lru_list;
    struct listnode * free_list = &cache->free_list;
    struct listnode *node, *next;
    struct lru_item * it;

    list_for_each_safe(node, next, lru_list) {
        it = node_to_item(node, struct lru_item, entry);
        if (it->item == item) {
            fprintf(stderr, "item %d already exists\n", item);
            return false;
        }
    }

    if (cache->size == LRU_CAPACITY) {
        // evict tail and add this item
        it = node_to_item(list_tail(lru_list), struct lru_item, entry);
        fprintf(stderr, "evict item %d and replace with %d\n", it->item, item);
        it->item = item;
    } else {
        // take head from free_list and add to tail
        node = list_head(free_list);
        list_remove(node);
        it = node_to_item(node, struct lru_item, entry);
        it->item = item;
        list_add_tail(lru_list, node);
        ++cache->size;
    }
    it->valid = true;
    return true;
}

bool lru_del(struct lru_cache * cache, int item)
{
    struct listnode * lru_list = &cache->lru_list;
    struct listnode * free_list = &cache->free_list;
    struct listnode *node, *next;

    list_for_each_safe(node, next, lru_list) {
        struct lru_item * i = node_to_item(node, struct lru_item, entry);
        if (i->item == item) {
            i->valid = false;
            list_remove(node);
            list_add_tail(free_list, node);
            --cache->size;
            return true;
        }
    }
    return false;
}

bool lru_find(struct lru_cache * cache, int item)
{
    struct listnode * lru_list = &cache->lru_list;
    struct listnode * free_list = &cache->free_list;
    struct listnode *node, *next;

    list_for_each_safe(node, next, lru_list) {
        struct lru_item * i = node_to_item(node, struct lru_item, entry);
        if (i->item == item) {
            fprintf(stderr, "move item %d to front\n", item);
            list_remove(node);
            list_add_head(lru_list, node);
            return true;
        }
    }
    return false;
}

void lru_walk(struct lru_cache * cache)
{
    struct listnode * lru_list = &cache->lru_list;
    struct listnode *node, *next;
    list_for_each_safe(node, next, lru_list) {
        struct lru_item * i = node_to_item(node, struct lru_item, entry);
        fprintf(stderr, "%d", i->item);
        if (list_head(node) != lru_list) {
            fprintf(stderr, "->");
        }
    }
    fprintf(stderr, "\n");
}

void lru_free(struct lru_cache * cache) {
    struct listnode * lru_list = &cache->lru_list;
    struct listnode * free_list = &cache->free_list;

    while (!list_empty(lru_list)) {
        list_remove(list_head(lru_list));
    }

    while (!list_empty(free_list)) {
        list_remove(list_head(free_list));
    }
}

int main(int argc __unused, char** argv __unused) {
    struct lru_cache * lru = mklru();
    lru_add(lru, 1);
    lru_add(lru, 2);
    lru_add(lru, 3);
    fprintf(stderr, "%s found 1\n", lru_find(lru, 1) ? "" : "not");
    fprintf(stderr, "%s found 4\n", lru_find(lru, 4) ? "" : "not");
    lru_add(lru, 1);
    lru_add(lru, 2);
    lru_add(lru, 3);
    lru_add(lru, 4);
    fprintf(stderr, "%s found 1\n", lru_find(lru, 1) ? "" : "not");
    fprintf(stderr, "%s found 4\n", lru_find(lru, 4) ? "" : "not");
    lru_del(lru, 1);
    lru_del(lru, 3);
    lru_add(lru, 5);
    lru_add(lru, 6);
    lru_walk(lru);
    lru_free(lru);
    fprintf(stderr, "all done\n");
    return 0;
}
