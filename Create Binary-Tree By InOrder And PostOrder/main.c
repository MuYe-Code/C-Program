#include"binaryTree.h"

#define SIZE(a) (sizeof(a)/sizeof(a[0]))
int main(void) {
	char inArr[] = {'D','B','E','A','C','F'}, postArr[] = {'D','E','B','F','C','A'};
	int len = SIZE(inArr);
	puts("输入：");
	printf("中序数组：");
	for (int i = 0; i < len; i++) {
		printf("%c ", inArr[i]);
	}

	printf("\n后序数组：");
	for (int i = 0; i < len; i++) {
		printf("%c ", postArr[i]);
	}

	puts("\n\n根据后序数组和中序数组建立二叉树");
	Tree* tree = Tree_createByInAndPost(inArr, postArr, SIZE(inArr), SIZE(postArr));
	puts("二叉树建立完成\n");

	puts("输出：");
	printf("后序数组：");
	Tree_postOrder(tree);
	printf("前序数组：");
	Tree_preOrder(tree);
	Tree_destroyTree(tree);
	return 0;
}
