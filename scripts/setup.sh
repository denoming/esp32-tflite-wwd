#!/usr/bin/env bash

TOP_DIR=$(dirname "$(dirname "$(realpath -s $0)")")
EXT_DIR=${TOP_DIR}/.external

source ${TOP_DIR}/scripts/env

get_tflite_dep() {
    WD="${EXT_DIR}/tflite-micro"
    if [[ -d "${WD}" ]]; then
        rm -fr ${WD}
    fi
    git clone --quiet --recurse-submodules ${TFL_SRC} "${WD}"
    pushd "${WD}"
    git checkout --quiet -b current ${TFL_SRC_HASH}
    popd
    rm -f "${TOP_DIR}/components/tflite-lib"
    ln -s "${WD}/components/tflite-lib" "${TOP_DIR}/components/tflite-lib"
    rm -f "${TOP_DIR}/components/esp-nn"
    ln -s "${WD}/components/esp-nn" "${TOP_DIR}/components/esp-nn"
}

get_kissfft_dep() {
    WD="${EXT_DIR}/kissfft"
    if [[ -d "${WD}" ]]; then
        rm -fr ${WD}
    fi
    git clone --quiet ${KISSFFT_SRC} "${WD}"
    pushd "${WD}"
    git checkout --quiet -b current ${KISSFFT_HASH}
    popd
    rm -f "${TOP_DIR}/components/kissfft"
    ln -s "${WD}" "${TOP_DIR}/components/kissfft"
}

if [[ ! -d "${EXT_DIR}" ]]; then
    mkdir -p "${EXT_DIR}"
fi

get_tflite_dep
get_kissfft_dep