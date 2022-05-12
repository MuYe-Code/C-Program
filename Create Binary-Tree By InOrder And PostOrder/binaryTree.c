#include"binaryTree.h"
#include <stdlib.h>
#include <stdio.h>

//前序遍历
void preOrder(TreeNode* root) {
	//边界条件
	if (root == NULL) return;
	//访问根节点
	printf("%c ", root->val);
	//访问左子树
	preOrder(root->left);
	//访问右子树
	preOrder(root->right);
}
void Tree_preOrder(Tree* T) {
	preOrder(T->root);
	puts("");
}

//后序遍历
void postOrder(TreeNode* root) {
	//边界条件
	if (root == NULL) return;
	//访问左子树
	postOrder(root->left);
	//访问右子树
	postOrder(root->right);
	//访问根节点
	printf("%c ", root->val);
}
void Tree_postOrder(Tree* T) {
	postOrder(T->root);
	puts("");
}

//用中序和后续构建一颗二叉树
TreeNode* createByInAndPost(E inArr[], E postArr[], int inStart, int inEnd, int rootPos) {
	E rootVal = postArr[rootPos];
	//创建当前结点
	TreeNode* newNode = (TreeNode*)calloc(1, sizeof(TreeNode));
	newNode->val = rootVal;
	//寻找根节点在inArr中的位置
	int rootIndex = inStart;
	for (; rootIndex <= inEnd; rootIndex++) {
		if (inArr[rootIndex] == rootVal) break;
	}
	//若还存在左子树
	if (rootIndex > inStart) {
		newNode->left = createByInAndPost(inArr, postArr, inStart, rootIndex - 1, rootPos - inEnd + rootIndex - 1);
	}
	//若还存在右子树
	if (rootIndex < inEnd) {
		newNode->right = createByInAndPost(inArr, postArr, rootIndex + 1, inEnd, rootPos - 1);
	}
	return newNode;
}
Tree* Tree_createByInAndPost(E inArr[], E postArr[], int inArrLen, int postArrLen) {
	Tree* ret=(Tree*)calloc(1, sizeof(Tree));
	ret->root = createByInAndPost(inArr, postArr,0, inArrLen - 1, postArrLen - 1);
	return ret;
}

void destroyTree(TreeNode* root) {
	//边界条件
	if (root == NULL) return;
	//依据后序遍历删除每个结点
	destroyTree(root->left);
	destroyTree(root->right);
	free(root);
}
void Tree_destroyTree(Tree* T) {
	//删除树的每个结点
	destroyTree(T->root);
	//删除树结构体
	free(T);
	//将T重定向到NULL，避免悬空指针
	T = NULL;
}
