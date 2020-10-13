# base image
#FROM ubuntu:rolling
FROM placket/ubuntu-opencv:3.4.6

# install essential packages
RUN apt-get update
RUN apt install -y libboost-all-dev x11-apps

# Project build
WORKDIR /project
RUN git clone https://github.com/ralpioxxcs/QT_ImageViewer.git \
    && cd QT_ImageViewer \
    && cmake -H. -BRelease \ 
    && cd Release \
    && cmake --build . --config Release
# clean
WORKDIR /
RUN apt-get autoclean autoremove
RUN rm -rf /var/lib/apt/lists/*

# default command.
WORKDIR /project/QT_ImageViewer/Release
CMD ["./imgviewer"]
