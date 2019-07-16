# https://hub.docker.com/r/raspbian/stretch
FROM raspbian/stretch:latest

ARG HOME_DIR=/root
ARG INF_SRV=/root/inference_server
ARG TEMP_DIR=/tmp

# #############################################################
# Update OS

RUN     apt-get update && \
        apt-get -y upgrade


# #############################################################
# Install generic tools & libs

RUN     apt-get -y --no-install-recommends install \
        nano \
	curl \
	git \
	cmake \
        python3-pip


# #############################################################
# Setup gRPC & Protoc

# Install tools & libs
RUN     apt-get -y --no-install-recommends install \
        build-essential \
        autoconf \
        libtool \
        pkg-config \
	libgflags-dev \
	libgtest-dev \
	clang \
	libc++-dev \
	automake

RUN     cd $HOME_DIR && \
        git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc && \
	cd $HOME_DIR/grpc && \
        git submodule update --init && \
	make && \
	make install

RUN    cd $HOME_DIR/grpc/third_party/protobuf && \
        make && \
	make install


# #############################################################
# Install OpenVino & USB Stick

# Install tools & libs
RUN     apt-get -y --no-install-recommends install \
        lsb-release \
        libusb-1.0 \
        udev \
        usbutils

# Install Intel OpenVino Toolkit. Default path: inference_engine_vpu_arm
ADD https://download.01.org/opencv/2019/openvinotoolkit/l_openvino_toolkit_raspbi_p_2019.1.094.tgz $HOME_DIR/
RUN     cd $HOME_DIR && \
        tar xf l_openvino_toolkit*.tgz && \
        rm -rf l_openvino_toolkit*.tgz

ENV INTEL_OPENVINO_DIR=$HOME_DIR/inference_engine_vpu_arm

RUN sed -i "s|<INSTALLDIR>|/root/inference_engine_vpu_arm|" $INTEL_OPENVINO_DIR/bin/setupvars.sh

# Setup MYRIAD USB access
RUN $INTEL_OPENVINO_DIR/bin/setupvars.sh
CMD $INTEL_OPENVINO_DIR/install_dependencies/install_NCS_udev_rules.sh



# #############################################################
RUN apt-get install -y net-tools


# #############################################################
# Set Environment
COPY model_zoo $INF_SRV/model_zoo
COPY test $INF_SRV/test
COPY server $INF_SRV/server
COPY client $INF_SRV/client
RUN mkdir $HOME_DIR/docker_mount
ENV LD_LIBRARY_PATH=$INF_SRV/openvino_runtime:$LD_LIBRARY_PATH
ENV INTEL_OPENVINO_MODELS_DIR=$INF_SRV/model_zoo/

# #############################################################
# Compile and Run Inference server
WORKDIR $INF_SRV/server/c++
RUN make

EXPOSE 50051
RUN chmod +x start_inference_server.sh
CMD ./start_inference_server.sh

# #############################################################
# Compile and Run Test server
#WORKDIR $INF_SRV/test/app
#RUN make

#EXPOSE 50051
#RUN chmod +x start_test_server.sh
#CMD ./start_test_server.sh

