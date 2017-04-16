mkdir platformio-build
platformio ci --verbose --board megaatmega2560 --project-option="lib_extra_dirs=C:\Program Files (x86)\Arduino\libraries" --build-dir=platformio-build --keep-build-dir Marlin