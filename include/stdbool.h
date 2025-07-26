#ifndef STDBOOL_H
#define STDBOOL_H

// Define boolean type and constants
typedef enum {
    false = 0,
    true = 1
} bool;

// For C99 compatibility
#define __bool_true_false_are_defined 1

#endif
