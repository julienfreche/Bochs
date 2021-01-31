/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2021  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA

//
// bochs.h is the master header file for all C++ code.  It includes all
// the system header files needed by bochs, and also includes all the bochs
// C++ header files.  Because bochs.h and the files that it includes has
// structure and class definitions, it cannot be called from C code.
//

#ifndef BX_BOCHS_H
#  define BX_BOCHS_H 1

#include "config.h"      /* generated by configure script from config.h.in */

#ifndef __QNXNTO__
extern "C" {
#endif

#ifdef WIN32
// In a win32 compile (including cygwin), windows.h is required for several
// files in gui and iodev.  It is important to include it here in a header
// file so that WIN32-specific data types can be used in fields of classes.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(__sun__)
#undef EAX
#undef ECX
#undef EDX
#undef EBX
#undef ESP
#undef EBP
#undef ESI
#undef EDI
#undef EIP
#undef CS
#undef DS
#undef ES
#undef SS
#undef FS
#undef GS
#endif
#include <assert.h>
#include <errno.h>

#ifndef WIN32
#  include <unistd.h>
#else
#  include <io.h>
#endif
#include <time.h>
#if BX_WITH_MACOS
#  include <types.h>
#  include <stat.h>
#  include <cstdio>
#  include <unistd.h>
#elif BX_WITH_CARBON
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/param.h> /* for MAXPATHLEN */
#  include <sys/time.h>
#  include <utime.h>
#else
#  ifndef WIN32
#    include <sys/time.h>
#  endif
#  include <sys/types.h>
#  include <sys/stat.h>
#endif
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#ifdef macintosh
#  define SuperDrive "[fd:]"
#endif

#ifndef __QNXNTO__
}
#endif

#include "osdep.h"       /* platform dependent includes and defines */
#include "gui/paramtree.h"

// BX_SHARE_PATH should be defined by the makefile.  If not, give it
// a value of NULL to avoid compile problems.
#ifndef BX_SHARE_PATH
#define BX_SHARE_PATH NULL
#endif

// prototypes
int  bx_begin_simulation(int argc, char *argv[]);
void bx_stop_simulation();
char *bx_find_bochsrc(void);
const char *get_builtin_variable(const char *varname);
int  bx_parse_cmdline(int arg, int argc, char *argv[]);
int  bx_read_configuration(const char *rcfile);
int  bx_write_configuration(const char *rcfile, int overwrite);
void bx_reset_options(void);
void bx_set_log_actions_by_device(bool panic_flag);
// special config parameter and options functions for plugins
void bx_init_std_nic_options(const char *name, bx_list_c *menu);
void bx_init_usb_options(const char *usb_name, const char *pname, int maxports);
int  bx_parse_param_from_list(const char *context, const char *input, bx_list_c *list);
int  bx_parse_nic_params(const char *context, const char *param, bx_list_c *base);
int  bx_parse_usb_port_params(const char *context, bool devopt,
                              const char *param, int maxports, bx_list_c *base);
int  bx_split_option_list(const char *msg, const char *rawopt, char **argv, int max_argv);
int  bx_write_param_list(FILE *fp, bx_list_c *base, const char *optname, bool multiline);
int  bx_write_usb_options(FILE *fp, int maxports, bx_list_c *base);

Bit32u crc32(const Bit8u *buf, int len);

// used to print param tree from debugger
void print_tree(bx_param_c *node, int level = 0, bool xml = false);

#if BX_ENABLE_STATISTICS
// print statistics
void print_statistics_tree(bx_param_c *node, int level = 0);
#define INC_STAT(stat) (++(stat))
#else
#define INC_STAT(stat)
#endif

//
// some macros to interface the CPU and memory to external environment
// so that these functions can be redirected to the debugger when
// needed.
//

