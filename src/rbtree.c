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
void inorderDelete(rbtree *t, node_t *node) {
    if (node != t->nil) {
        node_t *left = node->left;
        node_t *right = node->right;
        free(node);
        inorderDelete(t, left);
        inorderDelete(t, right);
    }
}

void delete_rbtree(rbtree *t) {
    inorderDelete(t, t->root);
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
    temp->left = t->nil;  // 새 노드 초기화
    temp->right = t->nil;
    temp->color = RBTREE_RED;  // 새 노드 RED
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
    return NULL;
}
node_t *rbtree_min(const rbtree *t) {
    node_t *current = t->root;
    while (current->left != t->nil) {
        current = current->left;
    }
    return current;
}

node_t *rbtree_max(const rbtree *t) {
    node_t *current = t->root;
    while (current->right != t->nil) {
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

node_t tree_minimun(rbtree *t, node_t *current) {
    while (current != t->nil) {
        current = current->left;
    }
    return *current;
}

void delete_fixup(rbtree *t, node_t *x) {
    // 노드 x가 루트가 아니고, BLACK인 동안 반복
    while (x != t->root && x->color == RBTREE_BLACK) {
        // x가 왼쪽 자식인 경우
        if (x == x->parent->left) {
            // 형제 노드 설정
            node_t *sibling = x->parent->right;

            // 형제가 RED인 경우
            if (sibling->color == RBTREE_RED) {
                // 형제의 색을 BLACK으로,  부모를 RED로 변경함ㄴ
                sibling->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                // 부모 노드 기준으로 왼쪽 회전
                left_rotate(t, x->parent);
                // 새로운 형제 노드 설정
                sibling = x->parent->right;
            }

            // 형제가 BLACK이고, 형제의 자식 둘다 BLACK
            if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) {
                // 형제를 RED로 변경
                sibling->color = RBTREE_RED;
                // x를 부모 노드로 이동
                x = x->parent;
            } else {
                // 형제가 BLACK이고, 형제의 왼쪽 자식이 RED, 오른쪽 자식이 BLACK인 경우
                if (sibling->right->color == RBTREE_BLACK) {
                    // 형제의 왼쪽 자식 BLACK으로, 형제를 RED로 변경
                    sibling->left->color = RBTREE_BLACK;
                    sibling->color = RBTREE_RED;
                    // 형제 노드 기준으로 오른쪽 회전
                    right_rotate(t, sibling);
                    // 새로운 형제 노드 설정
                    sibling = x->parent->right;
                }

                // 형제가 BLACK이고, 형제의 오른쪽 자식이 RED인 경우
                // 형제를 부모의 색으로 변경
                sibling->color = x->parent->color;
                // 부모의 BLACK으로, 형제의 오른쪽 자식 BLACK으로 변경
                x->parent->color = RBTREE_BLACK;
                sibling->right->color = RBTREE_BLACK;
                // 부모 노드 기준으로 왼쪽 회전
                left_rotate(t, x->parent);
                // x를 루트로 이동하여 반복 종료
                x = t->root;
            }
        }
        // x가 오른쪽 자식인 경우 (왼쪽 자식인 경우와 대칭)
        else {
            node_t *sibling = x->parent->left;

            // 형제가 RED인 경우
            if (sibling->color == RBTREE_RED) {
                sibling->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotate(t, x->parent);
                sibling = x->parent->left;
            }

            // 형제가 BLACK이고, 형제의 양쪽 자식 노드도 BLACK인 경우
            if (sibling->right->color == RBTREE_BLACK && sibling->left->color == RBTREE_BLACK) {
                sibling->color = RBTREE_RED;
                x = x->parent;
            } else {
                // 형제가 BLACK이고, 형제의 오른쪽 자식이 RED, 왼쪽 자식이 BLACK인 경우
                if (sibling->left->color == RBTREE_BLACK) {
                    sibling->right->color = RBTREE_BLACK;
                    sibling->color = RBTREE_RED;
                    left_rotate(t, sibling);
                    sibling = x->parent->left;
                }

                // 형제가 BLACK이고, 형제의 왼쪽 자식이 RED인 경우
                sibling->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                sibling->left->color = RBTREE_BLACK;
                right_rotate(t, x->parent);
                x = t->root;
            }
        }
    }
    // x의 BLACK으로 변경
    x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
    node_t *y = p;
    color_t original_color = y->color;
    node_t *x;
    if (p->left == t->nil) {
        x = p->right;
        transplant(t, p, p->right);
    } else if (p->right == t->nil) {
        x = p->left;
        transplant(t, p, p->left);
    } else {
        y = p->right;
        while (y->left != t->nil) {
            y = y->left;
        }
        original_color = y->color;
        x = y->right;
        if (y->parent == p) {
            x->parent = y;
        } else {
            transplant(t, y, y->right);
            y->right = p->right;
            y->right->parent = y;
        }
        transplant(t, p, y);
        y->left = p->left;
        y->left->parent = y;
        y->color = p->color;
    }
    if (original_color == RBTREE_BLACK) {
        // delete_fixup(t, x);  // 삭제 후 RB 트리 속성을 복구하기 위한 함수 호출 (구현 필요)
    }
    free(p);  // 삭제된 노드의 메모리 해제
    return 0;
}
// int rbtree_erase(rbtree *t, node_t *p) {
//     node_t *y = p;
//     color_t original_color = y->color;
//     node_t *x;
//     if (p->left == t->nil) {
//         x = p->right;
//         transplant(t, p, p->right);
//     } else if (p->right == t->nil) {
//         x = p->left;
//         transplant(t, p, p->left);
//     } else {
//         *y = tree_minimun(t, p->right);
//         original_color = y->color;
//         x = y->right;
//         if (y->parent == p) {
//             x->parent = y;
//         } else {
//             transplant(t, y, y->right);
//             y->left = p->left;
//             y->left->parent = y;
//             y->color = p->color;
//         }
//     }
//     return 0;
// }

void inorderArray(rbtree *t, key_t **arr, node_t *node) {
    if (node != t->nil) {
        inorderArray(t, arr, node->left);
        **arr = node->key;
        (*arr)++;
        inorderArray(t, arr, node->right);
    }
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
    key_t *ptr = arr;
    inorderArray(t, &ptr, t->root);
    return 0;
}
