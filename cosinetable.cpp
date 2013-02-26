#include <math.h>
#include "stdio.h"

int main(){
int i,j,k;
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

for(i=0;i<4;i++){
	for(j=0;j<4;j++){
		for(k=0;k<4;k++){
			printf("%d,%d,%d,",(-400)+(i*200),(100)-(j*200),(1300)-(k*400));
		}
		printf("\n");
	}
}
return 0;
}
