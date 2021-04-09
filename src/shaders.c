#include <stdio.h>
#include <stdlib.h>

const size_t buffer_size = 8 * 1024;

char *load_shader(char *filename)
{
	FILE *stream = fopen(filename, "r");

	if (stream == NULL) {
		return NULL;
	}

	char *contents = (char *)malloc(buffer_size);
	size_t bytes_read = fread(contents, sizeof(char), buffer_size, stream);

	if (!feof(stream)) {
		return NULL;
	}

	fclose(stream);

	return contents;
}
