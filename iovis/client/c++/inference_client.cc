#include <iostream>
#include <memory>
#include <string>
#include <fstream>

#include <grpcpp/grpcpp.h>

#include "inference_service.grpc.pb.h"

#define buffersize 4096

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientWriter;

using inferer::Inferer;
using inferer::Request;
using inferer::Reply;


class InferenceClient {
public:
	InferenceClient(std::shared_ptr<Channel> channel) : stub_(Inferer::NewStub(channel)) {}
	void Infer(const std::string& filepath) {
		Reply reply;
		ClientContext context;
		char * memblock;

		int fileByte;
		std::ifstream f(filepath, std::ios::in | std::ios::binary | std::ios::ate);
		if (f.is_open()) {
			fileByte = f.tellg();
		}
		else {
			std::cout << "Unable to open the file.";
			std::exit(-1);
		}

		memblock = new char[buffersize];
		f.seekg(0, std::ios::beg);
		std::unique_ptr<ClientWriter<Request>> writer(stub_->infer(&context, &reply));
		int size = buffersize;
		while (f) {
			f.read(memblock, buffersize);
			if (f.eof()) {
				size = fileByte % buffersize;
			}
			Request request;
			std::string s(memblock, size);
			request.set_content(s);
			if (!writer->Write(request)) {
				break;
			}
		}

		delete[] memblock;

		writer->WritesDone();
		Status status = writer->Finish();

		if (status.ok()) {
			std::cout << "Inference result : " << std::endl << reply.message() << std::endl;
		}
		else {
			std::cout << "client:\t\t " << status.error_code() << ": " << status.error_message();
			std::cout << "client:\t\t file rpc failed." << std::endl;
		}
	}

private:
	std::unique_ptr<Inferer::Stub> stub_;
};

int main(int argc, char** argv) {
	try
	{
		std::string pIP = argv[1];
		std::string pPort = argv[2];
		std::string pFullFileName = argv[3];

		std::string pSrvAddr = pIP + ":" + pPort;
		std::cout << "Calling inference server: " << std::endl;
		std::cout << "\tFull file name: " << pFullFileName << std::endl;
		std::cout << "\tServer address: " << pSrvAddr << std::endl;

		InferenceClient file(grpc::CreateChannel(pSrvAddr, grpc::InsecureChannelCredentials()));

		std::cout << "\nSending file for inference..." << std::endl;
		file.Infer(pFullFileName);
	}
	catch (const std::exception& error) {
		std::cout << error.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cout << "Unknown/internal exception happened." << std::endl;
		return 1;
	}
	return 0;
}

