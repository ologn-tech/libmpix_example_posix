#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <mpix/image.h>

#define APP_WIDTH 1600
#define APP_HEIGHT 1296
#define APP_FOURCC MPIX_FMT_SBGGR8

uint8_t buf_in[APP_HEIGHT * APP_WIDTH];
uint8_t buf_out[APP_HEIGHT * APP_WIDTH * 3];

static int app_read_test_data(void)
{
	int fd = open("test.raw", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Error opening test.raw: %s\n", strerror(errno));
		return -1;
	}

	ssize_t bytes_read = read(fd, buf_in, sizeof(buf_in));
	close(fd);

	if (bytes_read < 0) {
		fprintf(stderr, "Error reading test.raw: %s\n", strerror(errno));
		return -1;
	}

	if (bytes_read != sizeof(buf_in)) {
		fprintf(stderr, "Warning: Expected %zu bytes, read %zd bytes\n", 
			sizeof(buf_in), bytes_read);
	}

	return 0;
}

static int app_save_bmp(const char *filename, uint8_t *rgb_data, uint16_t width, uint16_t height)
{
	FILE *file = fopen(filename, "wb");
	if (!file) {
		fprintf(stderr, "Error opening %s for writing: %s\n", filename, strerror(errno));
		return -1;
	}

	// BMP header (14 bytes)
	uint8_t bmp_header[14] = {
		0x42, 0x4D,  // BM signature
		0x00, 0x00, 0x00, 0x00,  // File size (will be filled later)
		0x00, 0x00, 0x00, 0x00,  // Reserved
		0x36, 0x00, 0x00, 0x00   // Offset to pixel data (54 bytes)
	};

	// DIB header (40 bytes)
	uint8_t dib_header[40] = {
		0x28, 0x00, 0x00, 0x00,  // Header size (40 bytes)
		0x00, 0x00, 0x00, 0x00,  // Width (will be filled)
		0x00, 0x00, 0x00, 0x00,  // Height (will be filled)
		0x01, 0x00,              // Planes (1)
		0x18, 0x00,              // Bits per pixel (24)
		0x00, 0x00, 0x00, 0x00,  // Compression (none)
		0x00, 0x00, 0x00, 0x00,  // Image size (will be filled)
		0x13, 0x0B, 0x00, 0x00,  // X pixels per meter
		0x13, 0x0B, 0x00, 0x00,  // Y pixels per meter
		0x00, 0x00, 0x00, 0x00,  // Colors in color table
		0x00, 0x00, 0x00, 0x00   // Important color count
	};

	// Calculate row size (must be multiple of 4)
	uint32_t row_size = ((width * 3 + 3) / 4) * 4;
	uint32_t image_size = row_size * height;
	uint32_t file_size = 54 + image_size;

	// Fill in the headers
	*(uint32_t*)(bmp_header + 2) = file_size;
	*(uint32_t*)(dib_header + 4) = width;
	*(uint32_t*)(dib_header + 8) = height;
	*(uint32_t*)(dib_header + 20) = image_size;

	// Write headers
	if (fwrite(bmp_header, 1, 14, file) != 14 ||
	    fwrite(dib_header, 1, 40, file) != 40) {
		fprintf(stderr, "Error writing BMP headers\n");
		fclose(file);
		return -1;
	}

	// Write pixel data (BMP is stored bottom-to-top)
	uint8_t *row_buffer = malloc(row_size);
	if (!row_buffer) {
		fprintf(stderr, "Error allocating row buffer\n");
		fclose(file);
		return -1;
	}

	for (int y = height - 1; y >= 0; y--) {
		// Copy RGB data for this row
		uint8_t *src_row = &rgb_data[y * width * 3];
		for (int x = 0; x < width; x++) {
			// BMP stores as BGR, not RGB
			row_buffer[x * 3 + 0] = src_row[x * 3 + 2]; // B
			row_buffer[x * 3 + 1] = src_row[x * 3 + 1]; // G
			row_buffer[x * 3 + 2] = src_row[x * 3 + 0]; // R
		}
		// Pad row to multiple of 4 bytes
		for (uint32_t x = width * 3; x < row_size; x++) {
			row_buffer[x] = 0;
		}

		if (fwrite(row_buffer, 1, row_size, file) != row_size) {
			fprintf(stderr, "Error writing pixel data\n");
			free(row_buffer);
			fclose(file);
			return -1;
		}
	}

	free(row_buffer);
	fclose(file);

	printf("Successfully saved BMP file: %s (%dx%d, %d bytes)\n", 
		filename, width, height, file_size);
	return 0;
}

int main(void)
{
	struct mpix_image img;

	if (app_read_test_data() != 0) {
		return 1;
	}

	mpix_image_from_buf(&img, buf_in, sizeof(buf_in), APP_WIDTH, APP_HEIGHT, APP_FOURCC);
	mpix_image_debayer(&img, 2);
	mpix_image_convert(&img, MPIX_FMT_RGB24);
	mpix_image_to_buf(&img, buf_out, sizeof(buf_out));

	if (img.err != 0) {
		printf("Oops an error occured: %s!\n", strerror(-img.err));
		return img.err;
	}

	// mpix_image_print_truecolor(&img);

	// Save the processed image data as BMP
	if (app_save_bmp("output.bmp", buf_out, APP_WIDTH, APP_HEIGHT) != 0) {
		return 1;
	}

	return 0;
}
