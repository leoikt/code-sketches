// src: https://stepik.org/lesson/1443634/step/14?unit=1462435

/* The following functions are already available to you: */
bool stack_push(struct stack* s, int64_t value);
struct maybe_int64 stack_pop(struct stack* s);

/*
  struct maybe_int64 {
  int64_t value;
  bool valid;
  };
*/
struct maybe_int64 some_int64(int64_t i);
/*  const struct maybe_int64 none_int64; */

/*  Command interpreters */
void interpret_swap(struct vm_state* state) {
  struct maybe_int64 num2 = stack_pop(&state->data_stack);
  if (num2.valid) {
    struct maybe_int64 num1 = stack_pop(&state->data_stack);
    if (num1.valid) {
      stack_push(&state->data_stack, num2.value);
      stack_push(&state->data_stack, num1.value);
    } else {
      state->ip = NULL;
    }  // end num1.valid
  } else {
    state->ip = NULL;
  }  // end of branching num2.valid
}

void interpret_pop(struct vm_state* state) {
  struct maybe_int64 num = stack_pop(&state->data_stack);
  if (!num.valid) {
    state->ip = NULL;
  }
}

void interpret_dup(struct vm_state* state) {
  struct maybe_int64 num = stack_pop(&state->data_stack);
  if (num.valid) {
    stack_push(&state->data_stack, num.value);
    stack_push(&state->data_stack, num.value);
  } else
    state->ip = NULL;  // end of branching num2.valid
}

// These functions raise operations on numbers to the level of stack operations.
// lift_unop applies the operation to the top of the stack;
void lift_unop(struct stack* s, int64_t(f)(int64_t)) {
  struct maybe_int64 num = stack_pop(s);
  if (num.valid) {
    stack_push(s, f(num.value));
  }
}

// lift_binop pops two arguments from the stack,
// applies a function of two variables to them and pushes the result onto the
// stack
void lift_binop(struct stack* s, int64_t(f)(int64_t, int64_t)) {
  struct maybe_int64 num2 = stack_pop(s);
  if (num2.valid) {
    struct maybe_int64 num1 = stack_pop(s);
    if (num1.valid) {
      int64_t res_value = f(num1.value, num2.value);
      stack_push(s, res_value);
    }
  }
}

int64_t i64_add(int64_t a, int64_t b) { return a + b; }
int64_t i64_sub(int64_t a, int64_t b) { return a - b; }
int64_t i64_mul(int64_t a, int64_t b) { return a * b; }
int64_t i64_div(int64_t a, int64_t b) { return a / b; }
int64_t i64_cmp(int64_t a, int64_t b) {
  if (a > b)
    return 1;
  else if (a < b)
    return -1;
  return 0;
}

int64_t i64_neg(int64_t a) { return -a; }

void interpret_iadd(struct vm_state* state) {
  lift_binop(&state->data_stack, i64_add);
}
void interpret_isub(struct vm_state* state) {
  lift_binop(&state->data_stack, i64_sub);
}
void interpret_imul(struct vm_state* state) {
  lift_binop(&state->data_stack, i64_mul);
}
void interpret_idiv(struct vm_state* state) {
  lift_binop(&state->data_stack, i64_div);
}
void interpret_icmp(struct vm_state* state) {
  lift_binop(&state->data_stack, i64_cmp);
}

void interpret_ineg(struct vm_state* state) {
  lift_unop(&state->data_stack, i64_neg);
}

// src: https://stepik.org/lesson/1443634/step/16?unit=1462435
void interpret(struct vm_state* state, ins_interpreter* const(actions)[]) {
  for (; state->ip;) {
    const union ins* ins = state->ip;

    const struct ins_descr* ins_descr = instructions + ins->opcode;

    if (state->data_stack.count < ins_descr->min_stack) {
      printf("Stack underflow\n");
      return;
    }

    if (state->data_stack.count + ins_descr->stack_delta >
        state->data_stack.data.size) {
      printf("Stack overflow\n");
      return;
    }

    actions[ins->opcode](state);

    if (state->ip && !ins_descr->affects_ip) {
      state->ip = state->ip + 1;
    }
  }
}
