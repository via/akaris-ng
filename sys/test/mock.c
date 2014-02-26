#include "mock.h"
#include <stdio.h>

struct mocked_call *
mock_call_alloc(const char *name) {
  struct mocked_call *m = malloc(sizeof(struct mocked_call));
  m->name = (char *)malloc(strlen(name) + 1);
  strcpy(m->name, name);
  STAILQ_INIT(&m->params);
  return m;
}

void
mock_call_list_init(mock_call_list *list) {
  STAILQ_INIT(list);
}

static void mock_free_param(struct mocked_call_parameter *p) {
  if (p->type == PARAM_STR)
    free(p->data.str);

  free(p);
}

static void mock_free_call(struct mocked_call *c) {
  struct mocked_call_parameter *p, *tmp;

  STAILQ_FOREACH_SAFE(p, &c->params, params, tmp) {
    mock_free_param(p);
  }

  free(c->name);
  free(c);

}

static const char *
mock_param_text(struct mocked_call_parameter *p) {
  static char parambuf[64];
  switch(p->type) {
    case PARAM_INT:
      snprintf(parambuf, 64, "%d ", p->data.i);
      break;
    case PARAM_LONG:
      snprintf(parambuf, 64, "%ld", p->data.l);
      break;
    case PARAM_PTR:
      snprintf(parambuf, 64, "%p ", p->data.ptr);
      break;
    case PARAM_STR:
      snprintf(parambuf, 64, "\"%.60s\" ", p->data.str);
      break;
    case PARAM_DONTCARE:
      snprintf(parambuf, 64, "<dontcare>");
      break;
    case PARAM_END:
    case PARAM_CUST:
      strcpy(parambuf, "");
      break;
  }
  return parambuf;
}
static char *
mock_call_text(struct mocked_call *c) {

  char *buf;
  int totlen = 0;
  struct mocked_call_parameter *p;
  /* First figure total length of params list */
  STAILQ_FOREACH(p, &c->params, params) {
    totlen += strlen(mock_param_text(p)) + 1;
  }

  buf = malloc(totlen + strlen(c->name) + 4); /* lengths + two paren, space, null */
  strcpy(buf, c->name);
  strcat(buf, "( ");
  STAILQ_FOREACH(p, &c->params, params) {
    strcat(buf, mock_param_text(p));
    strcat(buf, " ");
  }
  strcat(buf, ")");

  return buf;
}

static const char *
mock_error_text(struct mocked_call *expected, struct mocked_call *observed) {
  char *ex = NULL, *ob = NULL;
  char *buf;

  if (expected != NULL) {
    ex = mock_call_text(expected);
  } else {
    ex = " -none- ";
  }
  if (observed != NULL) {
    ob = mock_call_text(observed);
  } else {
    ob = " - none- ";
  }

  const char *fmt = "expected: %s   observed: %s";
  int bufsize = strlen(ex) + strlen(ob) + strlen(fmt) + 1; 
  buf = malloc(bufsize);
  snprintf(buf, bufsize, fmt, ex, ob);

  if (expected)
    free(ex);
  if (observed)
    free(ob);

  return buf;
}

static int
mock_compare_param(struct mocked_call_parameter *expected, 
    struct mocked_call_parameter *observed) {

  if (expected->type == PARAM_DONTCARE)
    return 1;

  if (expected->type != observed->type)
    return 0;

  switch (expected->type) {
    case PARAM_END:
      return 1;
      break;
    case PARAM_INT:
      return (expected->data.i == observed->data.i);
    case PARAM_LONG:
      return (expected->data.l == observed->data.l);
    case PARAM_PTR:
      return (expected->data.str == observed->data.str);
    case PARAM_STR:
      /* Both null? Ok */
      if ((expected->data.str == NULL) && (observed->data.str == NULL))
        return 1;
      /* Only one null? Not Ok */
      if ((expected->data.str == NULL) || (observed->data.str == NULL))
        return 0;
      return (strcmp(expected->data.str, observed->data.str) == 0);
    case PARAM_CUST:
    default:
      return 0;

  }
}

static struct mocked_call_parameter *mock_int(int a) {
  struct mocked_call_parameter *newparam = malloc(sizeof(struct mocked_call_parameter));
  *newparam = MOCK_INT(a);
  return newparam;
}

