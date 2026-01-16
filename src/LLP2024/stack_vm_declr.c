// src: https://stepik.org/lesson/1443634/step/13?unit=1462435

/* Available to you:

struct maybe_int64 {
    int64_t value;
    bool valid;
};

struct maybe_int64 some_int64(int64_t i);

extern const struct maybe_int64 none_int64;

void maybe_int64_print( struct maybe_int64 i );
struct maybe_int64 maybe_read_int64();
void print_int64(int64_t i)
*/

typedef void (*insHandler)(struct vm_state*);

void interpret_push(struct vm_state* state) {
  stack_push(&state->data_stack, (state->ip - 1)->as_arg64.arg);
}

void interpret_iread(struct vm_state* state) {
  struct maybe_int64 num = maybe_read_int64();
  if (num.valid) {
    if (stack_push(&state->data_stack, num.value) == false) state->ip = NULL;
    ;
  }
}

void interpret_iadd(struct vm_state* state) {
  struct maybe_int64 num2 = stack_pop(&state->data_stack);
  if (num2.valid) {
    struct maybe_int64 num1 = stack_pop(&state->data_stack);
    if (num1.valid) {
      stack_push(&state->data_stack, num2.value + num1.value);
    } else
      state->ip = NULL;         // end num1.valid
  } /*else state->ip = NULL;*/  // end of branching num2.valid
}

void interpret_iprint(struct vm_state* state) {
  struct maybe_int64 num = stack_pop(&state->data_stack);
  if (num.valid) {
    print_int64(num.value);
  }
}

/* Hint: you can run the program while ip != NULL,
    then to stop it, it is enough to reset ip to zero  */
void interpret_stop(struct vm_state* state) { state->ip = NULL; }

insHandler interpreters[] = {[BC_PUSH] = interpret_push,
                             [BC_IREAD] = interpret_iread,
                             [BC_IADD] = interpret_iadd,
                             [BC_IPRINT] = interpret_iprint,
                             [BC_STOP] = interpret_stop};

void interpret(struct vm_state* state) {
  if (!state) return;
  while (state->ip) {
    insHandler foo = interpreters[state->ip->opcode];
    state->ip++;
    if (foo) {
      foo(state);
    } else {
      err("Not implemented");
      break;
    }
  }
}
