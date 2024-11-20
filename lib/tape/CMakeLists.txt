add_library(TapeLib
            delays/delays.cpp delays/delays.hpp
            tape_interface.hpp 
            tape.cpp tape.hpp
            sorter/tape_sorter.cpp sorter/tape_sorter.hpp 
            chunk/chunk.cpp chunk/chunk.hpp)

target_link_libraries(TapeLib)
