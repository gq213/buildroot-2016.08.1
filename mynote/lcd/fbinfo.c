#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
	int ret = -1;
	
	if (argc < 2) {
		printf("usage: %s fb_dev_path\n", argv[0]);
		goto exit;
	}
	
	char *fb_path = argv[1];
	int fd = open(fb_path, O_RDWR);
	if (fd < 0) {
		printf("open %s failed\n", fb_path);
		goto exit;
	}
	
	struct fb_var_screeninfo var;
	ret = ioctl(fd, FBIOGET_VSCREENINFO, &var);
	if(ret < 0)
	{
		printf("ioctl %s FBIOGET_VSCREENINFO failed\n", fb_path);
		goto exit_fd;
	}
	
	printf("width:%d, height:%d, bit:%d\n", var.xres, var.yres, var.bits_per_pixel);
	
exit_fd:
	close(fd);
exit:
	return ret;
}