#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* An implementation of regex evaluation based on Thompson's NFA
   algorithm */

struct listnode {
    struct listnode *prev;
    struct listnode *next;
};

#define node_to_item(node, container, member) \
  (container *) (((char*) (node)) - offsetof(container, member))

struct listnode *list_begin(struct listnode *h) { return h->next; }
struct listnode *list_end(struct listnode *h) { return h; }
struct listnode *list_next(struct listnode *n) { return n->next; }
int list_empty(struct listnode *n) { return list_begin(n) == n; }
struct listnode *list_prev(struct listnode *n) { return n->prev; }
struct listnode *list_rev(struct listnode *h) { return h->prev; }

void list_init(struct listnode *l) {
    l->prev=l->next=l;
}

void list_add_tail(struct listnode *head, struct listnode *item) {
    item->next = head;
    item->prev = head->prev;
    head->prev->next = item;
    head->prev = item;
}

void list_add_head(struct listnode *head, struct listnode *item) {
    item->prev = head;
    item->next = head->next;
    head->prev->prev = item;
    head->next = item;
}

void list_remove(struct listnode *item) {
    item->prev->next = item->next;
    item->next->prev = item->prev;
}

typedef enum {
    CHAR=10,
    ALT,
    STAR,
    QUEST,
    PLUS,
    ACC,
} type_t;

static const char *text;
static int cursor;
static inline char getch() { return text[cursor]; }
static inline int pos() { return cursor; }
static inline int len() { return strlen(text); }
static inline int eos() { return pos()==len(); }
static inline void inc() { ++cursor; }

//partial machine
typedef struct mach {
    type_t type;
    struct listnode node; // membership in alt-list, needed outside of union
                          // as a mach can be start of a sub-group. such a mach must be
                          // added to the alt_list
    union {
        struct {
            char c;
            struct mach *e1; //points to next mach
        } ch;
        struct {
            struct mach *e1; // points to mach for match
            struct mach *e2; // epsilon transition to next mach
        } quest;
        struct {
            struct mach *e1; // points to mach for match. e1 in turn should point back to this mach
            struct mach *e2; // epsilon transition to next mach
        } star;
        struct {
            struct mach *e1; // e1 of type plus points to a char which points to star
            struct mach *e2; // not used in the case of plus, used internally to hide pointer to the star
        } plus;
    } nfa;
} mach;

static mach *start;
static int branch;
static mach *sub_start;

// need a separate eval_node as a mach can belong to multiple
// nodes
typedef struct eval_node {
  mach *m;
  struct listnode node;
} eval_node;

static int connect(mach *this, mach *to) {
    switch (this->type) {
        case PLUS:
            switch (to->type) {
                case ALT:
                    fprintf(stderr, "connect to alt not needed until paranthesis is supported\n");
                    exit(-1);
                default:
                    // this is only valid for plus as e2 stores the pointer to the star mach
                    connect(this->nfa.plus.e2, to);
                    break;
            }
            break;
        case CHAR:
            switch (to->type) {
                case ALT:
                    fprintf(stderr, "connect to alt not needed until paranthesis is supported\n");
                default:
                    this->nfa.ch.e1 = to;
                    break;
            }
            break;
        case STAR:
            switch (to->type) {
                case ALT:
                    fprintf(stderr, "connect to alt not needed until paranthesis is supported\n");
                default:
                    this->nfa.star.e2 = to; //epsilon transition
                    break;
            }
            break;
        case QUEST:
            switch (to->type) {
                case ALT:
                    fprintf(stderr, "connect to alt not needed until paranthesis is supported\n");
                    exit(-1);
                    break;
                default:
                    this->nfa.quest.e2 = to; //epsilon transition
                    connect(this->nfa.quest.e1, to); // connect the char mach for match to to as well
                    break;
            }
            break;
        case ALT:
            switch (to->type) {
                case ALT:
                    fprintf(stderr, "alt->alt?\n");
                    exit(-1);
                default:
                    fprintf(stderr, "connect to alt not needed until paranthesis is supported\n");
                    // this->nfa.alt.e2 = to;
                    break;
            }
        default:
            fprintf(stderr, "unknown this type\n");
            exit(-1);
    }
    return 0;
}

static mach *mk_accept() {
    mach *m = (mach *)calloc(1, sizeof(mach));
    m->type = ACC;
    list_init(&m->node);
    return m;
}

static mach *mk_alt() {
    mach *m = (mach *)calloc(1, sizeof(mach));
    m->type = ALT;
    list_init(&m->node);
    return m;
}

static mach *mk_char(char c) {
    mach *m = (mach *)calloc(1, sizeof(mach));
    m->type = CHAR;
    m->nfa.ch.c = c;
    list_init(&m->node);
    return m;
}

static mach *clone(mach *m) {
    mach *cm = (mach *)calloc(1, sizeof(mach));
    memcpy(cm, m, sizeof(mach));
    list_init(&cm->node);
    return cm;
}

static mach *mk_mach(type_t t, char c) {
    mach *m = (mach *)calloc(1, sizeof(mach));
    list_init(&m->node);
    mach *cm = mk_char(c);
    m->type = t;
    switch (t) {
        case PLUS: {
            mach *m2 = mk_mach(STAR, c);
            connect(cm, m2);
            m->nfa.plus.e1 = cm;
            // hidden pointer to the star mach. used only in connect
            m->nfa.plus.e2 = m2;
            break;
        }
        case QUEST:
            m->nfa.quest.e1 = cm;
            break;
        case STAR:
            m->nfa.star.e1 = cm;
            cm->nfa.ch.e1 = m;
            break;
        default:
            fprintf(stderr, "unknown type\n");
            exit(-1);
    }
    return m;
}

