/* 
 * 
 *
 * Copyright (c) 2006 Hisilicon Co., Ltd. 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 * 
 * History: 
 *      10-April-2006 create this file
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/config.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

#include <linux/proc_fs.h>
#include <linux/poll.h>

#include <asm/hardware.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>

#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/kcom.h>
#include <kcom/gpio_i2c.h>
#include "hi3120.h"

#define I2C_HI3120 0xA0

unsigned char reset[]=
{
0xff,0x00,0x10,0x0f,0x0f,0x10,0x0f,0x00,
0x88,0x10,0x87,0x80,0x86,0x80,
0x80,0x00,0x81,0x00,0x82,0x0c, 
/*
0xff, 0x00,
0x00,0x06, 
0x01,0x00,
0x03,0x08,
0x04,0x00,

0xff, 0x00,
0x06, 0x62,
0x07, 0x1f,
*/
0xff,0x00,0x00,0x06,0x01,0x00,0x03,0x08,0x04,0x00,0xff,0x00,0x06,0x62,0x07,0x1f, 

0xff,0x02,0xb0,0x02,
0xb1,0x00,0xb2,0x00,0xb3,0xff,0xb4,0x00,0xb5,0x00,
0xb1,0x04,0xb2,0x01,0xb3,0xfd,0xb4,0x04,0xb5,0x01,
0xb1,0x08,0xb2,0x03,0xb3,0xfb,0xb4,0x08,0xb5,0x01,
0xb1,0x0c,0xb2,0x04,0xb3,0xf8,0xb4,0x0c,0xb5,0x01,
0xb1,0x10,0xb2,0x04,0xb3,0xf6,0xb4,0x10,0xb5,0x03,
0xb1,0x14,0xb2,0x07,0xb3,0xf4,0xb4,0x15,0xb5,0x03,
0xb1,0x18,0xb2,0x07,0xb3,0xf1,0xb4,0x19,0xb5,0x04,
0xb1,0x1c,0xb2,0x07,0xb3,0xee,0xb4,0x1d,0xb5,0x05,
0xb1,0x20,0xb2,0x09,0xb3,0xeb,0xb4,0x22,0xb5,0x05,
0xb1,0x24,0xb2,0x0a,0xb3,0xe8,0xb4,0x26,0xb5,0x05,
0xb1,0x28,0xb2,0x0a,0xb3,0xe5,0xb4,0x2a,0xb5,0x06,
0xb1,0x2c,0xb2,0x0b,0xb3,0xe2,0xb4,0x2f,0xb5,0x07,
0xb1,0x30,0xb2,0x0c,0xb3,0xe0,0xb4,0x33,0xb5,0x08,
0xb1,0x34,0xb2,0x0c,0xb3,0xdc,0xb4,0x37,0xb5,0x08,
0xb1,0x38,0xb2,0x0d,0xb3,0xd8,0xb4,0x3d,0xb5,0x09,
0xb1,0x3c,0xb2,0x0d,0xb3,0xd4,0xb4,0x41,0xb5,0x09,
0xb1,0x40,0xb2,0x0e,0xb3,0xd1,0xb4,0x46,0xb5,0x0a,
0xb1,0x44,0xb2,0x0e,0xb3,0xcd,0xb4,0x4a,0xb5,0x0a,
0xb1,0x48,0xb2,0x0f,0xb3,0xca,0xb4,0x4f,0xb5,0x0b,
0xb1,0x4c,0xb2,0x0f,0xb3,0xc6,0xb4,0x54,0xb5,0x0c,
0xb1,0x50,0xb2,0x0f,0xb3,0xc2,0xb4,0x58,0xb5,0x0c,
0xb1,0x54,0xb2,0x10,0xb3,0xbe,0xb4,0x5d,0xb5,0x0c,
0xb1,0x58,0xb2,0x10,0xb3,0xba,0xb4,0x62,0xb5,0x0d,
0xb1,0x5c,0xb2,0x10,0xb3,0xb6,0xb4,0x66,0xb5,0x0d,
0xb1,0x60,0xb2,0x10,0xb3,0xb2,0xb4,0x6b,0xb5,0x0e,
0xb1,0x64,0xb2,0x10,0xb3,0xae,0xb4,0x6f,0xb5,0x0e,
0xb1,0x68,0xb2,0x10,0xb3,0xa9,0xb4,0x74,0xb5,0x0e,
0xb1,0x6c,0xb2,0x10,0xb3,0xa5,0xb4,0x79,0xb5,0x0f,
0xb1,0x70,0xb2,0x10,0xb3,0xa1,0xb4,0x7d,0xb5,0x0f,
0xb1,0x74,0xb2,0x10,0xb3,0x9d,0xb4,0x81,0xb5,0x0f,
0xb1,0x78,0xb2,0x10,0xb3,0x98,0xb4,0x86,0xb5,0x0f,
0xb1,0x7c,0xb2,0x10,0xb3,0x94,0xb4,0x8b,0xb5,0x10,
0xb1,0x80,0xb2,0x0f,0xb3,0x8f,0xb4,0x8f,0xb5,0x10,
0xb1,0x84,0xb2,0x10,0xb3,0x8b,0xb4,0x94,0xb5,0x10,
0xb1,0x88,0xb2,0x0f,0xb3,0x86,0xb4,0x98,0xb5,0x10,
0xb1,0x8c,0xb2,0x0f,0xb3,0x81,0xb4,0x9d,0xb5,0x10,
0xb1,0x90,0xb2,0x0f,0xb3,0x7d,0xb4,0xa1,0xb5,0x10,
0xb1,0x94,0xb2,0x0f,0xb3,0x79,0xb4,0xa5,0xb5,0x10,
0xb1,0x98,0xb2,0x0e,0xb3,0x74,0xb4,0xa9,0xb5,0x10,
0xb1,0x9c,0xb2,0x0e,0xb3,0x6f,0xb4,0xae,0xb5,0x10,
0xb1,0xa0,0xb2,0x0e,0xb3,0x6b,0xb4,0xb2,0xb5,0x10,
0xb1,0xa4,0xb2,0x0d,0xb3,0x66,0xb4,0xb6,0xb5,0x10,
0xb1,0xa8,0xb2,0x0d,0xb3,0x62,0xb4,0xba,0xb5,0x10,
0xb1,0xac,0xb2,0x0c,0xb3,0x5d,0xb4,0xbe,0xb5,0x10,
0xb1,0xb0,0xb2,0x0c,0xb3,0x58,0xb4,0xc2,0xb5,0x0f,
0xb1,0xb4,0xb2,0x0c,0xb3,0x54,0xb4,0xc6,0xb5,0x0f,
0xb1,0xb8,0xb2,0x0b,0xb3,0x4f,0xb4,0xca,0xb5,0x0f,
0xb1,0xbc,0xb2,0x0a,0xb3,0x4a,0xb4,0xcd,0xb5,0x0e,
0xb1,0xc0,0xb2,0x0a,0xb3,0x46,0xb4,0xd1,0xb5,0x0e,
0xb1,0xc4,0xb2,0x09,0xb3,0x41,0xb4,0xd4,0xb5,0x0d,
0xb1,0xc8,0xb2,0x09,0xb3,0x3d,0xb4,0xd8,0xb5,0x0d,
0xb1,0xcc,0xb2,0x08,0xb3,0x37,0xb4,0xdc,0xb5,0x0c,
0xb1,0xd0,0xb2,0x08,0xb3,0x33,0xb4,0xe0,0xb5,0x0c,
0xb1,0xd4,0xb2,0x07,0xb3,0x2f,0xb4,0xe2,0xb5,0x0b,
0xb1,0xd8,0xb2,0x06,0xb3,0x2a,0xb4,0xe5,0xb5,0x0a,
0xb1,0xdc,0xb2,0x05,0xb3,0x26,0xb4,0xe8,0xb5,0x0a,
0xb1,0xe0,0xb2,0x05,0xb3,0x22,0xb4,0xeb,0xb5,0x09,
0xb1,0xe4,0xb2,0x05,0xb3,0x1d,0xb4,0xee,0xb5,0x07,
0xb1,0xe8,0xb2,0x04,0xb3,0x19,0xb4,0xf1,0xb5,0x07,
0xb1,0xec,0xb2,0x03,0xb3,0x15,0xb4,0xf4,0xb5,0x07,
0xb1,0xf0,0xb2,0x03,0xb3,0x10,0xb4,0xf6,0xb5,0x04,
0xb1,0xf4,0xb2,0x01,0xb3,0x0c,0xb4,0xf8,0xb5,0x04,
0xb1,0xf8,0xb2,0x01,0xb3,0x08,0xb4,0xfb,0xb5,0x03,
0xb1,0xfc,0xb2,0x01,0xb3,0x04,0xb4,0xfd,0xb5,0x01,
0xb0,0x03,
0xb1,0x00,0xb2,0x00,0xb3,0xff,0xb4,0x00,0xb5,0x00,
0xb1,0x04,0xb2,0x01,0xb3,0xfd,0xb4,0x04,0xb5,0x01,
0xb1,0x08,0xb2,0x03,0xb3,0xfb,0xb4,0x08,0xb5,0x01,
0xb1,0x0c,0xb2,0x04,0xb3,0xf8,0xb4,0x0c,0xb5,0x01,
0xb1,0x10,0xb2,0x04,0xb3,0xf6,0xb4,0x10,0xb5,0x03,
0xb1,0x14,0xb2,0x07,0xb3,0xf4,0xb4,0x15,0xb5,0x03,
0xb1,0x18,0xb2,0x07,0xb3,0xf1,0xb4,0x19,0xb5,0x04,
0xb1,0x1c,0xb2,0x07,0xb3,0xee,0xb4,0x1d,0xb5,0x05,
0xb1,0x20,0xb2,0x09,0xb3,0xeb,0xb4,0x22,0xb5,0x05,
0xb1,0x24,0xb2,0x0a,0xb3,0xe8,0xb4,0x26,0xb5,0x05,
0xb1,0x28,0xb2,0x0a,0xb3,0xe5,0xb4,0x2a,0xb5,0x06,
0xb1,0x2c,0xb2,0x0b,0xb3,0xe2,0xb4,0x2f,0xb5,0x07,
0xb1,0x30,0xb2,0x0c,0xb3,0xe0,0xb4,0x33,0xb5,0x08,
0xb1,0x34,0xb2,0x0c,0xb3,0xdc,0xb4,0x37,0xb5,0x08,
0xb1,0x38,0xb2,0x0d,0xb3,0xd8,0xb4,0x3d,0xb5,0x09,
0xb1,0x3c,0xb2,0x0d,0xb3,0xd4,0xb4,0x41,0xb5,0x09,
0xb1,0x40,0xb2,0x0e,0xb3,0xd1,0xb4,0x46,0xb5,0x0a,
0xb1,0x44,0xb2,0x0e,0xb3,0xcd,0xb4,0x4a,0xb5,0x0a,
0xb1,0x48,0xb2,0x0f,0xb3,0xca,0xb4,0x4f,0xb5,0x0b,
0xb1,0x4c,0xb2,0x0f,0xb3,0xc6,0xb4,0x54,0xb5,0x0c,
0xb1,0x50,0xb2,0x0f,0xb3,0xc2,0xb4,0x58,0xb5,0x0c,
0xb1,0x54,0xb2,0x10,0xb3,0xbe,0xb4,0x5d,0xb5,0x0c,
0xb1,0x58,0xb2,0x10,0xb3,0xba,0xb4,0x62,0xb5,0x0d,
0xb1,0x5c,0xb2,0x10,0xb3,0xb6,0xb4,0x66,0xb5,0x0d,
0xb1,0x60,0xb2,0x10,0xb3,0xb2,0xb4,0x6b,0xb5,0x0e,
0xb1,0x64,0xb2,0x10,0xb3,0xae,0xb4,0x6f,0xb5,0x0e,
0xb1,0x68,0xb2,0x10,0xb3,0xa9,0xb4,0x74,0xb5,0x0e,
0xb1,0x6c,0xb2,0x10,0xb3,0xa5,0xb4,0x79,0xb5,0x0f,
0xb1,0x70,0xb2,0x10,0xb3,0xa1,0xb4,0x7d,0xb5,0x0f,
0xb1,0x74,0xb2,0x10,0xb3,0x9d,0xb4,0x81,0xb5,0x0f,
0xb1,0x78,0xb2,0x10,0xb3,0x98,0xb4,0x86,0xb5,0x0f,
0xb1,0x7c,0xb2,0x10,0xb3,0x94,0xb4,0x8b,0xb5,0x10,
0xb1,0x80,0xb2,0x0f,0xb3,0x8f,0xb4,0x8f,0xb5,0x10,
0xb1,0x84,0xb2,0x10,0xb3,0x8b,0xb4,0x94,0xb5,0x10,
0xb1,0x88,0xb2,0x0f,0xb3,0x86,0xb4,0x98,0xb5,0x10,
0xb1,0x8c,0xb2,0x0f,0xb3,0x81,0xb4,0x9d,0xb5,0x10,
0xb1,0x90,0xb2,0x0f,0xb3,0x7d,0xb4,0xa1,0xb5,0x10,
0xb1,0x94,0xb2,0x0f,0xb3,0x79,0xb4,0xa5,0xb5,0x10,
0xb1,0x98,0xb2,0x0e,0xb3,0x74,0xb4,0xa9,0xb5,0x10,
0xb1,0x9c,0xb2,0x0e,0xb3,0x6f,0xb4,0xae,0xb5,0x10,
0xb1,0xa0,0xb2,0x0e,0xb3,0x6b,0xb4,0xb2,0xb5,0x10,
0xb1,0xa4,0xb2,0x0d,0xb3,0x66,0xb4,0xb6,0xb5,0x10,
0xb1,0xa8,0xb2,0x0d,0xb3,0x62,0xb4,0xba,0xb5,0x10,
0xb1,0xac,0xb2,0x0c,0xb3,0x5d,0xb4,0xbe,0xb5,0x10,
0xb1,0xb0,0xb2,0x0c,0xb3,0x58,0xb4,0xc2,0xb5,0x0f,
0xb1,0xb4,0xb2,0x0c,0xb3,0x54,0xb4,0xc6,0xb5,0x0f,
0xb1,0xb8,0xb2,0x0b,0xb3,0x4f,0xb4,0xca,0xb5,0x0f,
0xb1,0xbc,0xb2,0x0a,0xb3,0x4a,0xb4,0xcd,0xb5,0x0e,
0xb1,0xc0,0xb2,0x0a,0xb3,0x46,0xb4,0xd1,0xb5,0x0e,
0xb1,0xc4,0xb2,0x09,0xb3,0x41,0xb4,0xd4,0xb5,0x0d,
0xb1,0xc8,0xb2,0x09,0xb3,0x3d,0xb4,0xd8,0xb5,0x0d,
0xb1,0xcc,0xb2,0x08,0xb3,0x37,0xb4,0xdc,0xb5,0x0c,
0xb1,0xd0,0xb2,0x08,0xb3,0x33,0xb4,0xe0,0xb5,0x0c,
0xb1,0xd4,0xb2,0x07,0xb3,0x2f,0xb4,0xe2,0xb5,0x0b,
0xb1,0xd8,0xb2,0x06,0xb3,0x2a,0xb4,0xe5,0xb5,0x0a,
0xb1,0xdc,0xb2,0x05,0xb3,0x26,0xb4,0xe8,0xb5,0x0a,
0xb1,0xe0,0xb2,0x05,0xb3,0x22,0xb4,0xeb,0xb5,0x09,
0xb1,0xe4,0xb2,0x05,0xb3,0x1d,0xb4,0xee,0xb5,0x07,
0xb1,0xe8,0xb2,0x04,0xb3,0x19,0xb4,0xf1,0xb5,0x07,
0xb1,0xec,0xb2,0x03,0xb3,0x15,0xb4,0xf4,0xb5,0x07,
0xb1,0xf0,0xb2,0x03,0xb3,0x10,0xb4,0xf6,0xb5,0x04,
0xb1,0xf4,0xb2,0x01,0xb3,0x0c,0xb4,0xf8,0xb5,0x04,
0xb1,0xf8,0xb2,0x01,0xb3,0x08,0xb4,0xfb,0xb5,0x03,
0xb1,0xfc,0xb2,0x01,0xb3,0x04,0xb4,0xfd,0xb5,0x01,

