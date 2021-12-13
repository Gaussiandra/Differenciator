#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
//#include "differenciator.hpp"
#include "tree.hpp"

const size_t MAX_CMD_LEN = 250;
const size_t MAX_NODE_COUNT = 250;
const size_t MAX_NODE_NAME_LEN = 100;
const size_t MAX_OPERATION_LEN = 20;

const char FIRST_NODE_NAME[] = "Z";
const char CONST_COLOR[]     = "1F85DE";
const char VARIABLE_COLOR[]  = "65DE1F";
const char OPERATION_COLOR[] = "DE781F";
const char FUNCTION_COLOR[]  = "EA8DF7";
const char NO_COLOR[]        = "FFFFFF";

size_t buildTreeFromStr(char *inputSequence, size_t length, size_t i, node *curNode) {
    assert(inputSequence);

    bool leftLeafIsCreated = false;
    while (i < length) {
        if (inputSequence[i] == '(' || inputSequence[i] == ')') {
            if (inputSequence[i] == '(') {
                if (leftLeafIsCreated) {
                    curNode->right = NEW_NO_WITH_PARENT(curNode);
                    i = buildTreeFromStr(inputSequence, length, i+1, curNode->right);
                }
                else {
                    curNode->left = NEW_NO_WITH_PARENT(curNode);
                    i = buildTreeFromStr(inputSequence, length, i+1, curNode->left);
                    leftLeafIsCreated = true;
                }
            }
            else if (inputSequence[i] == ')') {
                return i;
            }
        }
        else if (curNode->type == TYPE_NO && curNode->operation == OP_NO) {
            char *nameStart = inputSequence + i;
            size_t end1 = size_t(strchr(nameStart, ')') - nameStart),
                   end2 = size_t(strchr(nameStart, '(') - nameStart),
                   nameLen = end1 < end2 ? end1 : end2;
            setNodeTypeAndOp(curNode, nameStart, nameLen);
            //RETURN_ON_ERROR(setNodeTypeAndOp(curNode, nameStart, nameLen)); //!!!!
        }
        ++i;
    }
    return i;
}

// define?
void setNodeTypeAndOp(node *curNode, char string[], size_t len) {
    assert(curNode);
    assert(string);

    if (!strncmp(string, "+", len)) { 
        curNode->type = TYPE_OPERATION; 
        curNode->operation = OP_ADD;
        curNode->isFunction = false;
    }
    else if (!strncmp(string, "-", len)) { 
        curNode->type = TYPE_OPERATION; 
        curNode->operation = OP_SUB;
        curNode->isFunction = false;
    }
    else if (!strncmp(string, "*", len)) { 
        curNode->type = TYPE_OPERATION; 
        curNode->operation = OP_MUL;
        curNode->isFunction = false;
    }
    else if (!strncmp(string, "/", len)) { 
        curNode->type = TYPE_OPERATION; 
        curNode->operation = OP_DIV;
        curNode->isFunction = false;
    }
    else if (!strncmp(string, "sin", len)) { 
        curNode->type = TYPE_OPERATION; 
        curNode->operation = OP_SIN;
        curNode->isFunction = true;
    }
    else if (!strncmp(string, "cos", len)) { 
        curNode->type = TYPE_OPERATION; 
        curNode->operation = OP_COS;
        curNode->isFunction = true;
    }
    else if (!strncmp(string, "x", len)) { 
        curNode->type = TYPE_VARIABLE; 
        curNode->operation = OP_NO;
    }
    // как не считать 2sin как 2?...
    else if (sscanf(string, STR(NODE_TYPE_SPEC), &curNode->value) == 1) {
        curNode->type = TYPE_CONST;
        curNode->operation = OP_NO;
    }
    else {
        printf("Unknown input: %.*s", (int) len, string);
        return;//UNKNOWN_INPUT_TYPE;
    }
    return;// OKAY;
}

void opToName(node *curNode, char opName[]) {
    assert(curNode);
    assert(opName);

    if (curNode->operation == OP_ADD) {
        strcpy(opName, "+");
    }
    else if (curNode->operation == OP_SUB) {
        strcpy(opName, "-");
    }
    else if (curNode->operation == OP_MUL) {
        strcpy(opName, "*");
    }
    else if (curNode->operation == OP_DIV) {
        strcpy(opName, "/");
    }
    else if (curNode->operation == OP_COS) {
        strcpy(opName, "cos");
    }
    else if (curNode->operation == OP_SIN) {
        strcpy(opName, "sin");
    }
    else {
        strcpy(opName, "unknown");
    }
}

