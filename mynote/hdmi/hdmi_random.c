#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <linux/v4l2-dv-timings.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define ERRSTR strerror(errno)

#define BYE_ON(cond, ...) \
do { \
	if (cond) { \
		int errsv = errno; \
		fprintf(stderr, "ERROR(%s:%d) : ", \
			__FILE__, __LINE__); \
		errno = errsv; \
		fprintf(stderr,  __VA_ARGS__); \
		abort(); \
	} \
} while(0)

#define BUFFER_CNT	3

struct buffer {
	int index;
	void *data;
	size_t size;
	size_t width;
	size_t height;

	/* buffer state */
	double t;
};

struct context {
	int fd;
	struct buffer *buffer;
	size_t buffer_cnt;
};

const struct {
	const int index;
	const size_t width;
	const size_t height;
	const struct v4l2_dv_timings dv_timings;
} output_formats[] = {
	{ 0, 720,  480,  V4L2_DV_BT_CEA_720X480P59_94 },
	{ 1, 1280, 720,  V4L2_DV_BT_CEA_1280X720P60   },
	{ 2, 1920, 1080, V4L2_DV_BT_CEA_1920X1080P30  },
	{ 3, 1920, 1080, V4L2_DV_BT_CEA_1920X1080I60  },
	{ 4, 1920, 1080, V4L2_DV_BT_CEA_1920X1080P60  },
};

int showtime(struct context *ctx);

