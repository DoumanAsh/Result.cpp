macro(set_output_dir dir)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${dir}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${dir}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/bin)
endmacro(set_output_dir)
