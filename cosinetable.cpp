#include <math.h>
#include "stdio.h"

int main(){
int i;
int j;
j=0;
for(i=0;i<360;i++)
{
	j++;
	double c = cos((double)i*3.14159/180)*8;
	printf("%.f,",c);
	if(j==8){printf("\n"); j=0;}
}
j=0;
printf("\n\n");
for(i=0;i<360;i++)
{
	j++;
	double c = sin((double)i*3.14159/180)*8;
	printf("%.f,",c);
	if(j==8){printf("\n"); j=0;}
}
return 0;
}
