#ifndef __CPU_H__
#define __CPU_H__

/* CPUID query leaf function */
#define CPUID_EAX_CPU			0U
#define CPUID_EAX_FEATURE		1U
#define CPUID_EAX_CACHE			2U
#define CPUID_EAX_SERIALNUM		3U
#define CPUID_EAX_EXTEND_FEATURE	7U

/* CPUID features support offset bit of EDX */
#define FPU_FEATURE	(1U<<0U)
#define VME_FEATURE	(1U<<1U)
#define DE_FEATURE	(1U<<2U)
#define PSE_FEATURE	(1U<<3U)
#define TSC_FEATURE	(1U<<4U)
#define MSR_FEATURE	(1U<<5U)
#define PAE_FEATURE	(1U<<6U)
#define MCE_FEATURE	(1U<<7U)
#define CX8_FEATURE	(1U<<8U)
#define APIC_FEATURE	(1U<<9U)
#define SEP_FEATURE	(1U<<11U)
#define MTRR_FEATURE	(1U<<12U)
#define PGE_FEATURE	(1U<<13U)
#define MCA_FEATURE	(1U<<14U)
#define CMOV_FEATURE	(1U<<15U)
#define PAT_FEATURE	(1U<<16U)
#define PSE36_FEATURE	(1U<<17U)
#define PSN_FEATURE	(1U<<18U)
#define CLFSH_FEATURE	(1U<<19U)
#define DS_FEATURE	(1U<<21U)
#define ACPI_FEATURE	(1U<<22U)
#define MMX_FEATURE	(1U<<23U)
#define FXSR_FEATURE	(1U<<24U)
#define SSE_FEATURE	(1U<<25U)
#define SSE2_FEATURE	(1U<<26U)
#define SS_FEATURE	(1U<<27U)
#define HTT_FEATURE	(1U<<28U)
#define TM_FEATURE	(1U<<29U)
#define PBE_FEATURE	(1U<<31U)

/* CPUID features support offset bit of ECX */

int get_cpuid(int *eax, int *ebx, int *ecx, int *edx, int leaf, int subleaf);
#endif