static struct mocked_call_parameter *mock_long(long a) {
  struct mocked_call_parameter *newparam = malloc(sizeof(struct mocked_call_parameter));
  *newparam = MOCK_LONG(a);
  return newparam;
}

static struct mocked_call_parameter *mock_ptr(void *a) {
  struct mocked_call_parameter *newparam = malloc(sizeof(struct mocked_call_parameter));
  *newparam = MOCK_PTR(a);
  return newparam;
}

static struct mocked_call_parameter *mock_str(const char *a) {
  struct mocked_call_parameter *newparam = (struct mocked_call_parameter *)
    malloc(sizeof(struct mocked_call_parameter));
  char *param = (char *)malloc(strlen(a) + 1);
  *newparam = MOCK_STR(param);
  return newparam;
}

static struct mocked_call_parameter *mock_dontcare() {
  struct mocked_call_parameter *newparam = malloc(sizeof(struct mocked_call_parameter));
  *newparam = MOCK_DONTCARE();
  return newparam;
}

static struct mocked_call_parameter *mock_end() {
  struct mocked_call_parameter *newparam = malloc(sizeof(struct mocked_call_parameter));
  *newparam = MOCK_END();
  return newparam;
}


static int
mock_compare_calls(struct mocked_call *expected, struct mocked_call *observed) {

  struct mocked_call_parameter *ex, *ob, *tmp;
  int retval;
  if (strcmp(expected->name, observed->name) != 0)
    return 0;

  STAILQ_FOREACH_SAFE(ex, &expected->params, params, tmp) {
    if (STAILQ_EMPTY(&observed->params))
      return 0;

    ob = STAILQ_FIRST(&observed->params);
    STAILQ_REMOVE_HEAD(&observed->params, params);
    STAILQ_REMOVE_HEAD(&expected->params, params);
    retval = mock_compare_param(ex, ob);
    mock_free_param(ex);
    mock_free_param(ob);
    if (retval == 0) 
      return 0;
  }


  return 1;
}


static void mock_assemble_args(struct mocked_call *call, va_list args) {
  
  struct mocked_call_parameter p;
  do {
    struct mocked_call_parameter *new;
    p = va_arg(args, struct mocked_call_parameter);
    if (p.type == PARAM_INT) {
      new = mock_int(p.data.i);
      STAILQ_INSERT_TAIL(&call->params, new, params);
    }
    if (p.type == PARAM_LONG) {
      new = mock_long(p.data.l);
      STAILQ_INSERT_TAIL(&call->params, new, params);
    }
    if (p.type == PARAM_PTR) {
      new = mock_ptr(p.data.ptr);
      STAILQ_INSERT_TAIL(&call->params, new, params);
    }
    if (p.type == PARAM_DONTCARE) {
      new = mock_dontcare();
      STAILQ_INSERT_TAIL(&call->params, new, params);
    }
    if (p.type == PARAM_STR) {
      new = mock_str(p.data.str);
      STAILQ_INSERT_TAIL(&call->params, new, params);
    }
  } while (p.type != PARAM_END);
  
  STAILQ_INSERT_TAIL(&call->params, mock_end(), params);
}


int
mock_expect(mock_call_list *list, const char **err, const char *fname, ...) {
  va_list ap;
  struct mocked_call *expected_call, *observed_call;
  int retval;

  va_start(ap, fname);
  expected_call = mock_call_alloc(fname);
  mock_assemble_args(expected_call, ap);

  if (STAILQ_EMPTY(list)) {
    *err = mock_error_text(expected_call, NULL);
    return 0;
  }

  observed_call = STAILQ_FIRST(list);
  STAILQ_REMOVE_HEAD(list, calls);
  *err = mock_error_text(expected_call, observed_call);
  retval = mock_compare_calls(expected_call, observed_call);
  mock_free_call(expected_call);
  mock_free_call(observed_call);
  if (retval == 0)
    return 0;


  return 1;


}

int mock_call_list_empty(mock_call_list *calls) {
  return STAILQ_EMPTY(calls);
}

void mock_call(mock_call_list *calls, const char *fname, ...) {
  va_list ap;
  struct mocked_call *m = mock_call_alloc(fname);

  va_start(ap, fname);
  mock_assemble_args(m, ap);

  STAILQ_INSERT_TAIL(calls, m, calls);
}

