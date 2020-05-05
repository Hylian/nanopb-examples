#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "pb_common.h"
#include "pb_encode.h"
#include "camera.pb.h"

// Fat pointer type to pass a dynamically sized string to our nanopb callback.
// We could pass the string pointer directly and rely on NULL termination, but more
// complex usecases can require passing objects like this around.
typedef struct {
  char * string;
  size_t len;
} StringContainer;

// Encode callback for nanopb. We pass this to nanopb in the struct we want to encode,
// along with an argument. Nanopb provides the `stream` and `field` values, while passing the
// pointer to the user provided arg.
static bool encode_string_cb(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
  // Typecast the (void **) back into a pointer to our fat string pointer type.
  StringContainer * string_to_write = (StringContainer *) *arg;

  // Encode the protobuf tag for this field. This is passed to us by nanopb in the `field` variable.
  // In this case, the field tag would be `hylian_camera_Camera_name_tag`.
  if (!pb_encode_tag_for_field(stream, field)) {
    printf("pb_encode_tag_for_field returned failure!\n");
    return false;
  }

  // Encode the string itself
  if (!pb_encode_string(stream, string_to_write->string, string_to_write->len)) {
    printf("pb_encode_string returned failure!\n");
    return false;
  }

  return true;
}

int main(void) {
  // Create the fat pointer holding the string we want to encode
  StringContainer my_name = {
    .string = "Leica M5",
    .len = strlen("Leica M5"),
  };

  // Create the protobuf struct to encode
  hylian_camera_Camera my_camera = {
    // `name` is now a dynamic string, so this field is of type `pb_callback_t`.
    // This type is defined in `pb.h` as:
    // ```
    // typedef struct pb_callback_s pb_callback_t;
    // struct pb_callback_s {
    //   union {
    //     bool (*decode)(pb_istream_t *stream, const pb_field_t *field, void **arg);
    //     bool (*encode)(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);
    //   } funcs;
    //   void *arg;
    // };
    // ```
    // We must provide a callback that encodes this field, or optionally doesn't encode the field at all.
    // The `arg` pointer will be passed as userdata to this callback.
    .name = {
      .funcs.encode = encode_string_cb,
      .arg = (void *) &my_name,
    },
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
