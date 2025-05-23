#include <string.h>

#include <mpix/image.h>

#define APP_WIDTH 32
#define APP_HEIGHT 32
#define APP_FOURCC MPIX_FMT_RGB24

uint8_t buf_in[APP_WIDTH * APP_HEIGHT * 3];
uint8_t buf_out[APP_WIDTH * APP_HEIGHT * 3];

int main(void)
{
	struct mpix_image img;

	mpix_image_from_buf(&img, buf_in, sizeof(buf_in), APP_WIDTH, APP_HEIGHT, APP_FOURCC);
	mpix_image_convert(&img, MPIX_FMT_YUYV);
	mpix_image_convert(&img, MPIX_FMT_RGB24);
	mpix_image_to_buf(&img, buf_out, sizeof(buf_out));

	if (img.err != 0) {
		printf("Oops an error occured: %s!\n", strerror(-img.err));
		return img.err;
	}

	return 0;
}
