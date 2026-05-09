add_library(eacp_examples_warnings INTERFACE)

if (MSVC)
    target_compile_options(eacp_examples_warnings INTERFACE /W4)
else ()
    target_compile_options(eacp_examples_warnings INTERFACE
            -Wall -Wextra -Wpedantic)
endif ()
