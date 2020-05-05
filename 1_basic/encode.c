#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "pb_common.h"
#include "pb_encode.h"
#include "camera.pb.h"

int main(void) {
  // Create the protobuf struct to encode
  hylian_camera_Camera my_camera = {
    .name = "Leica M5",
    .available = true,
    .type = hylian_camera_Camera_CameraType_RANGEFINDER,
  };

  char out_buf[200] = {0};

  // Create an output stream from a buffer
  pb_ostream_t ostream = pb_ostream_from_buffer(out_buf, sizeof(out_buf));

  if (!pb_encode(&ostream, hylian_camera_Camera_fields, &my_camera)) {
    printf("pb_encode returned failure!\n");
    return 1;
  }

  printf("Encode success!\n");

  // Test the encoded output against the protoc generated output
  FILE *test_file = fopen("build/camera_test.bin", "rb");
  if (!test_file) {
    printf("Unable to open test file!\n");
    return 1;
  }

  char test_buf[200] = {0};
  fread(test_buf, ostream.bytes_written, 1, test_file);
  fclose(test_file);
  if (memcmp(out_buf, test_buf, ostream.bytes_written)) {
    printf("FAIL: Output does not match test file!\n");
    return 1;
  }
  printf("PASS: Output matches test file!\n");

  return 0;
}
