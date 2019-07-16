all: inference_service.pb.o inference_service.grpc.pb.o inference_client.o inference_client

inference_client: inference_service.pb.o inference_service.grpc.pb.o inference_client.o
	g++ \
	$^ \
	-L/usr/local/lib \
	`pkg-config --libs protobuf grpc++ grpc` \
	-Wl,--no-as-needed -lgrpc++_reflection -ldl -pthread \
	-o $@

inference_client.o: inference_client.cc
	g++ \
	-Wall -std=c++11 \
	`pkg-config --cflags protobuf grpc` \
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
	rm -f *.o *.pb.cc *.pb.h inference_client