#pragma once

#define STR_(x) #x
#define STR(x) STR_(x)

extern const size_t MAX_NODE_COUNT;
extern const size_t MAX_NODE_NAME_LEN;
extern const size_t MAX_OPERATION_LEN;
extern const size_t MAX_CMD_LEN;

extern const char FIRST_NODE_NAME[];
extern const char CONST_COLOR[];
extern const char VARIABLE_COLOR[];
extern const char OPERATION_COLOR[];
extern const char FUNCTION_COLOR[];

enum NodeType {
    TYPE_NO,
    TYPE_CONST,
    TYPE_VARIABLE,
    TYPE_OPERATION,
};

enum Opertations {
    OP_NO,

    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,

    OP_POW,
    OP_SQRT,
    OP_LN,

    OP_SIN,
    OP_COS,
    OP_TG,
    OP_CTG,

    OP_ARGSIN,
    OP_ARCCOS,
    OP_ARCTG,
    OP_ARCCTG,

    OP_SH,
    OP_CH,
    OP_TH,
    OP_CTH,
};

#define NEW_CONST(x)               createNode(TYPE_CONST, OP_NO, x, false, nullptr, nullptr)
#define NEW_OP(op, l, r)           createNode(TYPE_OPERATION, op, 0, false, l, r)
#define NEW_FUNC(op, l, r)         createNode(TYPE_OPERATION, op, 0, true, l, r)
#define NEW_NO()                   createNode(TYPE_NO, OP_NO, 0, false, nullptr, nullptr)
#define NEW_NO_WITH_PARENT(parent) createNode(TYPE_NO, OP_NO, 0, false, nullptr, nullptr, parent)

#define NODE_TYPE_SPEC %d
typedef int nodeValueType;
struct node {
    node *left   = nullptr, 
         *right  = nullptr, 
         *parent = nullptr;
    nodeValueType value = 0;
    bool isFunction = false;
    NodeType type = TYPE_NO;
    Opertations operation = OP_NO;
};

struct nodePathElem_t {
    bool isLeftChild = true;
    node *nodePtr = nullptr;
};

//!!
size_t buildTreeFromStr(char *inputSequence, size_t length, size_t i, node *root);
void setNodeTypeAndOp(node *curNode, char string[], size_t len); //!!!!
bool getNodePath(nodePathElem_t path[], const char name[], node *curNode, bool isLeft, size_t arrSize);
void dumpGraph(FILE *dotFile, node *curNode, size_t depth, const char prevName[] = FIRST_NODE_NAME);
void printConnectionInfo(FILE *dotFile, node *curNode, const char prevName[]);
void saveGraphPic(node *curNode, char savingPath[]);
void opToName(node *curNode, char opName[]);
void printTree(node *root);
void printNode(node *root);
node* createNode(NodeType type, Opertations op, nodeValueType value, bool isFunc,
                 node* lPtr, node* rPtr, node *parent = nullptr);
void setDataValue(node *curNode, char value[]);
void clearTree(node *curNode);