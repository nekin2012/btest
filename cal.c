#include <stdlib.h>
#include "cal.h"

int heavy_cal(int var, int n_loop) {
	int i;

	for(i=0; i<n_loop; i++) { 
		/* try to fill the pipeline before look */
		var = 1103515245 * var + 12345;
		var = 0x4190001473/var;
		var = 1103515245 * var + 12345;
		var = 0x4190001473/var;
		var = 1103515245 * var + 12345;
		var = 0x4190001473/var;
		var = 1103515245 * var + 12345;
		var = 0x4190001473/var;
		var = 1103515245 * var + 12345;
		var = 0x4190001473/var;
	}

	return var;
}

int heavy_mem_read(int npage) {
	int i;
	int * data = malloc(PAGE_SIZE * npage);
	int sum = 0;

	for(i=0; i<npage*PAGE_SIZE/sizeof(int); i++) {
		sum += data[i];
	}

	free(data);

	return 0;
}

int heavy_mem_write(int npage) {
	int i;
	int * data = malloc(PAGE_SIZE * npage);

	for(i=0; i<npage*PAGE_SIZE/sizeof(int); i++) {
		data[i] = 1024;
	}

	free(data);

	return 0;
}