#define BXRS_PARAM_SPECIAL(parent, name, maxvalue, save_handler, restore_handler) \
{ \
  bx_param_num_c *param = new bx_param_num_c(parent, #name, "", "", 0, maxvalue, 0); \
  param->set_base(BASE_HEX); \
  param->set_sr_handlers(this, save_handler, restore_handler); \
}

#define BXRS_PARAM_SPECIAL64(parent, name, save_handler, restore_handler) \
  BXRS_PARAM_SPECIAL(parent, name, BX_MAX_BIT64U, save_handler, restore_handler)
#define BXRS_PARAM_SPECIAL32(parent, name, save_handler, restore_handler) \
  BXRS_PARAM_SPECIAL(parent, name, BX_MAX_BIT32U, save_handler, restore_handler)
#define BXRS_PARAM_SPECIAL16(parent, name, save_handler, restore_handler) \
  BXRS_PARAM_SPECIAL(parent, name, BX_MAX_BIT16U, save_handler, restore_handler)
#define BXRS_PARAM_SPECIAL8(parent, name, save_handler, restore_handler) \
  BXRS_PARAM_SPECIAL(parent, name, BX_MAX_BIT8U,  save_handler, restore_handler)

#define BXRS_HEX_PARAM_SIMPLE32(parent, name) \
  new bx_shadow_num_c(parent, #name, (Bit32u*)&(name), BASE_HEX)
#define BXRS_HEX_PARAM_SIMPLE64(parent, name) \
  new bx_shadow_num_c(parent, #name, (Bit64u*)&(name), BASE_HEX)

#define BXRS_HEX_PARAM_SIMPLE(parent, name) \
  new bx_shadow_num_c(parent, #name, &(name), BASE_HEX)
#define BXRS_HEX_PARAM_FIELD(parent, name, field) \
  new bx_shadow_num_c(parent, #name, &(field), BASE_HEX)

#define BXRS_DEC_PARAM_SIMPLE(parent, name) \
  new bx_shadow_num_c(parent, #name, &(name), BASE_DEC)
#define BXRS_DEC_PARAM_FIELD(parent, name, field) \
  new bx_shadow_num_c(parent, #name, &(field), BASE_DEC)

#define BXRS_PARAM_BOOL(parent, name, field) \
  new bx_shadow_bool_c(parent, #name, (bool*)&(field))

// =-=-=-=-=-=-=- Normal optimized use -=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define BX_INP(addr, len)           bx_devices.inp(addr, len)
#define BX_OUTP(addr, val, len)     bx_devices.outp(addr, val, len)

#if BX_SUPPORT_SMP
#define BX_CPU(x)                   (bx_cpu_array[x])
#else
#define BX_CPU(x)                   (&bx_cpu)
#endif

#define BX_MEM(x)                   (&bx_mem)

// you can't use static member functions on the CPU, if there are going
// to be 2 cpus.  Check this early on.
#if BX_SUPPORT_SMP
#  if BX_USE_CPU_SMF
#    error For SMP simulation, BX_USE_CPU_SMF must be 0.
#  endif
#endif

//
// Ways for the the external environment to report back information
// to the debugger.
//

#if BX_DEBUGGER
#  define BX_DBG_ASYNC_INTR bx_guard.async.irq
#  define BX_DBG_ASYNC_DMA  bx_guard.async.dma

#  define BX_DBG_DMA_REPORT(addr, len, what, val) \
        if (bx_guard.report.dma) bx_dbg_dma_report(addr, len, what, val)
#  define BX_DBG_IAC_REPORT(vector, irq) \
        if (bx_guard.report.irq) bx_dbg_iac_report(vector, irq)
#  define BX_DBG_A20_REPORT(val) \
        if (bx_guard.report.a20) bx_dbg_a20_report(val)
#  define BX_DBG_IO_REPORT(port, size, op, val) \
        if (bx_guard.report.io) bx_dbg_io_report(port, size, op, val)
#  define BX_DBG_LIN_MEMORY_ACCESS(cpu, lin, phy, len, memtype, rw, data) \
        bx_dbg_lin_memory_access(cpu, lin, phy, len, memtype, rw, data)
#  define BX_DBG_PHY_MEMORY_ACCESS(cpu, phy, len, memtype, rw, why, data) \
        bx_dbg_phy_memory_access(cpu, phy, len, memtype, rw, why, data)
#else  // #if BX_DEBUGGER
// debugger not compiled in, use empty stubs
#  define BX_DBG_ASYNC_INTR 1
#  define BX_DBG_ASYNC_DMA  1
#  define BX_DBG_DMA_REPORT(addr, len, what, val)                               /* empty */
#  define BX_DBG_IAC_REPORT(vector, irq)                                        /* empty */
#  define BX_DBG_A20_REPORT(val)                                                /* empty */
#  define BX_DBG_IO_REPORT(port, size, op, val)                                 /* empty */
#  define BX_DBG_LIN_MEMORY_ACCESS(cpu, lin, phy, len, memtype, rw, data)       /* empty */
#  define BX_DBG_PHY_MEMORY_ACCESS(cpu,      phy, len, memtype, rw, attr, data) /* empty */
#endif  // #if BX_DEBUGGER

#include "logio.h"

#ifndef UNUSED
#  define UNUSED(x) ((void)x)
#endif

//Generic MAX and MIN Functions
#define BX_MAX(a,b) ((a) > (b) ? (a) : (b))
#define BX_MIN(a,b) ((a) < (b) ? (a) : (b))

#if BX_SUPPORT_X86_64
#define FMT_ADDRX FMT_ADDRX64
#else
#define FMT_ADDRX FMT_ADDRX32
#endif

#if BX_PHY_ADDRESS_LONG
  #define FMT_PHY_ADDRX FMT_PHY_ADDRX64
#else
  #define FMT_PHY_ADDRX FMT_ADDRX32
#endif

#define FMT_LIN_ADDRX FMT_ADDRX

#if BX_GDBSTUB
// defines for GDB stub
void bx_gdbstub_init(void);
void bx_gdbstub_break(void);
int bx_gdbstub_check(unsigned int eip);
#define GDBSTUB_STOP_NO_REASON   (0xac0)

#if BX_SUPPORT_SMP
#error GDB stub was written for single processor support.  If multiprocessor support is added, then we can remove this check.
// The big problem is knowing which CPU gdb is referring to.  In other words,
// what should we put for "n" in BX_CPU(n)->dbg_xlate_linear2phy() and
// BX_CPU(n)->dword.eip, etc.
#endif
#endif

typedef struct {
  bool interrupts;
  bool exceptions;
  bool print_timestamps;
#if BX_DEBUGGER
  bool magic_break_enabled;
#endif
#if BX_GDBSTUB
  bool gdbstub_enabled;
#endif
#if BX_SUPPORT_APIC
  bool apic;
#endif
#if BX_DEBUG_LINUX
  bool linux_syscall;
#endif
} bx_debug_t;

#if BX_SHOW_IPS
BOCHSAPI_MSVCONLY void bx_show_ips_handler(void);
#endif
void CDECL bx_signal_handler(int signum);
int bx_atexit(void);
BOCHSAPI extern bx_debug_t bx_dbg;

enum {
#define bx_define_cpudb(model) bx_cpudb_##model,
#include "cpudb.h"
  bx_cpudb_model_last
};
#undef bx_define_cpudb

#if BX_SUPPORT_SMP
  #define BX_SMP_PROCESSORS (bx_cpu_count)
#else
  #define BX_SMP_PROCESSORS 1
#endif

BOCHSAPI extern Bit8u bx_cpu_count;
#if BX_SUPPORT_APIC
// determinted by XAPIC option
BOCHSAPI extern Bit32u apic_id_mask;
#endif

// memory access type (read/write/execute/rw)
enum {
  BX_READ    = 0,
  BX_WRITE   = 1,
  BX_EXECUTE = 2,
  BX_RW      = 3,
#if BX_SUPPORT_CET
  BX_SHADOW_STACK_READ    = 4,
  BX_SHADOW_STACK_WRITE   = 5,
  BX_SHADOW_STACK_INVALID = 6,  // can't execute shadow stack
  BX_SHADOW_STACK_RW      = 7,
#endif
};

// types of reset
#define BX_RESET_SOFTWARE 10
#define BX_RESET_HARDWARE 11

/* --- EXTERNS --- */

#if BX_GUI_SIGHANDLER
extern bool bx_gui_sighandler;
#endif

// This value controls how often each I/O device's timer handler
// gets called.  The timer is set up in iodev/devices.cc.
#define BX_IODEV_HANDLER_PERIOD 1000   // microseconds

#define BX_PATHNAME_LEN 512

#define BX_KBD_XT_TYPE        0
#define BX_KBD_AT_TYPE        1
#define BX_KBD_MF_TYPE        2

#define BX_N_OPTROM_IMAGES 4
#define BX_N_OPTRAM_IMAGES 4
#define BX_N_SERIAL_PORTS 4
#define BX_N_PARALLEL_PORTS 2
#define BX_N_PCI_SLOTS 5

void bx_center_print(FILE *file, const char *line, unsigned maxwidth);

#include "instrument.h"
#include "misc/bswap.h"

#endif  /* BX_BOCHS_H */
