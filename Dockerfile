FROM l.gcr.io/google/bazel:latest as build

RUN apt-get -y install libreadline-dev bison flex zlib1g-dev 
RUN apt-get -y install --reinstall build-essential
