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
    node *root = NEW_NO();

    buildTreeFromStr(inputSequence, length, 1, root);
    saveGraphPic(root, "logs/original");
    
    differenciateTree(root);
    
    clearTree(root);
    free(inputSequence);
    inputSequence = nullptr;
    return 0;
}

void differenciateTree(node *root) {
    assert(root);

    //saveGraphPic(root, "diffTree1");

    if (root->left) {
        differenciateTree(root->left);
    }
    if (root -> right) {
        differenciateTree(root->right);
    }

    // rand + заметим, что...
    printf("(");
    printTree(root);
    printf(")' = ");

    node *diffTree = D(root);
    saveGraphPic(diffTree, "logs/diff");

    optimizeTree(&diffTree);
    saveGraphPic(diffTree, "logs/opt");
    printTree(diffTree);
    printf("\n");

    clearTree(diffTree);
}

void optimizeTree(node **root) {
    assert(root);
    assert(*root);

    size_t nOptimizations = 0;
    do {
        nOptimizations = 0;
        nOptimizations += optAdd(root);
        nOptimizations += optMul(root);
        nOptimizations += optSub(root);
        nOptimizations += optChangeAddSub(root);
    } while (nOptimizations);
}

bool optAdd(node **rootPtr) {
    INIT_OPT_FUNC(optAdd);

    if (root->operation == OP_ADD) {
        if (root->right->type == TYPE_CONST && root->left->type == TYPE_CONST) {
            *rootPtr = NEW_CONST(root->left->value + root->right->value);
            clearTree(root);
            return true;
        }
    }

    return returnValue;
}

bool optMul(node **rootPtr) {
    INIT_OPT_FUNC(optMul);

    if (root->operation == OP_MUL) {
        if (root->left->type == TYPE_CONST && root->right->type == TYPE_CONST) {
            *rootPtr = NEW_CONST(root->left->value * root->right->value);
            clearTree(root);
            return true;
        }
        if ((root->left->type  == TYPE_CONST && root->left->value  == 0) ||
            (root->right->type == TYPE_CONST && root->right->value == 0)) {
            *rootPtr = NEW_CONST(0);
            clearTree(root);
            return true;
        }
        if (root->left->type  == TYPE_CONST && root->left->value  == 1) {
            *rootPtr = C(root->right);
            clearTree(root);
            return true;
        }
        if (root->right->type  == TYPE_CONST && root->right->value  == 1) {
            *rootPtr = C(root->left);
            clearTree(root);
            return true;
        }
        return optPropConst(rootPtr);
    }
    
    return returnValue;
}

bool optPropConst(node **rootPtr) {
    assert(rootPtr);                        
    assert(*rootPtr);                       

    node *root = *rootPtr;

    if (root->operation == OP_MUL && 
        root->left->type != TYPE_CONST &&
        root->right->type != TYPE_CONST) {
        nodeValueType mult = multConstAfterMul(rootPtr, true);
        if (mult != 1) {
            *rootPtr = NEW_OP(OP_MUL, NEW_CONST(mult), C(root));
            clearTree(root);
            return true;
        }
    }

    return false;
}

nodeValueType multConstAfterMul(node **rootPtr, bool setConstToOne) {
    assert(rootPtr);                        
    assert(*rootPtr);                       
                                            
    node *root = *rootPtr;                  
                                            
    nodeValueType returnValue = 1;               
    if (root->left && root->left->operation == OP_MUL) {                       
        returnValue *= multConstAfterMul(&root->left, setConstToOne);
    }                                       
    if (root->right && root->right->operation == OP_MUL) {                      
        returnValue *= multConstAfterMul(&root->right, setConstToOne);  
    }

    if (root->operation == OP_MUL) {
        if (root->left->type == TYPE_CONST) {
            returnValue *= root->left->value; 
            if (setConstToOne) {
                (*rootPtr)->right->value = 1;
            }
        }
        if (root->right->type == TYPE_CONST) {
            returnValue *= root->right->value;
            if (setConstToOne) {
                (*rootPtr)->right->value = 1;
            }
        }
    }

    return returnValue;
}

bool optSub(node **rootPtr) {
    INIT_OPT_FUNC(optSub);

    if (root->operation == OP_SUB) {
        if (root->left->type == TYPE_CONST && root->right->type == TYPE_CONST) {
            *rootPtr = NEW_CONST(root->left->value - root->right->value);
            clearTree(root);
            return true;
        }

        if (root->right->operation == OP_MUL) {
            if (root->right->value < 0) {
                (*rootPtr)->right->value *= -1;
                (*rootPtr)->operation = OP_ADD;
                return true;
            }
            if (root->right->value < 0) {
                (*rootPtr)->right->value *= -1;
                (*rootPtr)->operation = OP_ADD;
                return true;
            }
        }
    }

    return returnValue;
}

bool optChangeAddSub(node **rootPtr) {
    INIT_OPT_FUNC(optChangeAddSub);

    Opertations newOp = OP_NO;
    if (root->operation == OP_ADD) {
        newOp = OP_SUB;
    }
    else if (root->operation == OP_SUB) {
        newOp = OP_ADD;
    }

    if (newOp != OP_NO && root->right->operation == OP_MUL) {
        if (root->right->left && root->right->left->value < 0) {
            (*rootPtr)->right->left->value *= -1;
            (*rootPtr)->operation = newOp;
            return true;
        }
        if (root->right->right && root->right->right->value < 0) {
            (*rootPtr)->right->right->value *= -1;
            (*rootPtr)->operation = newOp;
            return true;
        }
    }

    return returnValue;
}

node *D(node *root) {
    switch (root->type) {
        case TYPE_CONST:    return NEW_CONST(0);
        case TYPE_VARIABLE: return NEW_CONST(1);
        case TYPE_OPERATION: {
            switch (root->operation) { //^/tg, arcsin, arccos, arctg
                case OP_ADD: return NEW_OP(OP_ADD, DL, DR);
                case OP_SUB: return NEW_OP(OP_SUB, DL, DR);
                case OP_MUL: return NEW_OP(OP_ADD, NEW_OP(OP_MUL, DL, CR), NEW_OP(OP_MUL, CL, DR));
                // case OP_POW: return NEW_OP(OP_POW, NEW_OP(OP_MUL, CR, ), NEW_OP(OP_SUB, ))
                case OP_SIN: return NEW_OP(OP_MUL, NEW_FUNC(OP_COS, CL, nullptr), DL);
                /////!!!
                case OP_COS: return NEW_OP(OP_MUL, NEW_OP(OP_MUL, NEW_FUNC(OP_SIN, CL, nullptr), NEW_CONST(-1)), DL);
                case OP_NO:
                default:     return NEW_NO();
            }
        }
        case TYPE_NO:
        default: return NEW_NO();
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