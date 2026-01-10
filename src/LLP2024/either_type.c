// src: https://stepik.org/lesson/1443631/step/10?unit=1462432

enum either_type { ET_INT, ET_STRING };

struct either_int_string {
  enum either_type type;
  union {
    char* as_string;
    int64_t as_int;
  };
};

// Instantiating from a string or from a number
// A heap allocated string is always passed
struct either_int_string either_from_string(char* s) {
  struct either_int_string res = {};
  if (s) {
    res.type = ET_STRING;
    res.as_string = s;
  }
  return res;
}

struct either_int_string either_from_int(int64_t i) {
  return (struct either_int_string){.type = ET_INT, .as_int = i};
}

// if string - clear memory for string
void either_int_string_destroy(struct either_int_string e) {
  if (e.type == ET_STRING) {
    if (e.as_string) free(e.as_string);
  }
}

// switch construct is used for branching by number value,
// for example, enumeration.
// Cannot be used for strings and other data types
// Each branch must end with break

void print(struct either_int_string e) {
  switch (e.type) {
    case ET_INT: {
      printf("Int %" PRId64, e.as_int);
      break;
    }
    case ET_STRING: {
      printf("String %s", e.as_string);
      break;
    }
    // Default branch if e.type is neither ET_INT nor ET_STRING
    default: {
      break;
    }
  }
}
