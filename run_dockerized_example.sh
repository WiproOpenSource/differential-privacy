#!/bin/bash
cd ..
SRC_ROOT=$(echo $PWD)
echo $SRC_ROOT
cd differential-privacy/differential_privacy
docker run \
  -it \
  -v $SRC_ROOT/differential-privacy/differential_privacy:/differential-privacy/differential_privacy \
  -v $SRC_ROOT/differential-privacy:/differential-privacy \
  -v $SRC_ROOT/tmp/build_output:/tmp/build_output \
  -w /differential-privacy \
  --entrypoint=/bin/bash \
  gdp-build-image \
  bazel run differential_privacy/example:report_the_carrots