int main(int argc, char *argv[])
{
	int ret;
	int fmt_index;

	BYE_ON(argc < 3, 
			"usage: %s device[/dev/videoX] format[0-4]\n", argv[0]);
	
	fmt_index = atoi(argv[2]);
	BYE_ON(((fmt_index < 0) || (fmt_index > 4)), 
			"usage: %s device[/dev/videoX] format[0-4] your format: %d\n", argv[0], fmt_index);

	int fd;
	fd = open(argv[1], O_RDWR);
	BYE_ON(fd < 0, "open failed: %s\n", ERRSTR);
	
	{
	struct v4l2_capability cap;
	ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
	BYE_ON(ret != 0, "VIDIOC_QUERYCAP failed: %s\n", ERRSTR);
	printf("Driver Name:%s\n"
			"Card Name:%s\n"
			"Bus info:%s\n"
			"Driver Version:%u.%u.%u\n"
			"capabilities:0x%08x\n",
			cap.driver,
			cap.card,
			cap.bus_info,
			(cap.version >> 16) & 0xff, (cap.version >> 8) & 0xff, cap.version & 0xff,
			cap.capabilities);
	/* 
	Driver Name:s5p-mixer
	Card Name:graph0
	Bus info:0
	Driver Version:3.10.105
	capabilities:0x84002000
	 */
	//#define V4L2_CAP_DEVICE_CAPS            0x80000000  /* sets device capabilities field */
	//#define V4L2_CAP_STREAMING              0x04000000  /* streaming I/O ioctls */
	/* Is a video output device that supports multiplanar formats */
	//#define V4L2_CAP_VIDEO_OUTPUT_MPLANE	0x00002000
	}
	
	{
	struct v4l2_dv_timings_cap dv_timings_cap;
	ret = ioctl(fd, VIDIOC_DV_TIMINGS_CAP, &dv_timings_cap);
	BYE_ON(ret != 0, "VIDIOC_DV_TIMINGS_CAP failed: %s\n", ERRSTR);
	printf("%d-%d, %d-%d, %lld-%lld\n",
			dv_timings_cap.bt.min_width, dv_timings_cap.bt.max_width,
			dv_timings_cap.bt.min_height, dv_timings_cap.bt.max_height,
			dv_timings_cap.bt.min_pixelclock, dv_timings_cap.bt.max_pixelclock);
	/* 
	720-1920, 480-1080, 27000000-148500000
	 */
	}
	
	{
	struct v4l2_enum_dv_timings enum_dv_timings;
	enum_dv_timings.index = 0;
	printf("Support enum_dv_timings:\n");
	while (ioctl(fd, VIDIOC_ENUM_DV_TIMINGS, &enum_dv_timings) == 0) {
		enum_dv_timings.index++;
		
		printf("%d. %d-%d-%lld\n", enum_dv_timings.index, 
			enum_dv_timings.timings.bt.width,
			enum_dv_timings.timings.bt.height,
			enum_dv_timings.timings.bt.pixelclock);
	}
	/* 
	Support enum_dv_timings:
	1. 720-480-27000000			V4L2_DV_BT_CEA_720X480P59_94
	2. 720-576-27000000			V4L2_DV_BT_CEA_720X576P50
	3. 1280-720-74250000		V4L2_DV_BT_CEA_1280X720P50
	4. 1280-720-74250000		V4L2_DV_BT_CEA_1280X720P60
	5. 1920-1080-74250000		V4L2_DV_BT_CEA_1920X1080P24
	6. 1920-1080-74250000		V4L2_DV_BT_CEA_1920X1080P30
	7. 1920-1080-148500000		V4L2_DV_BT_CEA_1920X1080P50
	8. 1920-1080-74250000		V4L2_DV_BT_CEA_1920X1080I50
	9. 1920-1080-74250000		V4L2_DV_BT_CEA_1920X1080I60
	10. 1920-1080-148500000		V4L2_DV_BT_CEA_1920X1080P60
	 */
	}
	
	{
	struct v4l2_dv_timings dv_timings;
	ret = ioctl(fd, VIDIOC_G_DV_TIMINGS, &dv_timings);
	BYE_ON(ret != 0, "VIDIOC_G_DV_TIMINGS failed: %s\n", ERRSTR);
	printf("before VIDIOC_S_DV_TIMINGS: %d-%d-%lld\n",
		dv_timings.bt.width,
		dv_timings.bt.height,
		dv_timings.bt.pixelclock);
	/* 
	before VIDIOC_S_DV_TIMINGS: 720-480-27000000
	 */
	}
	
	{
	struct v4l2_dv_timings dv_timings = output_formats[fmt_index].dv_timings;
	ret = ioctl(fd, VIDIOC_S_DV_TIMINGS, &dv_timings);
	BYE_ON(ret != 0, "VIDIOC_S_DV_TIMINGS failed: %s\n", ERRSTR);
	}
	
	{
	struct v4l2_dv_timings dv_timings;
	ret = ioctl(fd, VIDIOC_G_DV_TIMINGS, &dv_timings);
	BYE_ON(ret != 0, "VIDIOC_G_DV_TIMINGS failed: %s\n", ERRSTR);
	printf("after VIDIOC_S_DV_TIMINGS: %d-%d-%lld\n",
		dv_timings.bt.width,
		dv_timings.bt.height,
		dv_timings.bt.pixelclock);
	/* 
	after VIDIOC_S_DV_TIMINGS: 1280-720-74250000
	 */
	}
	
	{
	struct v4l2_fmtdesc fmtdesc;
    fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	printf("Support format:\n");
    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0) {
        fmtdesc.index++;
		
        printf("%d. %s\t%c%c%c%c\n", fmtdesc.index, fmtdesc.description,
			fmtdesc.pixelformat & 0xff,
			(fmtdesc.pixelformat >> 8) & 0xff,
			(fmtdesc.pixelformat >> 16) & 0xff,
			(fmtdesc.pixelformat >> 24) & 0xff);
    }
	/* 
	Support format:
	1. RGB565       RGBP V4L2_PIX_FMT_RGB565
	2. ARGB1555     RGBO V4L2_PIX_FMT_RGB555
	3. ARGB4444     R444 V4L2_PIX_FMT_RGB444
	4. ARGB8888     BGR4 V4L2_PIX_FMT_BGR32
	 */
	}
	
	{
	struct v4l2_format format;
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	BYE_ON(ret != 0, "VIDIOC_G_FMT failed: %s\n", ERRSTR);
	printf("before VIDIOC_S_FMT: %d-%d-%c%c%c%c\n",
			format.fmt.pix_mp.width, format.fmt.pix_mp.height,
			format.fmt.pix_mp.pixelformat & 0xff,
			(format.fmt.pix_mp.pixelformat >> 8) & 0xff,
			(format.fmt.pix_mp.pixelformat >> 16) & 0xff,
			(format.fmt.pix_mp.pixelformat >> 24) & 0xff);
	/* 
	before VIDIOC_S_FMT: 1280-720-RGBP
	 */
	}
	
	{
	struct v4l2_format format;
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	format.fmt.pix_mp.width = output_formats[fmt_index].width;
	format.fmt.pix_mp.height = output_formats[fmt_index].height;
	format.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_BGR32;
	ret = ioctl(fd, VIDIOC_S_FMT, &format);
	BYE_ON(ret != 0, "VIDIOC_S_FMT failed: %s\n", ERRSTR);
	}
	
	{
	struct v4l2_format format;
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	BYE_ON(ret != 0, "VIDIOC_G_FMT failed: %s\n", ERRSTR);
	printf("after VIDIOC_S_FMT: %d-%d-%c%c%c%c\n",
			format.fmt.pix_mp.width, format.fmt.pix_mp.height,
			format.fmt.pix_mp.pixelformat & 0xff,
			(format.fmt.pix_mp.pixelformat >> 8) & 0xff,
			(format.fmt.pix_mp.pixelformat >> 16) & 0xff,
			(format.fmt.pix_mp.pixelformat >> 24) & 0xff);
	/* 
	after VIDIOC_S_FMT: 1280-720-BGR4
	 */
	}
	
	{
	/* 
	vidioc_g_selection VIDIOC_G_SELECTION
	vidioc_s_selection VIDIOC_S_SELECTION
	 */
	}
	
	{
	struct v4l2_requestbuffers rqbufs;
	rqbufs.count = BUFFER_CNT;
	rqbufs.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	rqbufs.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(fd, VIDIOC_REQBUFS, &rqbufs);
	BYE_ON(ret != 0, "VIDIOC_REQBUFS failed: %s\n", ERRSTR);
	BYE_ON(rqbufs.count < BUFFER_CNT, "failed to get %d buffers\n",
		BUFFER_CNT);
	}
	
	struct buffer buffer[BUFFER_CNT];

	/* buffers initalization */
	for (int i = 0; i < BUFFER_CNT; ++i) {
		struct v4l2_buffer buf;
		struct v4l2_plane plane;
		
		buf.index = i;
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.m.planes = &plane;
		buf.length = 1;
		
		/* get buffer properties from a driver */
		ret = ioctl(fd, VIDIOC_QUERYBUF, &buf);
		BYE_ON(ret != 0, "VIDIOC_QUERYBUF for buffer %d failed: %s\n",
			buf.index, ERRSTR);

		buffer[i].index = i;
		/* mmap buffer to user space */
		buffer[i].data = mmap(NULL, plane.length, PROT_READ | PROT_WRITE,
			MAP_SHARED, fd, plane.m.mem_offset);
		BYE_ON(buffer[i].data == MAP_FAILED, "mmap failed: %s\n",
			ERRSTR);
		/* size_t size = 4 * output_formats[fmt_index].width * output_formats[fmt_index].height; */
		buffer[i].size = plane.length;
		buffer[i].width = output_formats[fmt_index].width;
		buffer[i].height = output_formats[fmt_index].height;
		
		/* fill buffer with black */
		for (size_t j = 0; j < (buffer[i].size / 4); ++j) {
			((uint32_t*)buffer[i].data)[j] = 0xff000000;
		}
	}

	struct context context;
	context.fd = fd;
	context.buffer = buffer;
	context.buffer_cnt = BUFFER_CNT;
	/* It's show-time !!! */
	showtime(&context);

	return 0;
}

