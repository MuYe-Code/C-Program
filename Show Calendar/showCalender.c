#include "showCalender.h"
//返回1代表是闰年，返回0代表是平年
int isLeapYear(int year) {
	if (year % 100 != 0 && year % 4 == 0 || year % 400 == 0)
		return 1;
	else return 0;
}

int isLongerMonth(int month) {
	if (month == 4 || month == 6 || month == 9 || month == 11) {
		return 0;
	}
    if (month == 2) return -2;
	else return 1;
}

int nextDay(int year, int month, int day) {
	//先处理2月
	if (month == 2) {
		int temp = 28 + isLeapYear(year);
		if (day > temp) return 0;
		if (day = temp) { day = 1;	month = 3; }
		//非特殊情况直接day+1
		else day++;
	}
	//再处理其他月份
	else {
		int temp = 30 + isLongerMonth(month);
		if (day > temp) return 0;

		else if (day == temp) {
			day = 1;
			//检查是否是12月，因为12月31日是特殊情况
			if (month == 12) { month = 1;	year++; }
			else month++;
		}
		//非特殊情况直接day+1
		else day++;
	}
	
	printf("The next day is %d/%.2d/%.2d\n", year, month, day);
	return 1;
}

int numOfDaysBetweenTwoDays(int year1, int month1, int day1, int year2, int month2, int day2) {
	int ans = 0;
	//保证year2不小于year1
	if (year1 > year2) ans = numOfDaysBetweenTwoDays(year2, month2, day2, year1, month1, day1);
	else {
		//year1剩余天数
		int nums1 = 365 + isLeapYear(year1) - numOfDaysFromBeginning(year1, month1, day1);
		//year2已经度过的天数
		int nums2 = numOfDaysFromBeginning(year2, month2, day2);
		ans = nums1 + nums2;
		//同一年的情况下
		if (year1 == year2) ans = abs(ans - 365 + isLeapYear(year1));
		//其余情况
		else {
			for (year1++; year1 < year2; year1++) {
				ans += (365 + isLeapYear(year1));
			}
		}
	}
	return ans;
}

int whatDay(int year, int month, int day) {
	//得到目标日期距1997/01/01的天数，将其模7运算
	int num = numOfDaysBetweenTwoDays(1970, 1, 1, year, month, day) % 7;
	if(year>=1970)
		//因为模7运算的结果为0到6，因此需要加1
		return 1 + (3 + num) % 7;
	else {
		//1970/01/01之前的日期
		if (num < 4) return 4 - num;
		else return 11 - num;
	}
}

void showCalendar(int year) {
	printf("|=======================The Calendar of Year %.4d=======================|\n\n", year);
	
	//每次打印两个月份，一共循环6轮
	for (int i = 1; i < 7;i++) {
		//dayOfWeek1表明当前月份1号是星期几
		int dayOfWeek1 = whatDay(year, i, 1);
		int dayOfWeek2 = whatDay(year, i + 6, 1);

		//startday1是当前月份的日期，当startday1<=0时代表当前位置是空位
		int startday1 = 1 - dayOfWeek1 % 7, startday2 = 1 - dayOfWeek2 % 7;
		
		//dayOfMonth1是当前月份的天数
		int dayOfMonth1 = 30 + isLongerMonth(i);
		//考虑闰年的2月
		if (i == 2) dayOfMonth1 += isLeapYear(year);
		int dayOfMonth2 = 30 + isLongerMonth(i+6);

		//numOfLine1是打印当前月份需要几行
		int numOfLine1 = (dayOfMonth1 - startday1) / 7 + 1;
		int numOfLine2 = (dayOfMonth2 - startday2) / 7 + 1;
		int numOfLine = max(numOfLine1, numOfLine2);
		
		printf(":  %2d  SUN MON TUE WED THU FRI SAT     %2d  SUN MON TUE WED THU FRI SAT  :\n", i, i + 6);
		
		//每次打印一行
		for (int j = 0; j < numOfLine;j++) {
			//注意匹配空格数量
			printf(":     ");

			//先打印左边，一排打印7个
			for (dayOfWeek1 = 0; dayOfWeek1 != 7; startday1++) {
				//当前日期有效则打印日期
				if (startday1 > 0 && startday1 <= dayOfMonth1) printf("%4d", startday1);
				//否则打印空格
				else printf("    ");
				dayOfWeek1++;
 			}
			
			printf("\t  ");
			//打印右边，一排打印7个
			for (dayOfWeek2 = 0; dayOfWeek2 != 7; startday2++) {
				if (startday2 > 0 && startday2 <= dayOfMonth2) printf("%4d", startday2);
				else printf("    ");
				dayOfWeek2++;
			}
			printf("  :\n");
		}
		puts("");
	}

	printf("|=======================================================================|\n");
}
