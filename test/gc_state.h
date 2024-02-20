/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef GC_STATE_H
#define GC_STATE_H

#include "natrix/util/gc.h"
#include "natrix/util/gc_internals.h"

class GcStateW {
public:
    GcStateW() : state(gc_get_internal_state()) {
        reset();
    }

    ~GcStateW() {
        reset();
    }

    void reset() {
        state->head = nullptr;
        state->objects_count = 0;
        state->threshold = 100;
        state->roots_count = 0;
    }

    bool is_valid(const void *obj) const {
        GcHeader *current = state->head;
        while (current != nullptr) {
            if (current == obj) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    [[nodiscard]] bool check_count(size_t expected_count) const {
        size_t count = 0;
        GcHeader *current = state->head;
        while (current != nullptr) {
            count++;
            current = current->next;
        }
        return count == state->objects_count && count == expected_count;
    }

    [[nodiscard]] size_t threshold() const {
        return state->threshold;
    }

private:
    GcState *state;
};

#endif //GC_STATE_H
