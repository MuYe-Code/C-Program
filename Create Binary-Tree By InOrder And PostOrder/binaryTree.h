#include <stdlib.h>
#define E char
typedef struct treeNode_s {
	E val;
	struct treeNode_s* left;
	struct treeNode_s* right;
}TreeNode;

typedef struct tree_s {
	TreeNode* root;
}Tree;

void Tree_preOrder(Tree* T);
void Tree_postOrder(Tree* T);
Tree* Tree_createByInAndPost(E inArr[], E postArr[], int len1, int len2);
void Tree_destroyTree(Tree* T);
