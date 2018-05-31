FROM ubuntu:14.04

ENV DEBIAN_FRONTEND noninteractive

RUN cat /etc/lsb-release
RUN apt-get -qq update
RUN apt-get install -qq software-properties-common
RUN apt-add-repository -y ppa:beineri/opt-qt563-trusty
RUN apt-get update -qq
RUN apt-get install -qq build-essential clang libgl1-mesa-dev libglu1-mesa-dev xvfb wget
RUN apt-get install -qq qt56base qt56declarative qt56tools qt56webengine qt56xmlpatterns qt56x11extras
RUN apt-get install -qq libwrap0 libwrap0-dev zlib1g zlib1g-dev libssl-dev
RUN rm -rf /var/lib/apt/lists/*
RUN unset DEBIAN_FRONTEND

RUN wget -nv --directory-prefix=/ http://trueta.udg.edu/apt/ubuntu/devel/0.15/starviewer-sdk-linux-0.15-2.tar.xz
RUN mkdir /sdk-0.15
RUN tar xf /starviewer-sdk-linux-0.15-2.tar.xz -C /sdk-0.15

ENV SDK_INSTALL_PREFIX /sdk-0.15/usr/local
ENV LD_LIBRARY_PATH /sdk-0.15/usr/local/lib:/sdk-0.15/usr/local/lib/x86_64-linux-gnu

WORKDIR /starviewer/starviewer
CMD ["bash"]
