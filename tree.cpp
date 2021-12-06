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

const char dotSavingPath[] = "dump.dot",
           picSavingPath[] = "dump.png";

size_t buildTreeFromStr(char *inputSequence, size_t length, size_t i, node *curNode) {
    assert(inputSequence);

    //size_t nameStartIdx = i;
    bool leftLeafIsCreated = false;
    while (i < length) {
        if (inputSequence[i] == '(' || inputSequence[i] == ')') {
            if (inputSequence[i] == '(') {
                if (leftLeafIsCreated) {
                    curNode->right = createNode(TYPE_NO, OP_NO, 0, nullptr, nullptr, curNode);
                    i = buildTreeFromStr(inputSequence, length, i+1, curNode->right);
                }
                else {
                    curNode->left = createNode(TYPE_NO, OP_NO, 0, nullptr, nullptr, curNode);
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
    if (!strncmp(string, "+", len)) { 
        curNode->type = TYPE_OPERATION; 
        curNode->operation = OP_ADD;
    }
    else if (!strncmp(string, "-", len)) { 
        curNode->type = TYPE_OPERATION; 
        curNode->operation = OP_SUB;
    }
    else if (!strncmp(string, "*", len)) { 
        curNode->type = TYPE_OPERATION; 
        curNode->operation = OP_MUL;
    }
    else if (!strncmp(string, "/", len)) { 
        curNode->type = TYPE_OPERATION; 
        curNode->operation = OP_DIV;
    }
    else if (!strncmp(string, "sin", len)) { 
        curNode->type = TYPE_OPERATION; 
        curNode->operation = OP_SIN;
    }
    else if (!strncmp(string, "cos", len)) { 
        curNode->type = TYPE_OPERATION; 
        curNode->operation = OP_COS;
    }
    else if (!strncmp(string, "x", len)) { 
        curNode->type = TYPE_VARIABLE; 
        curNode->operation = OP_NO;
    }
    // как не считать 2sin как 2?...
    else if (sscanf(string, "%zu", &curNode->value) == 1) {
        curNode->type = TYPE_CONST;
        curNode->operation = OP_NO;
    }
    else {
        printf("Unknown input: %.*s", len, string);
        return;//UNKNOWN_INPUT_TYPE;
    }
    return;// OKAY;
}

void typeToName(node *curNode, char opName[]) {
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

    if (curNode->type == TYPE_CONST) {
        fprintf(dotFile, 
                "%s [shape=rect, label=\""STR(NODE_TYPE_SPEC)"\" style=bold, color=\"#1F85DE\"];\n", 
                prevName, 
                curNode->value);
    }
    else if (curNode->type == TYPE_OPERATION) {
        char opName[MAX_OPERATION_LEN] = "";
        typeToName(curNode, opName);
        fprintf(dotFile, "%s [shape=rect, label=\"%s\" style=bold, color=\"#DE781F\"];\n", prevName, opName);
    }    
    else if (curNode->type == TYPE_VARIABLE) {
        fprintf(dotFile, "%s [shape=rect, label=\"x\" style=bold, color=\"#65DE1F\"];\n", prevName);
    }
    else {
        fprintf(dotFile, "%s [shape=rect, label=\"\" style=bold, color=\"#FFFFFF\"];\n", prevName);

    }

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

void saveGraphPic(node *curNode) {
    assert(curNode);

    FILE *dotFile = fopen(dotSavingPath, "w");
    dumpGraph(dotFile, curNode, 1);
    fclose(dotFile);

    char dumpCmd[MAX_CMD_LEN] = {0};
    sprintf(dumpCmd, "dot -Tpng %s -o %s", dotSavingPath, picSavingPath);
    system(dumpCmd);
}

node* createNode(NodeType type, Opertations op, size_t value, node* lPtr, node* rPtr, node *parent) {
    node *newNode = (node*) calloc(1, sizeof(node));
    newNode->parent = parent;
    newNode->type = type;
    newNode->operation = op;
    newNode->value = value;
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