0xff,0x00,0x10,0x00,0xff,0x00,
0xff,0x01,0x8c,0xfc,0x90,0x40,0x91,0x00,0x92,0x00,0x93,0x01,0x94,0x40,0x95,0x00,0x96,0x00,
0x97,0x01,0x98,0xff,0x99,0x00,0x9a,0x00,0x9b,0x00,

0xa1,0x00,0xa0,0x80,0xa3,0x03,0xa2,0x00,0xa5,0x01,0xa4,0x33,0xa7,0x01,0xa6,0x00,0xa8,0x80, 

0xff,0x01,0x0b,0x00,0x0a,0x81,0x0d,0x00,0x0c,0x19,0x0f,0x00,0x0e,0x42,0x11,0x00,0x10,0x10,
0x13,0x02,0x12,0x4a,0x15,0x00,0x14,0x70,0x17,0x02,0x16,0x26,0x19,0x00,0x18,0x80,0x1b,0x02,
0x1a,0x5e,0x1d,0x02,0x1c,0x12,0x1f,0x00,0x1e,0x70,0x21,0x00,0x20,0x80,

0xff,0x00,0x26,0x68,0x27,0x01,0x28,0x00,0x29,0x21,0x2a,0x43,0x2b,0x65,0x2c,0x87,0x2d,0xa9,
0x2e,0xcb,0x2f,0xed,0x30,0xff,  

