//#include <stdio.h>

int main(int argc, char* argv[]) {
	char i = 5;
	int result = 1;

	while(i>1) {
		result *= i;
		i--;
	}

	//printf("%d\n", result);

	return result;
}
