# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(SES_TOOLCHAIN_VARIANT "Andes")

set(CONFIG_HPM_MATH 1)
set(CONFIG_HPM_MATH_DSP 1)
set(CONFIG_FATFS 1)
set(CONFIG_SDMMC 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(demo)
sdk_ses_compile_options(-mabi=ilp32f)
sdk_ses_compile_options(-march=rv32imafc)
sdk_compile_definitions(-DSD_FATFS_ENABLE=1)
sdk_compile_definitions(-DFF_CODE_PAGE=437)
sdk_compile_definitions(-D__NDS32_EXT_PERF__)
sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA)

sdk_inc(inc)
sdk_inc(libs/dma)
sdk_inc(libs/hmi_drv)

sdk_app_src(src/main.c)
sdk_app_src(src/init.c)
sdk_app_src(src/interrupts.c)
sdk_app_src(src/err_print.c)
sdk_app_src(src/config.c)

sdk_app_src(libs/dma/dma.c)
sdk_app_src(libs/hmi_drv/hmi_drv.c)

sdk_compile_options(-Ofast)

generate_ses_project()
