#include "rtconfig.h"
#include "rtdef.h"
#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <rtthread.h>

struct ThreadStartArgs {
  void (*tentry)(void *);
  void (*texit)(void);
  void *parameter;
};

struct ThreadSwitchArgs {
  int save_from;
  Context **from;
  Context **to;
};
static Context *ev_handler(Event e, Context *c) {
  switch (e.event) {
  case EVENT_YIELD: {
    struct ThreadSwitchArgs *args =
        (struct ThreadSwitchArgs *)rt_thread_self()->user_data;
    if (args->save_from)
      *(args->from) = c;
    c = (*args->to);
    break;
  }
  case EVENT_IRQ_TIMER: {
    break;
  }
  default: {
    printf("Unhandled event ID = %d\n", e.event);
    assert(0);
  }
  }
  return c;
}

void __am_cte_init() { cte_init(ev_handler); }

void rt_hw_context_switch_to(rt_ubase_t to) {
  volatile struct ThreadSwitchArgs args = {0, 0, (Context **)to};
  rt_ubase_t t = rt_thread_self()->user_data;
  rt_thread_self()->user_data = (rt_ubase_t)&args;
  yield();
  rt_thread_self()->user_data = t;
}

void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to) {
  volatile struct ThreadSwitchArgs args = {1, (Context **)from, (Context **)to};
  rt_ubase_t t = rt_thread_self()->user_data;
  rt_thread_self()->user_data = (rt_ubase_t)&args;
  yield();
  rt_thread_self()->user_data = t;
}

void rt_hw_context_switch_interrupt(void *context, rt_ubase_t from,
                                    rt_ubase_t to,
                                    struct rt_thread *to_thread) {
  assert(0);
}

void _start_thread(struct ThreadStartArgs *args) {
  args->tentry(args->parameter);
  args->texit();
}

rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter,
                             rt_uint8_t *stack_addr, void *texit) {
  uintptr_t stack_alined = ROUNDDOWN(stack_addr, sizeof(uintptr_t));
  struct ThreadStartArgs *ts_args =
      ((struct ThreadStartArgs *)stack_alined) - 1;
  ts_args->tentry = tentry;
  ts_args->texit = texit;
  ts_args->parameter = parameter;
  return (rt_uint8_t *)kcontext((Area){0, (void *)ts_args},
                                (void (*)(void *))_start_thread, ts_args);
}
