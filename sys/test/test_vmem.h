#ifndef INCLUDE_TEST_VMEM_H
#define INCLUDE_TEST_VMEM_H


struct test_vmem_page {
    virtaddr_t virtaddr;
    physaddr_t physaddr;
    unsigned int used : 1;

};

struct test_virtmem {
    struct virtmem v;
    struct test_vmem_page *pages;
    int n_pages;
};

void check_initialize_virtmem_tests(TCase *t);

#endif