0xff,0x00,0x45,0x20,0x46,0x20,0x48,0x0a,0x4a,0x00,0x4e,0x00,0x4f,0x00,0x50,0x00,0x51,0x00,
0x52,0xc0,0x53,0x00,0x54,0x00,0x55,0x00,0x56,0xc0, 0x57,0x00,0x58,0x00,0x59,0x00,0x5a,0x00,
0x5b,0x00,0x5c,0x0f,0x5d,0x05,0x5e,0x00,0x5f,0x00,0x60,0x00,0x61,0x00,0x62,0x0f,0x63,0x05,0x4b,0x03,  

0xec,0x22,0xed,0x05, 

0x89,0x44, 

};

unsigned char gamma[]=
{
0x8a,0x00,0x8b,0x00,0x8a,0x08,0x8b,0x00,0x8a,0x10,0x8b,0x00,0x8a,0x1c,0x8b,0x00,
0x8a,0x24,0x8b,0x00,0x8a,0x30,0x8b,0x00,0x8a,0x3c,0x8b,0x00,0x8a,0x48,0x8b,0x00,
0x8a,0x54,0x8b,0x00,0x8a,0x64,0x8b,0x00,0x8a,0x70,0x8b,0x00,0x8a,0x7c,0x8b,0x00,
0x8a,0x8c,0x8b,0x00,0x8a,0x98,0x8b,0x00,0x8a,0xa8,0x8b,0x00,0x8a,0xb8,0x8b,0x00,
0x8a,0xc4,0x8b,0x00,0x8a,0xd4,0x8b,0x00,0x8a,0xe4,0x8b,0x00,0x8a,0xf4,0x8b,0x00,
0x8a,0x00,0x8b,0x01,0x8a,0x10,0x8b,0x01,0x8a,0x20,0x8b,0x01,0x8a,0x30,0x8b,0x01,
0x8a,0x40,0x8b,0x01,0x8a,0x50,0x8b,0x01,0x8a,0x60,0x8b,0x01,0x8a,0x70,0x8b,0x01,
0x8a,0x80,0x8b,0x01,0x8a,0x94,0x8b,0x01,0x8a,0xa4,0x8b,0x01,0x8a,0xb4,0x8b,0x01,
0x8a,0xc4,0x8b,0x01,0x8a,0xd4,0x8b,0x01,0x8a,0xe8,0x8b,0x01,0x8a,0xf8,0x8b,0x01,
0x8a,0x08,0x8b,0x02,0x8a,0x1c,0x8b,0x02,0x8a,0x2c,0x8b,0x02,0x8a,0x3c,0x8b,0x02,
0x8a,0x50,0x8b,0x02,0x8a,0x60,0x8b,0x02,0x8a,0x74,0x8b,0x02,0x8a,0x84,0x8b,0x02,
0x8a,0x98,0x8b,0x02,0x8a,0xa8,0x8b,0x02,0x8a,0xbc,0x8b,0x02,0x8a,0xcc,0x8b,0x02,
0x8a,0xe0,0x8b,0x02,0x8a,0xf4,0x8b,0x02,0x8a,0x04,0x8b,0x03,0x8a,0x18,0x8b,0x03,
0x8a,0x2c,0x8b,0x03,0x8a,0x3c,0x8b,0x03,0x8a,0x50,0x8b,0x03,0x8a,0x64,0x8b,0x03,
0x8a,0x74,0x8b,0x03,0x8a,0x88,0x8b,0x03,0x8a,0x9c,0x8b,0x03,0x8a,0xb0,0x8b,0x03,
0x8a,0xc0,0x8b,0x03,0x8a,0xd4,0x8b,0x03,0x8a,0xe8,0x8b,0x03,0x8a,0xfc,0x8b,0x03,
0x8a,0x00,0x8b,0x00,0x8a,0x08,0x8b,0x00,0x8a,0x10,0x8b,0x00,0x8a,0x1c,0x8b,0x00,
0x8a,0x24,0x8b,0x00,0x8a,0x30,0x8b,0x00,0x8a,0x3c,0x8b,0x00,0x8a,0x48,0x8b,0x00,
0x8a,0x54,0x8b,0x00,0x8a,0x64,0x8b,0x00,0x8a,0x70,0x8b,0x00,0x8a,0x7c,0x8b,0x00,
0x8a,0x8c,0x8b,0x00,0x8a,0x98,0x8b,0x00,0x8a,0xa8,0x8b,0x00,0x8a,0xb8,0x8b,0x00,
0x8a,0xc4,0x8b,0x00,0x8a,0xd4,0x8b,0x00,0x8a,0xe4,0x8b,0x00,0x8a,0xf4,0x8b,0x00,
0x8a,0x00,0x8b,0x01,0x8a,0x10,0x8b,0x01,0x8a,0x20,0x8b,0x01,0x8a,0x30,0x8b,0x01,
0x8a,0x40,0x8b,0x01,0x8a,0x50,0x8b,0x01,0x8a,0x60,0x8b,0x01,0x8a,0x70,0x8b,0x01,
0x8a,0x80,0x8b,0x01,0x8a,0x94,0x8b,0x01,0x8a,0xa4,0x8b,0x01,0x8a,0xb4,0x8b,0x01,
0x8a,0xc4,0x8b,0x01,0x8a,0xd4,0x8b,0x01,0x8a,0xe8,0x8b,0x01,0x8a,0xf8,0x8b,0x01,
0x8a,0x08,0x8b,0x02,0x8a,0x1c,0x8b,0x02,0x8a,0x2c,0x8b,0x02,0x8a,0x3c,0x8b,0x02,
0x8a,0x50,0x8b,0x02,0x8a,0x60,0x8b,0x02,0x8a,0x74,0x8b,0x02,0x8a,0x84,0x8b,0x02,
0x8a,0x98,0x8b,0x02,0x8a,0xa8,0x8b,0x02,0x8a,0xbc,0x8b,0x02,0x8a,0xcc,0x8b,0x02,
0x8a,0xe0,0x8b,0x02,0x8a,0xf4,0x8b,0x02,0x8a,0x04,0x8b,0x03,0x8a,0x18,0x8b,0x03,
0x8a,0x2c,0x8b,0x03,0x8a,0x3c,0x8b,0x03,0x8a,0x50,0x8b,0x03,0x8a,0x64,0x8b,0x03,
0x8a,0x74,0x8b,0x03,0x8a,0x88,0x8b,0x03,0x8a,0x9c,0x8b,0x03,0x8a,0xb0,0x8b,0x03,
0x8a,0xc0,0x8b,0x03,0x8a,0xd4,0x8b,0x03,0x8a,0xe8,0x8b,0x03,0x8a,0xfc,0x8b,0x03,
0x8a,0x00,0x8b,0x00,0x8a,0x08,0x8b,0x00,0x8a,0x10,0x8b,0x00,0x8a,0x1c,0x8b,0x00,
0x8a,0x24,0x8b,0x00,0x8a,0x30,0x8b,0x00,0x8a,0x3c,0x8b,0x00,0x8a,0x48,0x8b,0x00,
0x8a,0x54,0x8b,0x00,0x8a,0x64,0x8b,0x00,0x8a,0x70,0x8b,0x00,0x8a,0x7c,0x8b,0x00,
0x8a,0x8c,0x8b,0x00,0x8a,0x98,0x8b,0x00,0x8a,0xa8,0x8b,0x00,0x8a,0xb8,0x8b,0x00,
0x8a,0xc4,0x8b,0x00,0x8a,0xd4,0x8b,0x00,0x8a,0xe4,0x8b,0x00,0x8a,0xf4,0x8b,0x00,
0x8a,0x00,0x8b,0x01,0x8a,0x10,0x8b,0x01,0x8a,0x20,0x8b,0x01,0x8a,0x30,0x8b,0x01,
0x8a,0x40,0x8b,0x01,0x8a,0x50,0x8b,0x01,0x8a,0x60,0x8b,0x01,0x8a,0x70,0x8b,0x01,
0x8a,0x80,0x8b,0x01,0x8a,0x94,0x8b,0x01,0x8a,0xa4,0x8b,0x01,0x8a,0xb4,0x8b,0x01,
0x8a,0xc4,0x8b,0x01,0x8a,0xd4,0x8b,0x01,0x8a,0xe8,0x8b,0x01,0x8a,0xf8,0x8b,0x01,
0x8a,0x08,0x8b,0x02,0x8a,0x1c,0x8b,0x02,0x8a,0x2c,0x8b,0x02,0x8a,0x3c,0x8b,0x02,
0x8a,0x50,0x8b,0x02,0x8a,0x60,0x8b,0x02,0x8a,0x74,0x8b,0x02,0x8a,0x84,0x8b,0x02,
0x8a,0x98,0x8b,0x02,0x8a,0xa8,0x8b,0x02,0x8a,0xbc,0x8b,0x02,0x8a,0xcc,0x8b,0x02,
0x8a,0xe0,0x8b,0x02,0x8a,0xf4,0x8b,0x02,0x8a,0x04,0x8b,0x03,0x8a,0x18,0x8b,0x03,
0x8a,0x2c,0x8b,0x03,0x8a,0x3c,0x8b,0x03,0x8a,0x50,0x8b,0x03,0x8a,0x64,0x8b,0x03,
0x8a,0x74,0x8b,0x03,0x8a,0x88,0x8b,0x03,0x8a,0x9c,0x8b,0x03,0x8a,0xb0,0x8b,0x03,
0x8a,0xc0,0x8b,0x03,0x8a,0xd4,0x8b,0x03,0x8a,0xe8,0x8b,0x03,0x8a,0xfc,0x8b,0x03,
};

