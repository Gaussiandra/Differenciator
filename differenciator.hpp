#pragma once

#include "tree.hpp"

#define CL C(root->left)
#define CR C(root->right)
#define DL D(root->left)
#define DR D(root->right)

enum ErrorCodes {
    OKAY = 0,
    WRONG_ARGUMENTS_NUMBER,
    FILE_OPENING_ERROR,
    SIZE_GETTING_ERROR,
    MEM_ALLOCATION_ERROR,
    UNKNOWN_INPUT_TYPE,
};

#define RETURN_ON_ERROR(func) { ErrorCodes error = (func); if (error) return error; }

#define INIT_OPT_FUNC(func)                 \
    assert(rootPtr);                        \
    assert(*rootPtr);                       \
                                            \
    node *root = *rootPtr;                  \
                                            \
    bool returnValue = false;               \
    if (root->left) {                       \
        returnValue |= func(&root->left);   \
    }                                       \
    if (root->right) {                      \
        returnValue |= func(&root->right);  \
    }

void differenciateTree(node *root);
void optimizeTree(node **root);
bool optAdd(node **root);
bool optMul(node **root);
bool optPropConst(node **rootPtr);
bool optSub(node **root);
bool optChangeAddSub(node **rootPtr);
nodeValueType multConstAfterMul(node **rootPtr, bool setConstToOne);
node *D(node *root);
node *C(node *root);
ErrorCodes readDataFromPath(const char *path, char **string, size_t *szFile);