#!/bin/bash
echo [ci-build-test.sh Start!]
mkdir build
cp Assets_for_web.zip ../platforms/Web/build/Assets.zip
cd ../platforms/Web/build
emcmake cmake -DCMAKE_BUILD_TYPE=Debug ../
cmake --build .
mkdir result
mv CSEngineWeb.data CSEngineWeb.html CSEngineWeb.js CSEngineWeb.wasm CSEngineWeb.wasm.map result/