unsigned char PalTo800600[]=
{
//芯片复位 （4）
//复位各个子模块
0xff,0x00,0x10,0x0f,0x0f,0x10,0x0f,0x00,
//时钟设置 （8）
0xff,0x00,0x00,0x06,0x01,0x00,0x03,0x08,0x04,0x00,0xff,0x00,0x06,0x0b,0x07,0x06,

//VO输出格式（4）
0xff,0x01,0x80,0x00,0x81,0x40,0x82,0x0c,

//VO窗口 （20）
0xd1,0x04,0xd0,0x20,0xd3,0x02,0xd2,0x74,0xd5,0x00,0xd4,0xd8,0xd7,0x03,0xd6,0xf8,0xd9,0x00,0xd8,0x1b,0xdb,0x02,0xda,0x73,
0xdd,0x00,0xdc,0xd8,0xdf,0x03,0xde,0xf8,0xe1,0x00,0xe0,0x1b,0xe3,0x02,0xe2,0x73,

//VI时钟复位 （5）
0xff,0x00,0x10,0x0e,0x0f,0x00,0x0f,0x00,
//时钟设置 （8）
0xff,0x00,0x00,0x06,0x01,0x00,0x03,0x08,0x04,0x00,0xff,0x00,0x06,0x0b,0x07,0x06,

//视频格式发生变化 flag bit = 1 (2)
0xff,0x01,0x82,0x2c,

//输入捕捉窗口，格式（10）
0x00,0x92,0x01,0x10,0x03,0x00,0x02,0x22,0x05,0x02,0x04,0xaa,0x07,0x00,0x06,0x07,0x09,0x01,0x08,0x19,

//DNR （6）
0xff,0x00,0x20,0x00,0x21,0x24,0x22,0x02,0x23,0x88,0x24,0x02,

//SDRAM 控制（16）
0x80,0x24,0x81,0x02,0x82,0x88,0x83,0x02,0x84,0x24,0x85,0x02,0x86,0x88,0x87,0x02,0x88,0x33,0x89,0x00,0x8a,0x07,0x8b,0x67,0x8c,0x2b,0x8d,0x64,0x8e,0x05,0x8f,0x00,

//ITOP 参数 （5）
0x40,0x00,0x42,0x02,0x41,0x88,0x44,0x01,0x43,0x12,

//SCALE 参数（53）
0xff,0x02,0x83,0xff,0x84,0xf4,0x85,0x0c,0x86,0xf4,0x87,0x0c,0x88,0xf4,0x89,0x0c,0x8a,0xf4,0x8b,0x0c,0x8c,0xf4,0x8d,0x0c,0x8e,0x9f,
0x8f,0x00,0x90,0x3f,0x91,0x01,0x92,0xdf,0x93,0x01,0x94,0x7f,0x95,0x02,0x96,0x1e,0x97,0x03,0x98,0x87,0x99,0x02,0x9a,0x9c,0x9b,0x0e,0x9c,0x9c,0x9d,0x0e,0x9e,0x9c,0x9f,0x0e,0xa0,0x9c,0xa1,0x0e,
0xa2,0x9c,0xa3,0x0e,0xa4,0x77,0xa5,0x00,0xa6,0xef,0xa7,0x00,0xa8,0x67,0xa9,0x01,0xaa,0xdf,0xab,0x01,0xac,0x57,0xad,0x02,0xae,0x23,0xaf,0x02,0xbf,0xff,0xc0,0x00,0xc1,0x00,0xc2,0x00,0xc3,0x00,
0xc4,0x00,0xc5,0x00,

//时钟复位（4）
0xff,0x00,0xff,0x00,0x10,0x00,0xff,0x00,

//视频格式保持不变 flag bit = 0 (3)
0xff,0x01,0x01,0x12,0x82,0x0c,


};

