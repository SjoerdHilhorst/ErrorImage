#!/bin/bash

# Define variables
executable="./exeGCLF-ImageTriSimp"
config="./config.json"
input_image="./cat.png"
input_mesh="./simplified_linear_mesh.obj"
output_directory="./output/"
error_bound="10.0"
mode="curved"

# Run the executable with the provided arguments
"${executable}" "${config}" "${input_image}" "${input_mesh}" "${output_directory}" "${error_bound}" "${mode}"
