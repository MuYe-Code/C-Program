#include "showCalender.h"

int main(void){
  int n;
  puts("Please input the year whose calendar you want to know.");
  scanf("%d",&n);
  showCalendar(n);
  return 0;
}