unsigned char NtscTo800600[]=
{
//芯片复位 （4）
//复位各个子模块
0xff,0x00,0x10,0x0f,0x0f,0x10,0x0f,0x00,
//时钟设置 （8）
0xff,0x00,0x00,0x06,0x01,0x00,0x03,0x08,0x04,0x00,0xff,0x00,0x06,0x03,0x07,0x01,

//VO输出格式（4）
0xff,0x01,0x80,0x00,0x81,0x00,0x82,0x0c,

//VO窗口 （20）
0xd1,0x04,0xd0,0x20,0xd3,0x02,0xd2,0x74,0xd5,0x00,0xd4,0xd8,0xd7,0x03,0xd6,0xf8,0xd9,0x00,0xd8,0x1b,0xdb,0x02,0xda,0x73,
0xdd,0x00,0xdc,0xd8,0xdf,0x03,0xde,0xf8,0xe1,0x00,0xe0,0x1b,0xe3,0x02,0xe2,0x73,

//VI时钟复位 （5）
0xff,0x00,0x10,0x0e,0x0f,0x00,0x0f,0x00,
//时钟设置 （8）
0xff,0x00,0x00,0x06,0x01,0x00,0x03,0x08,0x04,0x00,0xff,0x00,0x06,0x03,0x07,0x01,

//视频格式发生变化 flag bit = 1 (2)
0xff,0x01,0x82,0x2c,

//输入捕捉窗口，格式（10）
0x00,0x92,0x01,0x10,0x03,0x00,0x02,0x14,0x05,0x02,0x04,0xb6,0x07,0x00,0x06,0x10,0x09,0x00,0x08,0xf8,

//DNR （6）
0xff,0x00,0x20,0x00,0x21,0xd0,0x22,0x01,0x23,0xa2,0x24,0x02,

//SDRAM 控制（16）
0x80,0xd0,0x81,0x01,0x82,0xa2,0x83,0x02,0x84,0xd0,0x85,0x01,0x86,0xa2,0x87,0x02,0x88,0x33,0x89,0x00,0x8a,0x07,0x8b,0x67,0x8c,0x2b,0x8d,0x64,0x8e,0x05,0x8f,0x00,

//ITOP 参数 （5）
0x40,0x00,0x42,0x02,0x41,0xa2,0x44,0x00,0x43,0xe8,

//SCALE 参数（53）
0xff,0x02,0x83,0xff,0x84,0x7a,0x85,0x0d,0x86,0x7a,0x87,0x0d,0x88,0x7a,0x89,0x0d,0x8a,0x7a,0x8b,0x0d,0x8c,0x7a,0x8d,0x0d,0x8e,0x9f,
0x8f,0x00,0x90,0x3f,0x91,0x01,0x92,0xdf,0x93,0x01,0x94,0x7f,0x95,0x02,0x96,0x1e,0x97,0x03,0x98,0xa1,0x99,0x02,0x9a,0x5e,0x9b,0x0c,0x9c,0x5e,0x9d,0x0c,0x9e,0x5e,0x9f,0x0c,0xa0,0x5e,0xa1,0x0c,
0xa2,0x5e,0xa3,0x0c,0xa4,0x77,0xa5,0x00,0xa6,0xef,0xa7,0x00,0xa8,0x67,0xa9,0x01,0xaa,0xdf,0xab,0x01,0xac,0x57,0xad,0x02,0xae,0xcf,0xaf,0x01,0xbf,0xff,0xc0,0x00,0xc1,0x00,0xc2,0x00,0xc3,0x00,
0xc4,0x00,0xc5,0x00,

//时钟复位（4）
0xff,0x00,0xff,0x00,0x10,0x00,0xff,0x00,

//视频格式保持不变 flag bit = 0 (3)
0xff,0x01,0x01,0x12,0x82,0x0c,
};

unsigned char RGB800600To800600[]=
{
//芯片复位 （4）
//复位各个子模块
0xff,0x00,0x10,0x0f,0x0f,0x10,0x0f,0x00,
//时钟设置 （8）
0xff,0x00,0x00,0x06,0x01,0x00,0x03,0x08,0x04,0x00,0xff,0x00,0x06,0x3d,0x07,0x51,

//VO输出格式（4）
0xff,0x01,0x80,0x00,0x81,0x00,0x82,0x0c,

//VO窗口 （20）
0xd1,0x04,0xd0,0x20,0xd3,0x02,0xd2,0x74,0xd5,0x00,0xd4,0xd8,0xd7,0x03,0xd6,0xf8,0xd9,0x00,0xd8,0x1b,0xdb,0x02,0xda,0x73,
0xdd,0x00,0xdc,0xd8,0xdf,0x03,0xde,0xf8,0xe1,0x00,0xe0,0x1b,0xe3,0x02,0xe2,0x73,

//VI时钟复位 （5）
0xff,0x00,0x10,0x0e,0x0f,0x00,0x0f,0x00,
//时钟设置 （8）
0xff,0x00,0x00,0x06,0x01,0x00,0x03,0x08,0x04,0x00,0xff,0x00,0x06,0x3d,0x07,0x51,

//视频格式发生变化 flag bit = 1 (2)
0xff,0x01,0x82,0x2c,

//输入捕捉窗口，格式（10）
0x00,0x0c,0x01,0xc0,0x03,0x00,0x02,0xd6,0x05,0x03,0x04,0xf6,0x07,0x00,0x06,0x1b,0x09,0x02,0x08,0x73,

//DNR （6）
0xff,0x00,0x20,0x00,0x21,0x58,0x22,0x02,0x23,0x20,0x24,0x03,

//SDRAM 控制（16）
0x80,0x58,0x81,0x02,0x82,0x20,0x83,0x03,0x84,0x58,0x85,0x02,0x86,0x20,0x87,0x03,0x88,0x33,0x89,0x0a,0x8a,0x07,0x8b,0x67,0x8c,0x2b,0x8d,0x64,0x8e,0x05,0x8f,0x00,

//ITOP 参数 （5）
0x40,0x05,0x42,0x03,0x41,0x20,0x44,0x02,0x43,0x58,

//SCALE 参数（53）
0xff,0x02,0x83,0xff,0x84,0x00,0x85,0x10,0x86,0x00,0x87,0x10,0x88,0x00,0x89,0x10,0x8a,0x00,0x8b,0x10,0x8c,0x00,0x8d,0x10,0x8e,0x9f,
0x8f,0x00,0x90,0x3f,0x91,0x01,0x92,0xdf,0x93,0x01,0x94,0x7f,0x95,0x02,0x96,0x1e,0x97,0x03,0x98,0x1f,0x99,0x03,0x9a,0x00,0x9b,0x10,0x9c,0x00,0x9d,0x10,0x9e,0x00,0x9f,0x10,0xa0,0x00,0xa1,0x10,
0xa2,0x00,0xa3,0x10,0xa4,0x77,0xa5,0x00,0xa6,0xef,0xa7,0x00,0xa8,0x67,0xa9,0x01,0xaa,0xdf,0xab,0x01,0xac,0x57,0xad,0x02,0xae,0x57,0xaf,0x02,0xbf,0xff,0xc0,0x00,0xc1,0x00,0xc2,0x00,0xc3,0x00,
0xc4,0x00,0xc5,0x00,

//时钟复位（4）
0xff,0x00,0xff,0x00,0x10,0x00,0xff,0x00,

//视频格式保持不变 flag bit = 0 (3)
0xff,0x01,0x01,0xc2,0x82,0x0c,
};

unsigned char  hw_dnr[4][11] =
{
//26   27   28   29   2a   2b   2c   2d   2e   2f   30
{0x68,0x01,0x00,0xbb,0xbb,0xbb,0xcc,0xcc,0xcd,0xef,0xff},
{0x68,0x01,0x00,0x99,0x99,0x99,0x99,0xab,0xcd,0xef,0xff},
{0x68,0x01,0x00,0x77,0x77,0x77,0x77,0x77,0x88,0x99,0xaa},
{0x68,0x01,0x00,0x65,0x87,0xa9,0xcb,0xed,0xff,0xff,0xff},
};

unsigned char  hw_lti[][2] = 
{
			{0xbb,0xbc},
			{0x00,0x00},
			{0x01,0x00},
			{0x02,0x00},
			{0x03,0x00},
			{0x05,0x00},
			{0x08,0x00},
			{0x0b,0x00},
			{0x10,0x00},
			{0x1f,0x00},
};

unsigned char  hw_cti[10][2] =
{
			{0xbd,0xbe},
			{0x00,0x00},
			{0x01,0x00},
			{0x02,0x00},
			{0x04,0x00},
			{0x08,0x00},
			{0x0b,0x00},
			{0x10,0x00},
			{0x18,0x00},
			{0x1f,0x00},
};

