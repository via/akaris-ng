#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#define I386

#define PHYS_BITS 32
#define VIRT_BITS 32

#define SMP


/* I386 Specific configuration */
#ifdef I386
#define MAX_PHYS_MEMORY (4L * 1024L * 1024L * 1024L)
#define MAX_PHYS_REGIONS (MAX_PHYS_MEMORY / (128 * 1024 * 1024))
#endif

/*#define HOSTED*/
#endif
