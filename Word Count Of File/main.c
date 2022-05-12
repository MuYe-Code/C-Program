#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

int main(void) {
	//打开文件
	FILE* fp = fopen("The_Holy_Bible_Res.txt", "r");
	//FILE* fp = fopen("test.txt", "r");
	if (fp == NULL) {
		perror("Error: ");
		exit(EXIT_FAILURE);
	}
	
	int ch, prech = ' ', wordCount = 0, lineCount = 1, characterCount = 0;
	while ((ch = fgetc(fp)) != EOF) {
		//遇到换行符则行数加一，没有换行符时行数为1
		if (ch == '\n') lineCount++;
		//如果是数字或者字母则字符数加一
		if (isalnum(ch)) characterCount++;
		//如果遇到字母且前一个字符是空格或者换行符，则单词数加一
		if (isspace(prech) && isalpha(ch)) wordCount++;
		prech = ch;
	}
	printf("字符个数：%d\n", characterCount);
	printf("行数：%d\n", lineCount);
	printf("单词个数：%d\n", wordCount);
	//关闭文件
	fclose(fp);
	return 0;
}
