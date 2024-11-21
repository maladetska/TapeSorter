add_library(TapeLib
            tape_interface.hpp 
            delays/delays.cpp delays/delays.hpp
            chunk/chunk.hpp
            chunks_info/chunks_info.cpp chunks_info/chunks_info.hpp
            tape.hpp
            sorter/tape_sorter.hpp 
            )

target_link_libraries(TapeLib)
