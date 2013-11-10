#ifndef _MOCK_H
#define _MOCK_H

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "queue.h"

typedef enum {
  PARAM_END,
  PARAM_INT,
  PARAM_LONG,
  PARAM_STR,
  PARAM_PTR,
  PARAM_CUST,
} mocked_parameter_type;

typedef int (*mock_comparator)(void *, void *);

struct mocked_call_parameter {
  mocked_parameter_type type;
  STAILQ_ENTRY(mocked_call_parameter) params;
  mock_comparator compare;
  union {
    char *str;
    int i;
    unsigned long l;
    void *ptr;
  } data;
};

struct mocked_call {
  char *name;
  STAILQ_ENTRY(mocked_call) calls;
  STAILQ_HEAD(,mocked_call_parameter) params;
};

typedef STAILQ_HEAD(, mocked_call) mock_call_list;

struct mocked_call *mock_call_alloc(const char *name);
void mock_call_list_init(mock_call_list *);

void mock_call(mock_call_list *calls, const char *fname, ...);
int mock_expect(mock_call_list *calls, const char **err, const char *fname, ...);
int mock_call_list_empty(mock_call_list *);

#define MOCK_INT(x) ( (struct mocked_call_parameter) { \
    .type = PARAM_INT, \
    .data = { \
      .i = (x), \
    }, \
    })

#define MOCK_LONG(x) ( (struct mocked_call_parameter) { \
    .type = PARAM_LONG, \
    .data = { \
      .l = (x), \
    }, \
    })

#define MOCK_STR(x) ( (struct mocked_call_parameter) { \
    .type = PARAM_STR, \
    .data = { \
      .str = (x), \
    }, \
    })

#define MOCK_PTR(x) ( (struct mocked_call_parameter) { \
    .type = PARAM_PTR, \
    .data = { \
      .ptr = (void *)(x), \
    }, \
    })
      
#define MOCK_END(x) ( (struct mocked_call_parameter) { \
    .type = PARAM_END, \
  })


#endif

