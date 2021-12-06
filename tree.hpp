#pragma once

#define STR_(x) #x
#define STR(x) STR_(x)

extern const size_t MAX_NODE_COUNT;
extern const size_t MAX_NODE_NAME_LEN;
extern const size_t MAX_OPERATION_LEN;
extern const size_t MAX_CMD_LEN;
extern const char FIRST_NODE_NAME[];
extern const char dotSavingPath[], picSavingPath[];

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
    OP_COS,
    OP_SIN,
};

#define NODE_TYPE_SPEC %zu
typedef size_t nodeValueType;
struct node {
    node *left   = nullptr, 
         *right  = nullptr, 
         *parent = nullptr;
    nodeValueType value = 0;
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
void saveGraphPic(node *curNode);
node* createNode(NodeType type, Opertations op, size_t value, node* lPtr, node* rPtr, node *parent = nullptr);
void setDataValue(node *curNode, char value[]);
void clearTree(node *curNode);