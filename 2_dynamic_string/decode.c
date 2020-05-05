#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "pb_common.h"
#include "pb_decode.h"
#include "camera.pb.h"

// Fat pointer type to pass a dynamically sized buffer to our nanopb callback.
typedef struct {
  char * buf;
  size_t len;
} BufContainer;

static bool decode_string_cb(pb_istream_t *stream, const pb_field_t *field, void **arg) {
  // Typecast the arg to our BufContainer type.
  BufContainer *container = (BufContainer *)*arg;

  // Trim the string if it's too long.
  size_t bytes_left = stream->bytes_left;
  if (bytes_left >= container->len) {
    bytes_left = container->len - 1;
  }

  // Read out the string to the buffer.
  // We don't need to worry about decoding the tag here, because nanopb has
  // already done that for us.
  if (!pb_read(stream, container->buf, bytes_left)) {
    printf("pb_read returned failure!\n");
    return false;
  }

  // Ensure NULL termination
  container->buf[bytes_left] = '\0';

  return true;
}

int main(void) {
  // Read the encoded output from the protoc generated output
  FILE *test_file = fopen("build/camera_test.bin", "rb");
  if (!test_file) {
    printf("Unable to open test file!\n");
    return 1;
  }
  char in_buf[200] = {0};
  size_t in_len = 0;
  while (fread(&in_buf[in_len], 1, 1, test_file) == 1) {
    in_len++;
  }
  fclose(test_file);

  // Create istream from the buffer
  pb_istream_t istream = pb_istream_from_buffer(in_buf, in_len);

  // Create BufContainer type to hold the dynamically sized name string
  char name_buf[100];
  BufContainer name_container = {
    .buf = name_buf,
    .len = sizeof(name_buf),
  };

  // Create camera struct to hold decoded values
  hylian_camera_Camera out_camera = {0};
  out_camera.name = (pb_callback_t) {
    .funcs.decode = decode_string_cb,
    .arg = &name_container,
  };

  // Decode the test input into a struct
  if (!pb_decode(&istream, hylian_camera_Camera_fields, &out_camera)) {
    printf("pb_decode returned failure!\n");
    return 1;
  }

  printf("Decode success!\n");

  // Test the results to make sure they're what we expect
  if (strcmp("Leica M5", name_buf) ||
      true != out_camera.available ||
      hylian_camera_Camera_CameraType_RANGEFINDER != out_camera.type) {
    printf("FAIL: Output does not match test file!\n");
    return 1;
  }

  printf("PASS: Output matches test file!\n");

  return 0;
}
