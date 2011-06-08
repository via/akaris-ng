#include "kernel.h"
#include "physical_memory.h"
#include "i686_cpu.h"

static void i686_cpu_init(struct cpu *_cpu) {




}


static void i686_cpu_schedule(struct cpu *_cpu) {


}



void i686_cpu_alloc(struct i686_cpu *cpu, struct kernel *k) {

  feeder_physmem_create(&cpu->feeder, k->phys, 1024, 1024);
  cpu->c.k = k;
  cpu->c.localmem = (struct physmem *)&cpu->feeder;
  cpu->c.model = "Lolwut i686";
  cpu->c.v.init = i686_cpu_init;
  cpu->c.v.schedule = i686_cpu_schedule;



}
