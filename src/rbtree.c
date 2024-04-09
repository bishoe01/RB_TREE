#include "rbtree.h"

#include <stdio.h>
#include <stdlib.h>

// static const node_t NILL_NODE = {RBTREE_BLACK, -1, NULL, NULL, NULL};
// static const node_t const *t->nil = &NILL_NODE;

rbtree *new_rbtree(void) {
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
    node_t *nil = (node_t *)calloc(1, sizeof(node_t));
    nil->color = RBTREE_BLACK;
    p->nil = nil;
    p->root = nil;
    return p;
}
void delete_rbtree(rbtree *t) {
    // TODO: reclaim the tree nodes's memory
    free(t->nil);
    free(t);
}

void left_rotate(rbtree *t, node_t *x) {
    node_t *y = x->right;
    x->right = y->left;

    if (y->left != t->nil) {
        y->left->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == t->nil) {
        t->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

void right_rotate(rbtree *t, node_t *y) {
    node_t *x = y->left;
    y->left = x->right;

    if (x->right != t->nil) {
        x->right->parent = y;
    }

    x->parent = y->parent;

    if (y->parent == t->nil) {
        t->root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }

    x->right = y;
    y->parent = x;
}
node_t *rbtree_insert_fixup(rbtree *t, node_t *z) {
    node_t *uncle;
    while (z->parent->color == RBTREE_RED) {
        if (z->parent == z->parent->parent->left) {
            uncle = z->parent->parent->right;
            if (uncle->color == RBTREE_RED) {
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                uncle->color = RBTREE_BLACK;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(t, z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                right_rotate(t, z->parent->parent);
            }
        } else {
            uncle = z->parent->parent->left;
            if (uncle->color == RBTREE_RED) {
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                uncle->color = RBTREE_BLACK;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(t, z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                left_rotate(t, z->parent->parent);
            }
        }
    }
    t->root->color = RBTREE_BLACK;
}
node_t *rbtree_insert(rbtree *t, const key_t key) {
    node_t *temp = (node_t *)calloc(1, sizeof(node_t));
    temp->key = key;
    node_t *parent = t->nil;
    node_t *current = t->root;
    while (current != t->nil) {
        parent = current;
        if (current->key > key) {  // 현재 키값이 목적 키값보다 클때 왼쪽
            current = current->left;
        } else {
            current = current->right;
        }
    }
    temp->parent = parent;
    if (parent == t->nil) {  // 수정: '='을 '=='로 변경
        t->root = temp;
    } else if (temp->key < parent->key) {  // 부모보다 작으니까 왼쪽 자식이라는 것을 알 수 있음
        parent->left = temp;
    } else {
        parent->right = temp;
    }
    temp->left = t->nil;  // 추가: 새로 삽입된 노드의 left와 right를 nil로 설정
    temp->right = t->nil;
    temp->color = RBTREE_RED;  // 추가: 새로 삽입된 노드의 색상을 RED로 설정
    rbtree_insert_fixup(t, temp);
    return temp;  // 추가: 삽입된 노드의 포인터를 반환
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
    node_t *temp = t->root;
    while (temp != t->nil) {
        if (temp->key == key) {  // 찾으면
            return temp;
        } else if (temp->key < key) {  // 오른쪽으로
            temp = temp->right;
        } else {  // 왼쪽으로
            temp = temp->left;
        }
    }
    printf("리턴값 0\n");
    return NULL;
}

node_t *rbtree_min(const rbtree *t) {
    node_t *current = t->root;
    while (current != t->nil) {
        current = current->left;
    }
    return current;
}

node_t *rbtree_max(const rbtree *t) {
    node_t *current = t->root;
    while (current != t->nil) {
        current = current->right;
    }
    return current;
}

void transplant(rbtree *t, node_t *u, node_t *v) {
    if (u->parent == t->nil) {  // tree가 비어있을 때
        t->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

int rbtree_erase(rbtree *t, node_t *p) {
    node_t *y = p;
    color_t original_color = y->color;

    if (p->left == t->nil) {
    }

    if (p->parent == t->nil) {
        t->root = t->nil;
        free(p);
        return 0;
    } else {
        if (p->left == NULL) {
            p->parent->right = p->right;

        } else if (p->right == NULL) {
            p->parent->left = p->left;
        } else {
            node_t *current = p->right;
            while (current->left != t->nil) {
                current = current->left;
            }
            // min Value 찾음
            p->key = current->key;
            rbtree_erase(t, p->right);
            return 0;
        }
    }
    return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
    // TODO: implement to_array
    return 0;
}