static char parse_char(char c, char prev_c, mach **current) {
    mach *m = NULL;
    char ret = 0;

    // printf("%c %c\n", c==0?'0':c, prev_c == 0 ? '0' : prev_c);
    switch (c) {
        case '+':
            assert(prev_c != 0); // do exps like a*+ make sense?
            m = mk_mach(PLUS, prev_c);
            break;
        case '?':
            assert(prev_c != 0);
            m = mk_mach(QUEST, prev_c);
            break;
        case '*':
            assert(prev_c != 0);
            m = mk_mach(STAR, prev_c);
            break;
        case '$':
        case '|':
            if (prev_c != 0) {
                printf("make last char with %c for %c\n", prev_c, c);
                m = mk_char(prev_c);
            } else {
                if (*current == NULL) {
                    fprintf(stderr, "bad exp\n");
                    exit(-1);
                }
            }
            break;
        default:
            if (prev_c != 0) {
                m = mk_char(prev_c);
                ret = c;
            } else {
                // this implies the previous char
                // was an alt or a a meta char and
                // we can't take action on this char
                // until we see the next char
                return c;
            }
            break;
    }

    if (m != NULL) {
        if (sub_start == NULL) {
            sub_start = m;
        } else {
            connect(*current, m);
        }
        *current = m;
    }

    if (c == '|' || c == '$') {
      list_add_tail(&start->node, &sub_start->node);
      ++branch;
      mach *acc_mach = mk_accept();
      connect(*current, acc_mach);
      sub_start = NULL;
    }
    return ret;
}

mach *parse_regexp(const char *exp) {
    int i;
    char prev_c = exp[0];
    mach *current = NULL;

    start = mk_alt();
    for (i=1; i<strlen(exp); i++) {
        prev_c = parse_char(exp[i], prev_c, &current);
        // printf("prev_c %c\n", prev_c);
    }
    parse_char('$', prev_c, &current);
    printf("branches %d\n", branch);
    return 0;
}

static void eval_add(eval_node *eval, mach *m) {
    eval_node *n = (eval_node *)calloc(1, sizeof(eval_node));
    list_init(&n->node);
    if (!m) {
        fprintf(stderr, "null m in eval_add\n");
        exit(-1);
    }
    n->m = m;
    list_add_tail(&eval->node, &n->node);
}

static int enter(mach *m, eval_node *eval) {
    mach *sub_m;
    struct listnode *sub_node;

    // printf("getch() %c\n", getch());
    switch (m->type) {
        case ACC:
            // printf("reached accept mach\n");
            if (pos() == (len()-1)) {
                fprintf(stderr, "matches\n");
                return 0;
            }
            break;
        case CHAR:
            // printf("type char %d..\n", m->nfa.ch.c);
            if (getch() == m->nfa.ch.c) {
                 // printf("char matches %c\n", m->nfa.ch.c);
                 eval_add(eval, m->nfa.ch.e1); // not epsilon
            }
            break;
        case STAR:
            // printf("type STAR\n");
            if (m->nfa.star.e2) { // epsilon
                enter(m->nfa.star.e2, eval);
            }
            enter(m->nfa.star.e1, eval);
            break;
        case PLUS:
            enter(m->nfa.plus.e1, eval); //epsilon
            break;
        case QUEST:
            enter(m->nfa.plus.e1, eval); //epsilon
            if (m->nfa.plus.e2) {
                enter(m->nfa.plus.e2, eval); //epsilon
            }
            break;
        case ALT:
            // printf("type ALT\n");
            sub_node = list_begin(&start->node);
            while (sub_node != list_end(&start->node)) {
                sub_m = (mach *)node_to_item(sub_node, mach, node);
                eval_add(eval, sub_m); // not a transition . only used to set up all branches
                sub_node = list_next(sub_node);
            }
            break;
        default:
            fprintf(stderr, "%s unknown type %d\n", __func__, m->type);
            exit(-1);
            break;
    }
    return -1;
}

static int eval() {
    eval_node head, new_head;
    list_init(&head.node);
    list_init(&new_head.node);
    enter(start, &head);
    struct listnode *n;
    while (!eos()) {
        while (!list_empty(&head.node)) {
            n = list_begin(&head.node);
            eval_node *ev = node_to_item(n, eval_node, node);
            list_remove(n);
            enter(ev->m, &new_head);
            free(ev);
        }
        inc();
        list_init(&head.node);
        while (!list_empty(&new_head.node)) {
            n = list_begin(&new_head.node);
            list_remove(n);
            list_add_tail(&head.node, n);
        }
    }
    return 0;
}

/*
abc*|cde|ef*g

s  c                   s //s.e = e1
|  |                   | //s.e = e2->e1
v  v                   v //s.e = e3->e2->e1
abc* |       ==>      e1 e2 ef*g
     ^
     |
   mk_alt
*/
int main(int argc, char **argv) {
    parse_regexp(argv[1]);
    text=argv[2];
    eval();
}

/*
haynes@haynes-laptop:~/works/regex$ ./a.out "x*ab|a*y?|x*a" "aaay$"
make last char with b for |
make last char with a for $
branches 3
matches
haynes@haynes-laptop:~/works/regex$ ./a.out "x*ab|a*y?|x*a" "aavay$"
make last char with b for |
make last char with a for $
branches 3
haynes@haynes-laptop:~/works/regex$ ./a.out "a*bb" "ab$"
make last char with b for $
branches 1
*/
