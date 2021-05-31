#!/bin/bash

tars_path=$1
target_path=${2//\//\\/}

perl -i.bak -npe 's/^(\s*)(set\(CMAKE_INSTALL_PREFIX.*)/\1#\2/g' ${tars_path}/cmake/Common.cmake
perl -i.bak -npe 's/^(\s*TARS_PATH\s*\:\=.*)/TARS_PATH:='${target_path}'/g' ${tars_path}/servant/makefile/makefile.tars
perl -i.bak -npe 's/^(^\s*set\(.*)\/usr\/local\/tars(.*)/\1'"${target_path}"'\2/g' ${tars_path}/servant/makefile/tars-tools.cmake

perl -i.bak -npe 's/(.*)\/home\/tarsproto\/(.*)/\1'"${target_path}\/tarsproto"'\2/g' ${tars_path}/servant/makefile/tars-tools.cmake
perl -i.bak -npe 's/(.*)\$ENV\{HOME\}\/tarsproto\/protocol(.*)/\1'"${target_path}\/tarsproto"'\2/g' ${tars_path}/servant/makefile/tars-tools.cmake
perl -i.bak -npe 's/\/home\/tarsproto\/protocol\//'"${target_path}\/tarsproto"'/g' ${tars_path}/servant/protocol/servant/CMakeLists.txt
perl -i.bak -npe 's/\$ENV\{HOME\}\/tarsproto\/protocol\//'"${target_path}\/tarsproto"'/g' ${tars_path}/servant/protocol/servant/CMakeLists.txt