double gettime(void);
void buffer_fill(struct buffer *buf, double t);
int queue(int fd, int index);
int dequeue(int fd, int *index);

int showtime(struct context *ctx)
{
	double t;

	/* fill and pass all buffers to the driver */
	for (int i = 0; i < ctx->buffer_cnt; ++i) {
		t = gettime();
		buffer_fill(&ctx->buffer[i], t);
		queue(ctx->fd, i);
	}
	
	fprintf(stderr, "start\n");
	
	/* start streaming */
	int ret, type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	ret = ioctl(ctx->fd, VIDIOC_STREAMON, &type);
	BYE_ON(ret != 0, "VIDIOC_STREAMON failed: %s\n", ERRSTR);

	int index;
	/* main loop, every dequeued buffer is refilled and enqueued again */
	while (dequeue(ctx->fd, &index) == 0) {
		t = gettime();
		buffer_fill(&ctx->buffer[index], t);
		queue(ctx->fd, index);
	}
	return 0;
}

double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	double t = tv.tv_sec + 1e-6 * tv.tv_usec;
	return t;
}

void draw_rect(struct buffer *buf, struct v4l2_rect *r, uint32_t v)
{
	uint32_t *ptr = buf->data;
	ptr += buf->width * r->top + r->left;
	for (int i = 0; i < r->height; ++i) {
		for (int j = 0; j < r->width; ++j)
			ptr[j] = v;
		ptr += buf->width;
	}
}

void prepare_rect(struct buffer *buf, struct v4l2_rect *r, double t)
{
	r->left = 0.8 * buf->width * (0.5 + 0.5 * sin(t));
	r->top = 0.8 * buf->height * (0.5 + 0.5 * sin(1.41 * t));
	r->width = 0.2 * buf->width;
	r->height = 0.2 * buf->height;
}

void buffer_fill(struct buffer *buf, double t)
{
	struct v4l2_rect r;
	prepare_rect(buf, &r, buf->t);
	draw_rect(buf, &r, 0xff000000);
	prepare_rect(buf, &r, t);
	int R, G, B;
	R = 125 + 125 * sin(3 * t);
	G = 125 + 125 * sin(4 * t);
	B = 125 + 125 * sin(5 * t);
	draw_rect(buf, &r, (R << 0) | (G << 8) | (B << 16) | (0xff << 24));
	buf->t = t;
}

int queue(int fd, int index)
{
	int ret;
	struct v4l2_buffer buf;
	struct v4l2_plane plane;
	
	memset(&buf, 0, sizeof buf);
	memset(&plane, 0, sizeof plane);
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	buf.index = index;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.m.planes = &plane;
	buf.length = 1;
	
	ret = ioctl(fd, VIDIOC_QBUF, &buf);
	BYE_ON(ret != 0, "VIDIOC_QBUF(index = %d) failed: %s\n",
		index, ERRSTR);
	
	return 0;
}

int dequeue(int fd, int *index)
{
	int ret;
	struct v4l2_buffer buf;
	struct v4l2_plane plane;
	
	memset(&buf, 0, sizeof buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.m.planes = &plane;
	buf.length = 1;
	
	ret = ioctl(fd, VIDIOC_DQBUF, &buf);
	BYE_ON(ret != 0, "VIDIOC_DQBUF failed: %s\n", ERRSTR);
	
	*index = buf.index;
	return 0;
}