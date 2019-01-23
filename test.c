#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(){
	char x[1] = "a";
	char y[1] = "b";
	x[0] = x[0] | y[0];
	printf("%c\n", x[0]);
}