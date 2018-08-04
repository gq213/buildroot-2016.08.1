#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/input.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>


static int stop = 0;

static void sig_handler(int sig)
{
	(void)sig;

	printf("\nstop requested...\n");

	stop = 1;
}

int main(int argc, char *argv[])
{
	int ret = -1;
	int fd, value;
	struct input_event event;
	
	if (argc < 3 ) {
		printf("usage: %s beep_dev_path value\n", argv[0]);
		goto exit;
	}
 
	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		printf("Open %s Faild!\n", argv[1]);
		goto exit;
	}
	
	value = atoi(argv[2]);
	if (value < 0) {
		printf("invalid value %d\n", value);
		goto exit_fd;
	}
	
	ret = 0;
    
	event.type = EV_SND;
	event.code = SND_TONE;
	event.value = value;
	printf("%d\n", event.value);
	write(fd, &event, sizeof(struct input_event));
	
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	
	while (stop == 0) {
		sleep(10);
	}
	
	event.value = 0;
	printf("%d\n", event.value);
	write(fd, &event, sizeof(struct input_event));
	
exit_fd:
	printf("close...!\n");
	close(fd);
exit:
	return ret;
}