unsigned char hw_hue[] = 
{
/* 
cos		  sin
*/	
0x00,0x7f,0x00,0xdb,
0x00,0x84,0x00,0xda,
0x00,0x84,0x00,0xd9,
0x00,0x84,0x00,0xd8,
0x00,0x88,0x00,0xd7,
0x00,0x84,0x00,0xd6,
0x00,0x84,0x00,0xd5,
0x00,0x8d,0x00,0xd4,
0x00,0x8d,0x00,0xd3,
0x00,0x91,0x00,0xd1,
0x00,0x95,0x00,0xce,
0x00,0x9a,0x00,0xcb,
0x00,0x9e,0x00,0xc7,
0x00,0xa2,0x00,0xc4,
0x00,0xa6,0x00,0xc1,
0x00,0xaa,0x00,0xbd,
0x00,0xae,0x00,0xb9,
0x00,0xb2,0x00,0xb6,
0x00,0xb6,0x00,0xb2,
0x00,0xb9,0x00,0xae,
0x00,0xbd,0x00,0xaa,
0x00,0xc1,0x00,0xa6,
0x00,0xc4,0x00,0xa2,
0x00,0xc7,0x00,0x9e,
0x00,0xcb,0x00,0x9a,
0x00,0xce,0x00,0x95,
0x00,0xd1,0x00,0x91,
0x00,0xd4,0x00,0x8d,
0x00,0xd7,0x00,0x88,
0x00,0xda,0x00,0x84,
0x00,0xdc,0x00,0x7f,
0x00,0xdf,0x00,0x7a,
0x00,0xe1,0x00,0x76,
0x00,0xe4,0x00,0x71,
0x00,0xe6,0x00,0x6c,
0x00,0xe8,0x00,0x67,
0x00,0xeb,0x00,0x62,
0x00,0xed,0x00,0x5d,
0x00,0xef,0x00,0x58,
0x00,0xf0,0x00,0x53,
0x00,0xf2,0x00,0x4e,
0x00,0xf4,0x00,0x49,
0x00,0xf5,0x00,0x44,
0x00,0xf6,0x00,0x3f,
0x00,0xf8,0x00,0x3a,
0x00,0xf9,0x00,0x35,
0x00,0xfa,0x00,0x2f,
0x00,0xfb,0x00,0x2a,
0x00,0xfc,0x00,0x25,
0x00,0xfc,0x00,0x1f,
0x00,0xfd,0x00,0x1a,
0x00,0xfe,0x00,0x15,
0x00,0xfe,0x00,0x0e,  
0x00,0xfe,0x00,0x0c,  
0x00,0xfe,0x00,0x09,  
0x00,0xfe,0x00,0x08,  
0x00,0xfe,0x00,0x05, 
0x00,0xfe,0x01,0x0a,
0x00,0xfe,0x01,0x10,
0x00,0xfe,0x01,0x15,
0x00,0xfd,0x01,0x1a,
0x00,0xfc,0x01,0x1f,
0x00,0xfc,0x01,0x25,
0x00,0xfb,0x01,0x2a,
0x00,0xfa,0x01,0x2f,
0x00,0xf9,0x01,0x35,
0x00,0xf8,0x01,0x3a,
0x00,0xf6,0x01,0x3f,
0x00,0xf5,0x01,0x44,
0x00,0xf4,0x01,0x49,
0x00,0xf2,0x01,0x4e,
0x00,0xf0,0x01,0x53,
0x00,0xef,0x01,0x58,
0x00,0xed,0x01,0x5d,
0x00,0xeb,0x01,0x62,
0x00,0xe8,0x01,0x67,
0x00,0xe6,0x01,0x6c,
0x00,0xe4,0x01,0x71,
0x00,0xe1,0x01,0x76,
0x00,0xdf,0x01,0x7a,
0x00,0xdc,0x01,0x7f,
0x00,0xda,0x01,0x84,
0x00,0xd7,0x01,0x88,
0x00,0xd4,0x01,0x8d,
0x00,0xd1,0x01,0x91,
0x00,0xce,0x01,0x95,
0x00,0xcb,0x01,0x9a,
0x00,0xc7,0x01,0x9e,
0x00,0xc4,0x01,0xa2,
0x00,0xc1,0x01,0xa6,
0x00,0xbd,0x01,0xaa,
0x00,0xb9,0x01,0xae,
0x00,0xb6,0x01,0xb2,
0x00,0xb2,0x01,0xb6,
0x00,0xae,0x01,0xb9,
0x00,0xaa,0x01,0xbd,
0x00,0xa6,0x01,0xc1,
0x00,0xa2,0x01,0xc4,
0x00,0x9e,0x01,0xc7,
0x00,0xa2,0x01,0xc9,
0x00,0x9e,0x01,0xcb,
};

unsigned char hw3120_init(void)
{
	unsigned int i;
	unsigned char u8RegAddr,u8RegValue;
	for(i =0 ; i<754; i++) 
	{
		u8RegAddr = reset[2*i];
		u8RegValue= reset[2*i+1];
		gpio_i2c_write(I2C_HI3120,u8RegAddr,u8RegValue);
	}
 	return 1;	
}

unsigned char hw3120_gamma(void)
{
	unsigned int i;
	unsigned char u8RegAddr,u8RegValue;
	unsigned char u8RegValueTmp;

	gpio_i2c_write(I2C_HI3120,0xff, 0x01); 			
	u8RegValueTmp = gpio_i2c_read(I2C_HI3120,0x80);
	u8RegValueTmp = u8RegValueTmp|0x08;
	gpio_i2c_write(I2C_HI3120,0x80, u8RegValueTmp); 			// 恢复0x80[3]为1，开始配置Gamma表

	for (i =0 ; i< 384; i++)
	{
		u8RegAddr = gamma[2*i];
		u8RegValue= gamma[2*i+1];
		gpio_i2c_write(I2C_HI3120,u8RegAddr, u8RegValue); 			
	}

	u8RegValueTmp = u8RegValueTmp&0xf7;
	gpio_i2c_write(I2C_HI3120,0x80, u8RegValueTmp); 			// 恢复0x80[3]为0，结束配置Gamma表
 	return 1;
}

unsigned char hw3120_mode(unsigned char * modetb)
{
	unsigned int i;
	unsigned char u8RegAddr,u8RegValue;
	for (i =0 ; i< 147; i++) 
	{
		if(i==2||i==3)
		{
			continue;
		}
		u8RegAddr = modetb[2*i];
		u8RegValue= modetb[2*i+1];
		gpio_i2c_write(I2C_HI3120,u8RegAddr, u8RegValue); 			
	}	
 	return 1;	
} 

void hw3120_gamma_onoff(unsigned char bvalue)
{
	unsigned char value;
	gpio_i2c_write(I2C_HI3120,0xff, 0x01); 	
	value = gpio_i2c_read(I2C_HI3120,0x8c);
	if(bvalue)
	{
		value = value&0xf7;
	}
	else
	{
		value = value|0x08;
	}
	gpio_i2c_write(I2C_HI3120,0x8c,value);
}

