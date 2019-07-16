#include <iostream>
#include <string>
#include <memory>

#include <grpcpp/grpcpp.h>

#include "test.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using test::HelloRequest;
using test::HelloReply;
using test::Greeter;

class GreeterServiceImpl final : public Greeter::Service
{
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }
};

int main(int argc, char** argv)
{
	try
	{
        	std::string pIP = argv[1];
        	std::string pPort = argv[2];

        	std::string pSrvAddr = pIP + ":" + pPort;
        	std::cout << "Running test server: " << std::endl;
        	std::cout << "\tAddress: " << pSrvAddr << std::endl;

		std::string server_address(pSrvAddr);
		GreeterServiceImpl service;

		ServerBuilder builder;
		// Listen on the given address without any authentication mechanism.
		builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
		// Register "service" as the instance through which we'll communicate with
		// clients. In this case it corresponds to an *synchronous* service.
		builder.RegisterService(&service);
		// Finally assemble the server.
		std::unique_ptr<Server> server(builder.BuildAndStart());
		std::cout << "Server listening on " << server_address << std::endl;

		// Wait for the server to shutdown. Note that some other thread must be
		// responsible for shutting down the server for this call to ever return.
		server->Wait();
	}
	catch (const std::exception& error) {
		std::cout << error.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cout << "Unknown/internal exception happened." << std::endl;
		return 1;
	}

	std::cout << "Execution successful" << std::endl;
	return 0;
}