void dumpGraph(FILE *dotFile, node *curNode, size_t depth, const char prevName[]) {
    assert(dotFile);
    assert(curNode);
    assert(prevName);

    if (depth == 1) {
        fprintf(dotFile, "%s", "digraph G {\n");
    }

    printConnectionInfo(dotFile, curNode, prevName);

    char newName[MAX_NODE_NAME_LEN] = {0};
    strcpy(newName, prevName);
    size_t newNameLen = strlen(newName);
    if (curNode->left) {
        newName[newNameLen] = 'L';
        fprintf(dotFile, "%s -> %s;\n", prevName, newName);
        dumpGraph(dotFile, curNode->left, depth + 1, newName);
    }
    if (curNode->right) {
        newName[newNameLen] = 'R';
        fprintf(dotFile, "%s -> %s;\n", prevName, newName);
        dumpGraph(dotFile, curNode->right, depth + 1, newName);
    }

    if (depth == 1) {
        fprintf(dotFile, "%s", "}");
    } 
}

void printConnectionInfo(FILE *dotFile, node *curNode, const char prevName[]) {
    assert(dotFile);
    assert(curNode);
    assert(prevName);

    switch (curNode->type) {
        case TYPE_CONST: {
            fprintf(dotFile, 
                    "%s [shape=rect, label=\""STR(NODE_TYPE_SPEC)"\"\
                    style=bold, color=\"#%s\"];\n", 
                    prevName, 
                    curNode->value,
                    CONST_COLOR);
            break;
        }
        case TYPE_OPERATION: {
            char opName[MAX_OPERATION_LEN] = "";
            opToName(curNode, opName);

            const char *color = FUNCTION_COLOR;
            if (!curNode->isFunction) {
                color = OPERATION_COLOR;
            }

            fprintf(dotFile, 
                    "%s [shape=rect, label=\"%s\"style=bold, color=\"#%s\"];\n", 
                    prevName, 
                    opName,
                    color);
            break;
        }    
        case TYPE_VARIABLE: {
            fprintf(dotFile,
                    "%s [shape=rect, label=\"x\"style=bold, color=\"#%s\"];\n", 
                    prevName,
                    VARIABLE_COLOR);
            break;
        }
        case TYPE_NO:
        default: {
            fprintf(dotFile, "%s [shape=rect, label=\"\"style=bold, color=\"#%s\"];\n", 
                    prevName,
                    NO_COLOR);
            break;
        }
    }
}

void printTree(node *root) {
    assert(root);

    if (root->isFunction) {
        printNode(root);
        printf("(");
        if (root->left) {
            printTree(root->left);
        }
        printf(")");
    }
    else {
        if (root->left) {
            printTree(root->left);
        }
        printNode(root);
        if (root->right) {
            printTree(root->right);
        }
    }
}

void printNode(node *root) {
    assert(root);

    switch (root->type) {
        case TYPE_NO: {
            printf("no_type");
            break;
        }
        case TYPE_CONST: {
            if (root->value < 0) {
                printf("("STR(NODE_TYPE_SPEC)")", root->value);
            }
            else {
                printf(STR(NODE_TYPE_SPEC), root->value);
            }
            break;
        }
        case TYPE_VARIABLE: {
            printf("x"); 
            break;
        }
        case TYPE_OPERATION: {
            char name[MAX_NODE_NAME_LEN] = "";
            opToName(root, name);
            printf("%s", name);
            break;
        }
        default: {
            printf("unknown"); 
            break;
        }
    }
}

void saveGraphPic(node *curNode, char savingName[]) {
    assert(curNode);
    assert(savingName);

    char savingPath[MAX_CMD_LEN] = {0};
    sprintf(savingPath, "%s.dot", savingName);
    FILE *dotFile = fopen(savingPath, "w");
    dumpGraph(dotFile, curNode, 1);
    fclose(dotFile);

    char dumpCmd[MAX_CMD_LEN] = {0};
    sprintf(dumpCmd, "dot -Tpng %s.dot -o %s.png", savingName, savingName);
    system(dumpCmd);
}

node* createNode(NodeType type, Opertations op, nodeValueType value, bool isFunc,
                 node* lPtr, node* rPtr, node *parent) {
    node *newNode = (node*) calloc(1, sizeof(node));
    newNode->parent = parent;
    newNode->type = type;
    newNode->operation = op;
    newNode->value = value;
    newNode->isFunction = isFunc;
    newNode->left = lPtr;
    newNode->right = rPtr;

    return newNode;
}

void clearTree(node *curNode) {
    if (curNode->left) {
        clearTree(curNode->left);
        curNode->left = nullptr;
    }
    if (curNode->right) {
        clearTree(curNode->right);
        curNode->right = nullptr;
    }

    free(curNode);
    curNode = nullptr;
}