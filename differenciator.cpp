#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include "differenciator.hpp"
#include "tree.hpp"

//все операции+маклорен, очистка памяти, упрощения, дсл, чистый код...

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
    saveGraphPic(D(root));
    
    differenciate(root);
    
    clearTree(root);
    free(inputSequence);
    inputSequence = nullptr;
    return 0;
}

void differenciate(node *root) {

}

node *D(node *root) {
    switch (root->type) {
        case TYPE_CONST:    return createNode(TYPE_CONST, OP_NO, 0, nullptr, nullptr);
        case TYPE_VARIABLE: return createNode(TYPE_CONST, OP_NO, 1, nullptr, nullptr);
        case TYPE_OPERATION: {
            switch (root->operation) {
                case OP_ADD: return createNode(TYPE_OPERATION, OP_ADD, 0, D(root->left), D(root->right));
                case OP_SUB: return createNode(TYPE_OPERATION, OP_SUB, 0, D(root->left), D(root->right));
                case OP_SIN: return createNode(TYPE_OPERATION, OP_COS, 0, C(root->left), nullptr); //C
                case OP_COS: return createNode(TYPE_OPERATION, OP_SUB, 0, createNode(TYPE_OPERATION, OP_SIN, 0, C(root->left), nullptr), nullptr);
            }
        }
    }

    return nullptr;
}

node *C(node *root) {
    if (root->left) {
        return C(root->left);
    }
    if (root->right) {
        return C(root->right);
    }

    node *newNode = (node*) calloc(1, sizeof(node));
    memcpy(newNode, root, sizeof(node));

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