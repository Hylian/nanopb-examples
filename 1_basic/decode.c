#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "pb_common.h"
#include "pb_decode.h"
#include "camera.pb.h"

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

  // Create camera struct to hold decoded values
  hylian_camera_Camera out_camera = {0};

  // Decode the test input into a struct
  if (!pb_decode(&istream, hylian_camera_Camera_fields, &out_camera)) {
    printf("pb_decode returned failure!\n");
    return 1;
  }

  printf("Decode success!\n");

  // Test the results to make sure they're what we expect
  hylian_camera_Camera expected_camera = {
    .name = "Leica M5",
    .available = true,
    .type = hylian_camera_Camera_CameraType_RANGEFINDER,
  };

  if (strcmp(expected_camera.name, out_camera.name) ||
      expected_camera.available != out_camera.available ||
      expected_camera.type != out_camera.type) {
    printf("FAIL: Output does not match test file!\n");
    return 1;
  }

  printf("PASS: Output matches test file!\n");

  return 0;
}