unsigned char hw3120_itop(void)
{
    	gpio_i2c_write(I2C_HI3120,0xff,0x00);
	gpio_i2c_write(I2C_HI3120,0x45,0x10);/*水平运动门限判断*/
	gpio_i2c_write(I2C_HI3120,0x46,0x10);/*垂直运动门限判断*/
	gpio_i2c_write(I2C_HI3120,0x48,0x0a);/*亮度中值滤波步长*/
	gpio_i2c_write(I2C_HI3120,0x4a,0x00);/*色度去隔行运动补偿*/
	gpio_i2c_write(I2C_HI3120,0x4e,0x00);/*水平方向相关运动检测所需要像素控制寄存器3*/
	gpio_i2c_write(I2C_HI3120,0x4f,0x00);/*水平方向相关运动检测所需要像素控制寄存器2*/
	gpio_i2c_write(I2C_HI3120,0x50,0x00);/*水平方向相关运动检测所需要像素控制寄存器1*/
	gpio_i2c_write(I2C_HI3120,0x51,0x00);/*水平方向相关运动检测所需要像素控制寄存器0*/
	gpio_i2c_write(I2C_HI3120,0x52,0xc0);/*水平方向相关运动检测所需要像素控制寄存器*/
	gpio_i2c_write(I2C_HI3120,0x53,0x00);/*垂直方向相关运动检测所需要像素控制寄存器2*/
	gpio_i2c_write(I2C_HI3120,0x54,0x00);/*垂直方向相关运动检测所需要像素控制寄存器1*/
	gpio_i2c_write(I2C_HI3120,0x55,0x00);/*垂直方向相关运动检测所需要像素控制寄存器0*/
	gpio_i2c_write(I2C_HI3120,0x56,0xc0);/*垂直方向相关运动检测所需要像素控制寄存器*/
	gpio_i2c_write(I2C_HI3120,0x57,0x00);/*水平方向相关运动噪声容限所需要像素控制寄存器5*/
	gpio_i2c_write(I2C_HI3120,0x58,0x00);/*水平方向相关运动噪声容限所需要像素控制寄存器4*/
	gpio_i2c_write(I2C_HI3120,0x59,0xff);/*水平方向相关运动噪声容限所需要像素控制寄存器3*/
	gpio_i2c_write(I2C_HI3120,0x5a,0xff);/*水平方向相关运动噪声容限所需要像素控制寄存器2*/
	gpio_i2c_write(I2C_HI3120,0x5b,0xff);/*水平方向相关运动噪声容限所需要像素控制寄存器1*/
	gpio_i2c_write(I2C_HI3120,0x5c,0xff);/*水平方向相关运动噪声容限所需要像素控制寄存器0*/
	gpio_i2c_write(I2C_HI3120,0x5d,0x05);/*水平方向相关运动噪声容限门限值寄存器*/
	gpio_i2c_write(I2C_HI3120,0x5e,0x00);/*垂直方向相关运动噪声容限所需要像素控制寄存器4*/
	gpio_i2c_write(I2C_HI3120,0x5f,0x00);/*垂直方向相关运动噪声容限所需要像素控制寄存器3*/
	gpio_i2c_write(I2C_HI3120,0x60,0xff);/*垂直方向相关运动噪声容限所需要像素控制寄存器2*/
	gpio_i2c_write(I2C_HI3120,0x61,0xff);/*垂直方向相关运动噪声容限所需要像素控制寄存器1*/
	gpio_i2c_write(I2C_HI3120,0x62,0xff);/*垂直方向相关运动噪声容限所需要像素控制寄存器0*/
	gpio_i2c_write(I2C_HI3120,0x63,0x05);/*垂直方向相关运动噪声容限门限值寄存器*/
	gpio_i2c_write(I2C_HI3120,0x4b,0x03);/*色度中值滤波的步长大小寄存器*/ 
 	gpio_i2c_write(I2C_HI3120,0xff, 0x01);
	gpio_i2c_write(I2C_HI3120,0x82,0x2c);
	return 0;
}

void hw3120_DNR(unsigned char value)
{
	unsigned char ucValue;
	ucValue = value&0x03;
	gpio_i2c_write(I2C_HI3120,0xff, 0x00);  
	gpio_i2c_write(I2C_HI3120,0x20,0x01);
	gpio_i2c_write(I2C_HI3120,0x26,hw_dnr[ucValue][0]);
	gpio_i2c_write(I2C_HI3120,0x27,hw_dnr[ucValue][1]);
	gpio_i2c_write(I2C_HI3120,0x28,hw_dnr[ucValue][2]);
	gpio_i2c_write(I2C_HI3120,0x29,hw_dnr[ucValue][3]);
	gpio_i2c_write(I2C_HI3120,0x2A,hw_dnr[ucValue][4]);
	gpio_i2c_write(I2C_HI3120,0x2B,hw_dnr[ucValue][5]);
	gpio_i2c_write(I2C_HI3120,0x2C,hw_dnr[ucValue][6]);
	gpio_i2c_write(I2C_HI3120,0x2D,hw_dnr[ucValue][7]);
	gpio_i2c_write(I2C_HI3120,0x2E,hw_dnr[ucValue][8]);
	gpio_i2c_write(I2C_HI3120,0x2F,hw_dnr[ucValue][9]);
	gpio_i2c_write(I2C_HI3120,0x30,hw_dnr[ucValue][10]);
}

void hw3120_LTI(unsigned char value)
{
	unsigned char ucValue,i;
	ucValue = value&0x0f;
	if(ucValue > 8)
		ucValue = 8;
	gpio_i2c_write(I2C_HI3120,0xff, 0x02);  // change page
	for (i = 0; i < 2; i++)
	{
		gpio_i2c_write(I2C_HI3120,hw_lti[0][i],hw_lti[ucValue+1][i]);
	}	
}

void hw3120_LTI_onoff(unsigned char bvalue)
{
	unsigned char u8value;
	gpio_i2c_write(I2C_HI3120,0xff,0x02);
	u8value = gpio_i2c_read(I2C_HI3120,0x83);
	if(bvalue)
	{
		u8value = u8value&0xef;
	}
	else
	{
		u8value = u8value|0x10;
	}
	gpio_i2c_write(I2C_HI3120,0x83,u8value);
}

void hw3120_CTI(unsigned char value)
{
	unsigned char ucValue,i;
	ucValue = value&0x0f;
	if(ucValue > 8)
		ucValue = 8;
	gpio_i2c_write(I2C_HI3120,0xff, 0x02);  // change page
	for (i = 0; i < 2; i++)
	{
		gpio_i2c_write(I2C_HI3120,hw_cti[0][i],hw_cti[ucValue+1][i]);
	}	
}

void hw3120_CTI_onoff(unsigned char bvalue)
{
	unsigned char u8value;
	gpio_i2c_write(I2C_HI3120,0xff,0x02);
	u8value = gpio_i2c_read(I2C_HI3120,0x83);
	if(bvalue)
	{
		u8value = u8value&0xdf;
	}
	else
	{
		u8value = u8value|0x20;
	}
	gpio_i2c_write(I2C_HI3120,0x83,u8value);
}


void hw3120_Sharpness(unsigned char value)
{
	unsigned char ucValue;
	gpio_i2c_write(I2C_HI3120,0xff, 0x02);  // change page
	ucValue = value&0x3f;
	gpio_i2c_write(I2C_HI3120,0xb7, ucValue);
	gpio_i2c_write(I2C_HI3120,0xb8, ucValue);
	gpio_i2c_write(I2C_HI3120,0xba, 0x00);
}

void	hw3120_image_enhance_onoff(unsigned char bvalue)
{
	unsigned char value;
	gpio_i2c_write(I2C_HI3120,0xff,0x01);
	value = gpio_i2c_read(I2C_HI3120,0x8c);
	if(bvalue)
	{
		value = value&0xfd;
	}
	else
	{
		value = value|0x02;
	}
	gpio_i2c_write(I2C_HI3120,0x8c,value);
}

void hw3120_Brightness(unsigned int u16value)
{
	unsigned char ucValue;
	gpio_i2c_write(I2C_HI3120,0xff, 0x01);  
	gpio_i2c_write(I2C_HI3120,0x90, 0x00);
	gpio_i2c_write(I2C_HI3120,0x91, 0x00);
	ucValue = u16value;
	gpio_i2c_write(I2C_HI3120,0x94,ucValue);
	ucValue = (u16value>>8)&0x03;
	gpio_i2c_write(I2C_HI3120,0x95,ucValue);
}

void hw3120_Contrast(unsigned int u16value)
{
	unsigned char ucValue;
	gpio_i2c_write(I2C_HI3120,0xff, 0x01);  
	ucValue = u16value;
 	gpio_i2c_write(I2C_HI3120,0x92,ucValue);
 	ucValue = (u16value>>8)&0x01;
	gpio_i2c_write(I2C_HI3120,0x93,ucValue);	
}

void hw3120_Hue(unsigned char value)
{
	unsigned int ucValue;
	ucValue = value&0xff;
	if(ucValue>100)	return;
	gpio_i2c_write(I2C_HI3120,0xff, 0x01);  // change page
	ucValue = ucValue*4;
	gpio_i2c_write(I2C_HI3120,0x98,hw_hue[ucValue+1]);
	gpio_i2c_write(I2C_HI3120,0x99,hw_hue[ucValue]);
	gpio_i2c_write(I2C_HI3120,0x9a,hw_hue[ucValue+3]);
	gpio_i2c_write(I2C_HI3120,0x9b,hw_hue[ucValue+2]);
}

