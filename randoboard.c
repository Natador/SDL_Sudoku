#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
	int i, j;
	srand(time(NULL));
	for (i = 0; i < 9; i++) {
		for(j = 0; j < 9; j++)
			printf(" %d", rand() % 9 + 1);
		printf("\n");
	}
	return 0;
}
