#!/bin/bash

# Define variables
executable="./exeGCLF-ImageTriSimp"
config="./config.json"
input_image="./output/cat.png"
input_mesh="./output/cat_init.obj"
output_directory="./output/"
error_bound="10.0"
mode="split"

# Run the executable with the provided arguments
"${executable}" "${config}" "${input_image}" "${input_mesh}" "${output_directory}" "${error_bound}" "${mode}"
