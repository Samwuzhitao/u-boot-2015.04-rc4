/*
 * (C) Copyright 2006 OpenMoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

#include <nand.h>
#include <asm/arch/s3c24x0_cpu.h>
#include <asm/io.h>


int nand_spl_load_image(uint32_t offs, unsigned int size, void *dst)
{
	return 0;
}

/* nand_init() - initialize data to make nand usable by SPL */
void nand_init(void)
{

}

/* Unselect after operation */
void nand_deselect(void)
{

}

