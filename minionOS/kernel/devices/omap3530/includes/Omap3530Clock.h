/*
 * Clock.h
 *
 *  Created on: 2 Apr 2017
 *      Author: Mathias
 */

#ifndef INCLUDES_OMAP3530CLOCK_H_
#define INCLUDES_OMAP3530CLOCK_H_

/* CM module register base address */
#define IVA2_CM             0x48004000
#define OCP_System_Reg_CM   0x48004800
#define MPU_CM              0x48004900
#define CORE_CM             0x48004A00
#define SGX_CM              0x48004B00
#define WKUP_CM             0x48004C00
#define Clock_Control_Reg_CM 0x48004D00
#define DSS_CM              0x48004E00
#define CAM_CM              0x48004F00
#define PER_CM              0x48005000
#define EMU_CM              0x48005100
#define Global_Reg_CM       0x48005200
#define NEON_CM             0x48005300
#define USBHOST_CM          0x48005400

/* MPU_CM Registers PU subsystem*/
#define CM_CLKEN_PLL_MPU    0x04
#define CM_IDLEST_MPU       0x20
#define CM_IDLEST_PLL_MPU   0x24
#define CM_AUTOIDLE_PLL_MPU 0x34
#define CM_CLKSEL1_PLL_MPU  0x40
#define CM_CLKSEL2_PLL_MPU  0x44
#define CM_CLKSTCTRL_MPU    0x48
#define CM_CLKSTST_MPU      0x4C

/* CORE_CM Registers p. 433, Section 4.14.1.5, Core domain peripherals set*/
#define CM_FCLKEN1_CORE     0x00
#define CM_FCLKEN3_CORE     0x08
#define CM_ICLKEN1_CORE     0x10
#define CM_ICLKEN3_CORE     0x18
#define CM_IDLEST1_CORE     0x20
#define CM_IDLEST3_CORE     0x28
#define CM_AUTOIDLE1_CORE   0x30
#define CM_AUTOIDLE3_CORE   0x38
#define CM_CLKSEL_CORE      0x40
#define CM_CLKSTCTRL_CORE   0x48
#define CM_CLKSTST_CORE     0x4C

/* PER_CM Registers p.498, Section 4.14.1.11, used for peripheral domain peripherals set*/
#define CM_FCLKEN_PER       0x00
#define CM_ICLKEN_PER       0x10
#define CM_IDLEST_PER       0x20
#define CM_AUTOIDLE_PER     0x30
#define CM_CLKSEL_PER       0x40        // p.497 Used to select clock source for timer e.g.; Bit 0 is for GPTIMER2, 0x0 for 32k clock and 0x1 for SYSCLK
#define CM_SLEEPDEP_PER     0x44
#define CM_CLKSTCTRL_PER    0x48
#define CM_CLKSTST_PER      0x4C


#endif /* INCLUDES_OMAP3530CLOCK_H_ */
