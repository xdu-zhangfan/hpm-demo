/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 *
 * Automatically generated by VSCode plugin: HPM Pinmux Tool, version: 0.2.14.
 * Generated Time: 2023/12/30 06:40:11
 *
 *
 * Note:
 *   PY and PZ IOs: if any SOC pin function needs to be routed to these IOs,
 *  besides of IOC, PIOC/BIOC needs to be configured SOC_GPIO_X_xx, so that
 *  expected SoC function can be enabled on these IOs.
 */

#include "board.h"


void init_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PZ06].PAD_CTL = IOC_PAD_PAD_CTL_SPD_SET(3) | IOC_PAD_PAD_CTL_PE_SET(0) | IOC_PAD_PAD_CTL_KE_SET(0);
    HPM_IOC->PAD[IOC_PAD_PZ06].FUNC_CTL = IOC_PZ06_FUNC_CTL_UART6_TXD;
    HPM_BIOC->PAD[IOC_PAD_PZ06].FUNC_CTL = IOC_PZ06_FUNC_CTL_SOC_PZ_06;

    HPM_IOC->PAD[IOC_PAD_PZ07].PAD_CTL = IOC_PAD_PAD_CTL_SPD_SET(3) | IOC_PAD_PAD_CTL_PE_SET(0) | IOC_PAD_PAD_CTL_KE_SET(0);
    HPM_IOC->PAD[IOC_PAD_PZ07].FUNC_CTL = IOC_PZ07_FUNC_CTL_UART6_RXD;
    HPM_BIOC->PAD[IOC_PAD_PZ07].FUNC_CTL = IOC_PZ07_FUNC_CTL_SOC_PZ_07;

}
