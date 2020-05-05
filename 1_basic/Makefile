CC ?= gcc
OUTPUT_DIR = ./build

NANOPB_SRC += \
	../nanopb/pb_encode.c \
	../nanopb/pb_decode.c \
	../nanopb/pb_common.c

NANOPB_INC += -I ../nanopb/

.PHONY: all generate_test_file encode decode clean

all: encode decode

# Use the nanopb generator to generate code for our proto files
$(OUTPUT_DIR)/basic.pb.c $(OUTPUT_DIR)/basic.pb.h:
	@mkdir -p $(@D)
	python ../nanopb/generator/nanopb_generator.py -D $(OUTPUT_DIR) basic.proto 

# Generate test binaries from text descriptions of a proto message
$(OUTPUT_DIR)/basic_test.bin:
	@mkdir -p $(@D)
	protoc --encode=hylian.camera.Camera basic.proto < basic_test.txt > $(OUTPUT_DIR)/basic_test.bin

generate_test_file: $(OUTPUT_DIR)/basic_test.bin

# Build and run encode test
encode: $(OUTPUT_DIR)/basic.pb.c $(OUTPUT_DIR)/basic.pb.h generate_test_file
	@mkdir -p $(@D)
	$(CC) -o $(OUTPUT_DIR)/encode encode.c $(OUTPUT_DIR)/basic.pb.c $(NANOPB_SRC) -I $(OUTPUT_DIR) $(NANOPB_INC)
	./$(OUTPUT_DIR)/encode

# Build and run decode test
decode: $(OUTPUT_DIR)/basic.pb.c $(OUTPUT_DIR)/basic.pb.h generate_test_file
	@mkdir -p $(@D)
	$(CC) -o $(OUTPUT_DIR)/decode decode.c $(OUTPUT_DIR)/basic.pb.c $(NANOPB_SRC) -I $(OUTPUT_DIR) $(NANOPB_INC)
	./$(OUTPUT_DIR)/decode

clean:
	rm -r $(OUTPUT_DIR)
