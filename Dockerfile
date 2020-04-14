FROM l.gcr.io/google/bazel:latest as build

RUN apt-get -y install libreadline-dev bison flex zlib1g-dev 
RUN apt-get -y install --reinstall build-essential

#TODO check GDP_SRC_ROOT is set and then proceed
#ARG ${GDP_SRC_ROOT:='/''}
#ENV SRC_ROOT=${GDP_SRC_ROOT}
# RUN -it \
#   -v $SRC_ROOT/differential-privacy/differential_privacy:/differential-privacy/differential_privacy \
#   -v $SRC_ROOT/differential-privacy:/differential-privacy \
#   -v $SRC_ROOT/tmp/build_output:/tmp/build_output \
#   -w /differential-privacy \
#   --entrypoint=/bin/bash \
#   gdp-build-image \
#   bazel build differential_privacy/...

# FROM alpine:latest

# WORKDIR /differential-privacy

# COPY --from=build /differential-privacy /differential-privacy

# ENTRYPOINT [ "bazel" ,"build","differential_privacy/..."]