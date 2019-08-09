#include <stdio.h>
#include <stdlib.h>

#include "src1.h"
#include "src2.h"

int foo1(void)
{
	printf("calling %s\n", __func__);
	foo2();
}

int main(int argc, char *argv[])
{
	return 0;
}
