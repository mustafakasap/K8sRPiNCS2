all: test.pb.o test.grpc.pb.o test_server.o test_server test_client.o test_client

test_server: test_server.o test.pb.o test.grpc.pb.o
	g++ \
	$^ \
	`pkg-config --libs protobuf grpc++ grpc` \
	-L/usr/local/lib \
	-Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl \
	-lpthread \
	-rdynamic \
	-o $@

test_server.o: test_server.cc
	g++ \
	-Wall -std=c++11 -march=armv7-a \
	`pkg-config --cflags protobuf grpc` \
	-c -o $@ $<

test_client: test.pb.o test.grpc.pb.o test_client.o
	g++ \
	$^ \
	-L/usr/local/lib \
	`pkg-config --libs protobuf grpc++ grpc` \
	-Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl \
	-o $@

test_client.o: test_client.cc
	g++ \
	-Wall -std=c++11 \
	`pkg-config --cflags protobuf grpc` \
	-c -o $@ $<

test.grpc.pb.o: test.grpc.pb.cc
	g++ -std=c++11 `pkg-config --cflags protobuf grpc` -c -o $@ $<

test.grpc.pb.cc: test.proto
	protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<

test.pb.o: test.pb.cc
	g++ -std=c++11 `pkg-config --cflags protobuf grpc` -c -o $@ $<

test.pb.cc: test.proto
	protoc -I . --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h test_server test_client
