# SPDX-License-Identifier: Unlicense
# src/CMakeLists.txt

target_sources( ${PROJECT_NAME}
    PRIVATE
        Example.h
        Example.cpp
        GDExtensionTemplate.h
        GDExtensionTemplate.cpp
        RegisterExtension.cpp
        EcalFramePublisher.hpp
        EcalFramePublisher.cpp
)

target_include_directories( ${PROJECT_NAME}
    PRIVATE
        "src"
)
