#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include "differenciator.hpp"
#include "tree.hpp"

//все операции+маклорен, упрощения, дсл, чистый код...

int main(const int argc, const char *argv[]) {
    if (argc != 2) {
        printf("Wrong number of arguments.\n");
        return WRONG_ARGUMENTS_NUMBER;
    }

    char *inputSequence = nullptr;
    size_t length = 0;
    RETURN_ON_ERROR(readDataFromPath(argv[1], &inputSequence, &length)); 
    node *root = createNode(TYPE_NO, OP_NO, 0, nullptr, nullptr);

    buildTreeFromStr(inputSequence, length, 1, root);
    saveGraphPic(root, "original");
    node *diffTree = D(root);
    saveGraphPic(diffTree, "diff");
    
    printTree(root->left);
    differenciate(root);
    
    clearTree(root);
    clearTree(diffTree);
    free(inputSequence);
    inputSequence = nullptr;
    return 0;
}

void differenciate(node *root) {

}

node *D(node *root) {
    switch (root->type) {
        case TYPE_CONST:    return NEW_CONST(0);
        case TYPE_VARIABLE: return NEW_CONST(1);
        case TYPE_OPERATION: {
            switch (root->operation) {
                case OP_ADD: return NEW_OP(OP_ADD, DL, DR);
                case OP_SUB: return NEW_OP(OP_SUB, DL, DR);
                case OP_MUL: return NEW_OP(OP_ADD, NEW_OP(OP_MUL, DL, CR), NEW_OP(OP_MUL, CL, DR));
                default: return NEW_NO;
            }
        }
        case TYPE_FUNCTION: {
            switch (root->operation)
            {
                case OP_SIN: return NEW_OP(OP_MUL, NEW_FUNC(OP_COS, CL, nullptr), DL);
                case OP_COS: return NEW_OP(OP_MUL, NEW_OP(OP_SUB, NEW_FUNC(OP_SIN, CL, nullptr), nullptr), DL);
                default: return NEW_NO;
            }
        }
        default: return NEW_NO;
    }

    return nullptr;
}

node *C(node *root) {
    node *left = nullptr, *right = nullptr;
    if (root->left) {
        left = C(root->left);
    }
    if (root->right) {
        right = C(root->right);
    }

    node *newNode = (node*) calloc(1, sizeof(node));
    memcpy(newNode, root, sizeof(node));
    newNode->left = left;
    newNode->right = right;

    return newNode;
}

ErrorCodes readDataFromPath(const char *path, char **string, size_t *szFile) {
    assert(path);
    assert(string);

    FILE *file = fopen(path, "r");
    if (!file) {
        printf("File wasn't opened. Provided path: %s\n", path);
        return FILE_OPENING_ERROR;
    }
    
    fseek(file, 0, SEEK_END);
    long signedSzFile = ftell(file);
    if (signedSzFile == -1) {
        return SIZE_GETTING_ERROR;
    }
    *szFile = (size_t) signedSzFile;
    rewind(file);

    *string = (char*)calloc(*szFile + 1, sizeof(char));
    if (!*string) {
        printf("Memory wasn't allocated.\n");
        return MEM_ALLOCATION_ERROR;
    }

    // fread on Windows returns readingResult <= szFile due to deleting /r from stream
    size_t readingResult = fread(*string, sizeof(char), *szFile, file);
    *szFile = readingResult;
    (*string)[readingResult] = '\0';

    fclose(file);
    return OKAY;
}