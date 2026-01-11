// src: https://stepik.org/lesson/1443632/step/11?unit=1462433
//
enum move_dir { MD_UP, MD_RIGHT, MD_DOWN, MD_LEFT, MD_NONE };

// Determine the type of event handler move_callback with typedef
typedef void(move_callback)(enum move_dir);

struct callback_node {
  move_callback* handler;
  struct callback_node* next;
};

// The robot and his callbacks
// The number of callbacks is unlimited
struct robot {
  const char* name;
  struct callback_node* callbacks;
};

// Add a callback to the robot so that it is called when moving
// The direction of movement will be transmitted to the callback
void register_callback(struct robot* robot, move_callback new_cb) {
  struct callback_node* node = malloc(sizeof(struct callback_node));
  if (node && robot) {
    node->handler = new_cb;
    node->next = robot->callbacks;
    robot->callbacks = node;
  }
}

// Cancel all event subscriptions.
// This is necessary to release reserved resources for example,
//  memory allocated in the heap.
void unregister_all_callbacks(struct robot* robot) {
  if (!robot || !robot->callbacks) return;

  struct callback_node *current = robot->callbacks, *next = NULL;

  while (current) {
    next = current->next;
    free(current);
    current = next;
  }
  robot->callbacks = NULL;
}

// Calls when the robot is moving
// This function should call all event handlers
void move(struct robot* robot, enum move_dir dir) {
  if (!robot || !robot->callbacks) return;
  struct callback_node* current = robot->callbacks;
  while (current) {
    current->handler(dir);
    current = current->next;
  }
}
