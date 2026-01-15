// src: https://stepik.org/lesson/1443634/step/10?unit=1462435

/* Description of instructions (see previous step) */
enum opcode { BC_PUSH, BC_IPRINT, BC_IREAD, BC_IADD, BC_STOP };

struct bc_noarg {
  enum opcode opcode;
};
struct bc_arg64 {
  enum opcode opcode;
  int64_t arg;
};
union ins {
  enum opcode opcode;
  struct bc_arg64 as_arg64;
  struct bc_noarg as_noarg;
};

/* ------------------------ */

struct vm_state {
  const union ins* ip;
  struct stack data_stack;
};

/*struct stack {
  size_t count;
  struct array_int data;
};*/

/* The initial capacity of the stack is given by the definition STACK_CAPACITY
 */
struct vm_state state_create(const union ins* ip) {
  return (struct vm_state){.ip = ip,
                           .data_stack = stack_create(STACK_CAPACITY)};
}

// How to properly deinitialize the state, free all the resources?
void state_destroy(struct vm_state* state) {
  if (state) {
    stack_destroy(&state->data_stack);
    state->ip = NULL;
  }
}

/* You can use these functions: */
void print_int64(int64_t);
struct maybe_int64 maybe_read_int64();

struct stack stack_create(size_t size);
void stack_destroy(struct stack* s);
bool stack_push(struct stack* s, int64_t value);
struct maybe_int64 stack_pop(struct stack* s);

/* Describe an interpretation cycle with fetching and executing commands (until
 * we execute STOP) */
void interpret(struct vm_state* state) {
  if (state && state->ip) {
    while (1) {
      const union ins cur_ip = *state->ip;
      state->ip++;

      switch (cur_ip->opcode) {
        case BC_PUSH:
          if (stack_push(&state->data_stack, cur_ip.as_arg64.arg) == false)
            return;
          break;
        case BC_IPRINT: {
          struct maybe_int64 num = stack_pop(&state->data_stack);
          if (num.valid) {
            print_int64(num.value);
          } else {
            return;
          }
          break;
        }
        case BC_IREAD: {
          struct maybe_int64 num = maybe_read_int64();
          if (num.valid) {
            if (stack_push(&state->data_stack, num.value) == false) return;
          }
          break;
        }
        case BC_IADD: {
          struct maybe_int64 num2 = stack_pop(&state->data_stack);
          struct maybe_int64 num1 = stack_pop(&state->data_stack);
          if (num1.valid && num2.valid) {
            if (stack_push(&state->data_stack, num1.value + num2.value) ==
                false)
              return;
          }
          break;
        }

        case BC_STOP:
          return;

        default:
          return;
      }  // end of switch /opcode/

    }  // end of while == opcode=bc_stop
  }
}

void interpret_program(const union ins* program) {
  struct vm_state state = state_create(program);
  interpret(&state);
  state_destroy(&state);
}