void hw3120_Saturation(unsigned int u16value)
{
	unsigned char ucValue;
	gpio_i2c_write(I2C_HI3120,0xff, 0x01);  // change page
	ucValue = u16value;
	gpio_i2c_write(I2C_HI3120,0x96,ucValue);
	ucValue = (u16value>>8)&0x01;
	gpio_i2c_write(I2C_HI3120,0x97,ucValue);
}

void hw3120_ColorBar(unsigned char value)
{
	unsigned char i;
	if(value)
	{
	    	hw3120_mode(RGB800600To800600);
		gpio_i2c_write(I2C_HI3120,0xff,0x01);
		//内部彩条宽度寄存器
		gpio_i2c_write(I2C_HI3120,0x28,0x58); 
		//内部彩条控制寄存器
		gpio_i2c_write(I2C_HI3120,0x29,0x00);  
		//用户定义彩条水平总长度寄存器
		gpio_i2c_write(I2C_HI3120,0x31,0x06);  	//0x03
		gpio_i2c_write(I2C_HI3120,0x30,0x40);  	//0x5a
		//用户定义彩条水平起始位置寄存器
		gpio_i2c_write(I2C_HI3120,0x33,0x00);  
		gpio_i2c_write(I2C_HI3120,0x32,0x7a); 
		//用户定义彩条水平有效长度寄存器
		gpio_i2c_write(I2C_HI3120,0x35,0x05);	//0x02  
		gpio_i2c_write(I2C_HI3120,0x34,0x56);   	//0xd0
		//用户定义彩条HSYNC控制信号保持高有效（或低有效）宽度寄存器
		gpio_i2c_write(I2C_HI3120,0x36,0x64);  
		//用户定义彩条垂直总长度寄存器
		gpio_i2c_write(I2C_HI3120,0x38,0x02);  
		gpio_i2c_write(I2C_HI3120,0x37,0x0d);  
		//用户定义彩条垂直起始位置寄存器
		gpio_i2c_write(I2C_HI3120,0x3a,0x00);  
		gpio_i2c_write(I2C_HI3120,0x39,0x13); 
		//用户定义彩条垂直有效长度寄存器
		gpio_i2c_write(I2C_HI3120,0x3c,0x00);  
		gpio_i2c_write(I2C_HI3120,0x3b,0xf0);   
		//用户定义彩条VSYNC控制信号保持高有效（或低有效）宽度寄存器
		gpio_i2c_write(I2C_HI3120,0x3d,0x03);  
		//打开YCBCR->RGB
		gpio_i2c_write(I2C_HI3120,0x8c,0x1a);  
		//VI_CTRLR	输入视频接口控制寄存器
		i=gpio_i2c_read(I2C_HI3120,0x01); 
		i=i|0x08;
		gpio_i2c_write(I2C_HI3120,0x01,i);	//设输入扫描模式为逐行输入
	}
	else  //normal output mode
	{
		//To Recovery The Previous Video Mode Here
		gpio_i2c_write(I2C_HI3120,0xff,0x01);	
		//VI_CTRLR	输入视频接口控制寄存器
		i=gpio_i2c_read(I2C_HI3120,0x01); 
		i=i&0xf7;
		gpio_i2c_write(I2C_HI3120,0x01,i);	//设输入扫描模式为隔行输入
	}
}


void hw3120_ColorBalance(unsigned int bvalue)
{
	unsigned char ucValue;
	gpio_i2c_write(I2C_HI3120,0xff, 0x01); 	
	ucValue = gpio_i2c_read(I2C_HI3120,0x8c);
	if(bvalue)
	{
		ucValue = ucValue&0xef;
	}
	else
	{
		ucValue = ucValue|0x10;
	}
	gpio_i2c_write(I2C_HI3120,0x8c,ucValue);
	gpio_i2c_write(I2C_HI3120,0xf0, 0x60); 	
	gpio_i2c_write(I2C_HI3120,0xf2, 0x60); 	
	gpio_i2c_write(I2C_HI3120,0xf4, 0x60); 	
	gpio_i2c_write(I2C_HI3120,0xf6, 0x00); 	
	gpio_i2c_write(I2C_HI3120,0xf8, 0x20); 	
	gpio_i2c_write(I2C_HI3120,0xfa, 0x10); 		
}


/**********************************soft reset**********************************************************/
void hi3120_reset(void)
{
   hw3120_init();
   hw3120_gamma();
   hw3120_gamma_onoff(1);
    
   //hw3120_mode(PalTo800600); 
   hw3120_mode(NtscTo800600); 
   
   hw3120_CTI(4);
   hw3120_CTI_onoff(1);
 //   hw3120_LTI(5);
 //   hw3120_Sharpness(0x08);
 //   hw3120_Brightness(0x40);
 //   hw3120_Contrast(0x100);
 //   hw3120_Saturation(0x100);
 //   hw3120_Hue(50);
   hw3120_image_enhance_onoff(1);
 //   hw3120_ColorBar(1);
 //   hw3120_itop();
   hw3120_ColorBalance(1);   
   hw3120_DNR(3);
}    



static int hi3120_open(struct inode * inode, struct file * file)
{  

    return 0;    	

}  
   
/* 
 *Close device, Do nothing!
 */
static int hi3120_close(struct inode *inode ,struct file *file)
{  

    return 0;
}  
   
   
static int hi3120_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{  
   
    	unsigned int __user *argp = (unsigned int __user *)arg;
        unsigned int temp;
        
        if(copy_from_user(&temp,argp,sizeof(temp)))
        {   
            return -EFAULT;
        }
        return 0;
       
}  
   
/* 
 *  The various file operations we support.
 */
   
static struct file_operations hi3120_fops = {
	.owner		= THIS_MODULE,
	.ioctl		= hi3120_ioctl,
	.open		= hi3120_open,
	.release	= hi3120_close
};
   
static struct miscdevice hi3120_dev = {
	MISC_DYNAMIC_MINOR,
	"hi3120",
	&hi3120_fops,
};
   
static int hi3120_device_init(void)
{  
        /* inite codec configs.*/
        unsigned char temp = 0;
   
        gpio_i2c_write(I2C_HI3120,0xff,0x00);
        temp = gpio_i2c_read(I2C_HI3120,0x00);
        gpio_i2c_write(I2C_HI3120,0x00,0xfe);
        if( gpio_i2c_read(I2C_HI3120,0x00) != 0xfe)
        {
            printk("init hi3120 error");
            return -1;
        }
        gpio_i2c_write(I2C_HI3120,0x00,temp);
        hi3120_reset(); 
        return 0;
}   	
   
DECLARE_KCOM_GPIO_I2C();
static int __init hi3120_init(void)
{  
    	unsigned int ret = 0;
    	ret = KCOM_GPIO_I2C_INIT();
       if(ret)
        {
          printk("GPIO I2C module is not load.\n");
          return -1;
        } 
    	ret = misc_register(&hi3120_dev);
    	if(ret)
    	{
    		KCOM_GPIO_I2C_EXIT();
    		printk("could not register hi3120 devices. \n");
    		return ret;
    	}
    	
     	if(hi3120_device_init()<0)
     	{
        	misc_deregister(&hi3120_dev);
        	KCOM_GPIO_I2C_EXIT();
        	printk("hi3120 driver init fail for device init error!\n");
        	return -1;
    	}  
        printk("hi3120 driver init successful!\n");
        return ret;   	
}     
   
static void __exit hi3120_exit(void)
{  
    misc_deregister(&hi3120_dev);
    printk("deregister hi3120");
    KCOM_GPIO_I2C_EXIT();
}  
   
module_init(hi3120_init);
module_exit(hi3120_exit);
   
#ifdef MODULE
#include <linux/compile.h>
#endif
   
MODULE_INFO(build, UTS_VERSION);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hisilicon");

