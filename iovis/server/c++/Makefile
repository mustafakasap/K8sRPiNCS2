all: inference_service.pb.o inference_service.grpc.pb.o inference_server.o inference_server

lib_path = $(HOME)


inference_server: inference_server.o inference_service.pb.o inference_service.grpc.pb.o
	g++ \
	$^ \
	`pkg-config --libs protobuf grpc++ grpc` \
	-L/usr/local/lib \
	$(lib_path)/inference_engine_vpu_arm/deployment_tools/inference_engine/lib/armv7l/libinference_engine.so \
	$(lib_path)/inference_engine_vpu_arm/deployment_tools/inference_engine/lib/armv7l/libmyriadPlugin.so \
	$(lib_path)/inference_engine_vpu_arm/opencv/lib/libopencv_imgcodecs.so.4.1.0 \
	$(lib_path)/inference_engine_vpu_arm/opencv/lib/libopencv_imgproc.so.4.1.0 \
	$(lib_path)/inference_engine_vpu_arm/opencv/lib/libopencv_core.so.4.1.0 \
	-Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl \
	-lpthread \
	-rdynamic \
	-o $@

inference_server.o: inference_server.cc
	g++ \
	-Wall -std=c++11 -march=armv7-a \
	`pkg-config --cflags protobuf grpc` \
	-I $(lib_path)/inference_engine_vpu_arm/inference_engine/include \
	-I $(lib_path)/inference_engine_vpu_arm/inference_engine/src/extension \
	-I $(lib_path)/inference_engine_vpu_arm/opencv/include \
	-c -o $@ $<

inference_service.grpc.pb.o: inference_service.grpc.pb.cc
	g++ -std=c++11 `pkg-config --cflags protobuf grpc` -c -o $@ $<

inference_service.grpc.pb.cc: inference_service.proto
	protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<



inference_service.pb.o: inference_service.pb.cc
	g++ -std=c++11 `pkg-config --cflags protobuf grpc` -c -o $@ $<

inference_service.pb.cc: inference_service.proto
	protoc -I . --cpp_out=. $<



clean:
	rm -f *.o *.pb.cc *.pb.h inference_server
