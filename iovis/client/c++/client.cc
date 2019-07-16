#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <dirent.h>
#include <grpcpp/grpcpp.h>

#include "inference_service.grpc.pb.h"

#define BUFFER_SIZE 4096

using grpc::Channel;
using grpc::ChannelArguments;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientWriter;

using inferer::Inferer;
using inferer::Request;
using inferer::Reply;

class InferenceClient {
public:
	InferenceClient(std::shared_ptr<Channel> channel) : stub_(Inferer::NewStub(channel)) {}
	void Infer(const std::string& p_FolderFileName) {
		Reply reply;
		ClientContext context;
		char * memblock;

		int fileByte;
		std::ifstream f(p_FolderFileName, std::ios::in | std::ios::binary | std::ios::ate);
		if (f.is_open()) {
			fileByte = f.tellg();
		}
		else {
			std::cout << "Unable to open the file." << std::endl;
			std::exit(-1);
		}

		memblock = new char[BUFFER_SIZE];
		f.seekg(0, std::ios::beg);
		std::unique_ptr<ClientWriter<Request>> writer(stub_->infer(&context, &reply));
		int size = BUFFER_SIZE;
		while (f) {
			f.read(memblock, BUFFER_SIZE);
			if (f.eof()) {
				size = fileByte % BUFFER_SIZE;
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
			//std::cout << "Inference result : " << std::endl << reply.message() << std::endl;

			std::string pfn = p_FolderFileName.substr(0, p_FolderFileName.find_last_of(".")) + ".txt";
			std::ofstream myfile;
			myfile.open (pfn);
			myfile << reply.message();
			myfile.close();
		}
		else {
			std::cout << "client:\t\t " << status.error_code() << ": " << status.error_message();
			std::cout << "client:\t\t file rpc failed." << std::endl;
		}
	}

private:
	std::unique_ptr<Inferer::Stub> stub_;
};

int main(int argc, char** argv)
{
	try
	{
		std::string pIP = argv[1];
		std::string pPort = argv[2];
		std::string pFolderName = argv[3];

		std::string pSrvAddr = pIP + ":" + pPort;

			DIR *dir;
			struct dirent *ent;
			int cnt = 0;

			std::cout << "Infer all files in the folder: " << pFolderName << std::endl;
			std::cout << "Creating list of files to be processed..." << std::endl;

			InferenceClient file (grpc::CreateChannel("192.168.2.20:50051", grpc::InsecureChannelCredentials()));

			if ((dir = opendir (pFolderName.c_str())) != NULL)
			{
				while ((ent = readdir (dir)) != NULL)
				{
					if (ent->d_type == DT_REG)
					{
						std::string fn = ent->d_name;
						if(fn.substr(fn.find_last_of(".") + 1) == "jpg")
						{
							std::string pFileName = pFolderName + "/" + fn;
							file.Infer(pFileName);
							std::cout << ++cnt << " " << pFileName << std::endl;
						}
					}
				}

				closedir (dir);
			} else {
				return 1;
			}
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