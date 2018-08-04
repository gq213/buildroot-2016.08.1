#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
	int ret = -1;
	
	if (argc < 5) {
		printf("usage: %s width height bit color_hex\n", argv[0]);
		goto exit;
	}
	
	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	int bit = atoi(argv[3]);
	int color_hex = strtol(argv[4], NULL, 0);
	printf("width:%d, height:%d, bit:%d, color_hex:0x%08x\n", width, height, bit, color_hex);
	
	char img_name[64];
	sprintf(img_name, "%d_%d_%d_%08x.bin", width, height, bit, color_hex);
	
	FILE *fp = fopen(img_name, "w+b");
	if (fp == NULL) {
		printf("open %s failed\n", img_name);
		goto exit;
	}
	
	char data;
	for (int i=0; i<width; i++) {
		for (int j=0; j<height; j++) {
			for (int k=0; k<(bit/8); k++) {
				data = (color_hex >> (((bit/8) - 1 - k) * 8)) & 0xff;
				fwrite(&data, sizeof(data), 1, fp);
			}
		}
	}
	
	fclose(fp);
	
exit:
	return ret;
}