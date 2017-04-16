mkdir platformio-build
platformio ci --verbose --board megaatmega2560 --project-option="lib_extra_dirs=/home/jenkins/.platformio/lib" --project-option="targets=upload" --project-option="upload_port=/dev/ttyUSB1" --build-dir=platformio-build --keep-build-dir Marlin
