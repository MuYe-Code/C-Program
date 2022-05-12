#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

#define N 10
typedef struct student_s {
	int number;
	char name[26];
	bool sex;
	int chinese;
	int math;
	int english;
}Student;

//qsort函数的比较函数
int cmp(const void *a,const void *b);

int main(void) {
	//按只读方式打开文件
	FILE* fp = fopen("StudentInfo.txt", "r+");
	if (fp == NULL) {
		perror("Error: ");
		exit(EXIT_FAILURE);
	}
	//学生结构体数组
	Student stuArr[N];
	int i = 0;
	//读入结构体数据，最大不超过N个结构体
	for (; i < N && fscanf(fp, "%d%s%d%d%d%d", &stuArr[i].number, stuArr[i].name, &stuArr[i].sex, &stuArr[i].chinese, &stuArr[i].math, &stuArr[i].english) != EOF; i++);
	fclose(fp);
	//按学号排序
	qsort(stuArr, i, sizeof(Student), cmp);
	//按只写方式打开文件
	fp = fopen("StudentInfo.txt", "w");
	if (fp == NULL) {
		perror("Error: ");
		exit(EXIT_FAILURE);
	}
	//将结构体数组数据写入文件
	for (int j = 0; j < i; j++) {
		fprintf(fp, "%d\t%s\t\t%d\t%d\t%d\t%d\n", stuArr[j].number, stuArr[j].name, stuArr[j].sex, stuArr[j].chinese, stuArr[j].math, stuArr[j].english);
	}
	fclose(fp);
	return 0;
}

int cmp(const void* a, const void* b) {
	Student* stu1 = (Student*)a;
	Student* stu2 = (Student*)b;
	return stu1->number - stu2->number;
}
