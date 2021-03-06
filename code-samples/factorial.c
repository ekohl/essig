#include <stdio.h>
#include <avr/io.h>

#ifdef WITH_PRINTF
static int factorial_putchar(char c, FILE *stream);

static FILE _destfile = FDEV_SETUP_STREAM(factorial_putchar, NULL,
		                                  _FDEV_SETUP_WRITE);

FILE *destfile = &_destfile;

static int
factorial_putchar(char c, FILE *stream)
{
	PORTA = c;
	return 0;
}
#endif /* WITH_PRINTF */

int main(int argc, char* argv[]) {
	char i = 12, *resultp;
	long result = 1;

	while (i > 1) {
		result *= i;
		i--;
	}

#ifdef WITH_PRINTF
	char buf[sizeof(result) + 2];
	fprintf(destfile, "%ld\n", result);
#else
	resultp = (char *) &result;
	for (i = 0; i < sizeof(result); i++) {
		PORTA = resultp[sizeof(result) - i - 1];
	}
#endif
	return 0;
}
