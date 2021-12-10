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

void differenciate(node *root);
node *D(node *root);
node *C(node *root);
ErrorCodes readDataFromPath(const char *path, char **string, size_t *szFile);