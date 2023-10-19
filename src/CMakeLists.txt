add_executable(${ProjectName}
    main.c
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${ProjectName}
    pico_stdlib
    pico_multicore
    FreeRTOS-Kernel-Heap4
    )

add_compile_definitions(
    FREE_RTOS_KERNEL_SMP=1
    portSUPPORT_SMP=1
)

pico_enable_stdio_usb(${ProjectName} 0)
pico_enable_stdio_uart(${ProjectName} 1)

pico_add_extra_outputs(${ProjectName})
