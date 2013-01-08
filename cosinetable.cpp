#include <math.h>
#include "stdio.h"

int main(){
int i;
int j;
for(i=0;i<360;i++)
{
	j++;
	double c = cos((double)i*3.14159/180)*128;
	printf("%.f,",c);
	if(j==8){printf("\n"); j=0;}
}
return 0;
}
