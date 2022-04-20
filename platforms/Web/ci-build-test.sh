#!/bin/bash
echo [ci-build-test.sh Start!]
mkdir build
cd ../../Assets
zip -r Assets.zip . -x '*Assets.zip*'
mv Assets.zip ../platforms/Web/build/Assets.zip
cd ../platforms/Web/build
emcmake cmake -DCMAKE_BUILD_TYPE=Debug ../
cmake --build .
mkdir result
mv CSEngineWeb.data CSEngineWeb.html CSEngineWeb.js CSEngineWeb.wasm CSEngineWeb.wasm.map result/
