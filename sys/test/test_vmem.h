#ifndef INCLUDE_TEST_VMEM_H
#define INCLUDE_TEST_VMEM_H


struct test_vmem_page {
    physaddr_t *physaddr;

};

struct test_virtmem {
    struct virtmem v;
    struct test_vmem_page *pages;
    int n_pages;
};

#endif

