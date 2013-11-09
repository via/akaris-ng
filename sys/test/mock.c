#include "mock.h"
#include <stdio.h>

struct mocked_call *
mock_call_alloc(const char *name) {
  struct mocked_call *m = malloc(sizeof(struct mocked_call));
  m->name = strdup(name);
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
  struct mocked_call_parameter *p;

  STAILQ_FOREACH(p, &c->params, params) {
    mock_free_param(p);
  }

  free(c->name);

}

static const char *
mock_param_text(struct mocked_call_parameter *p) {
  static char parambuf[64];
  switch(p->type) {
    case PARAM_INT:
      sprintf(parambuf, "%d ", p->data.i);
      break;
    case PARAM_STR:
      sprintf(parambuf, "\"%.60s\" ", p->data.str);
      break;
    case PARAM_END:
    case PARAM_CUST:
      strcpy(parambuf, "");
      break;
  }
  return parambuf;
}
static const char *
mock_error_text(struct mocked_call *expected, struct mocked_call *observed) {
  static char buf[256] = "";

  struct mocked_call_parameter *p;
  
  if (expected != NULL) {
    sprintf(buf, "Expected: %s( ", expected->name);
    STAILQ_FOREACH(p, &expected->params, params) {
      strcat(buf, mock_param_text(p));
    }
    strcat(buf, ") ");
  } else {
    strcat(buf, " - ");
  }
  if (observed != NULL) {
    strcat(buf, "    Observed: ");
    strcat(buf, observed->name);
    strcat(buf, "( ");
    STAILQ_FOREACH(p, &observed->params, params) {
      strcat(buf, mock_param_text(p));
    }
    strcat(buf, ")");
  } else {
    strcat(buf, " - ");
  }
  return buf;
}

static int
mock_compare_param(struct mocked_call_parameter *expected, 
    struct mocked_call_parameter *observed) {

  if (expected->type != observed->type)
    return 0;

  switch (expected->type) {
    case PARAM_END:
      return 1;
      break;
    case PARAM_INT:
      return (expected->data.i == observed->data.i);
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

static struct mocked_call_parameter *mock_str(const char *a) {
  struct mocked_call_parameter *newparam = malloc(sizeof(struct mocked_call_parameter));
  *newparam = MOCK_STR(strdup(a));
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
    if (p.type == PARAM_STR) {
      new = mock_str(p.data.str);
      STAILQ_INSERT_TAIL(&call->params, new, params);
    }
  } while (p.type != PARAM_END);
  
  STAILQ_INSERT_TAIL(&call->params, mock_end(), params);
}


int
mock_call_expect(mock_call_list *list, const char **err, const char *fname, ...) {
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

void mock_call(mock_call_list *calls, const char *fname, ...) {
  va_list ap;
  struct mocked_call *m = mock_call_alloc(fname);

  va_start(ap, fname);
  mock_assemble_args(m, ap);

  STAILQ_INSERT_TAIL(calls, m, calls);
}

