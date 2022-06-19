#!/bin/bash

bazel clean --expunge
bazel build //:mkr_ecs_lib