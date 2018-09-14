// BSD 3-Clause License
//
// Copyright (c) 2018, Gregory Meyer
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "internal/str.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

TreeErrorE StringView_init(StringView *self, const char *str, size_t len) {
    assert(self);
    assert(str);

    self->data = str;
    self->length = len;

    return TREE_SUCCESS;
}

static char* alloc_and_copy(StringView source) {
    char *const new_str = malloc(sizeof(char) * (source.length + 1));

    if (!new_str) {
        return NULL;
    }

    memcpy(new_str, source.data, source.length);
    new_str[source.length + 1] = '\0';

    return new_str;
}

TreeErrorE StringView_copy(StringView self, String *dest) {
    assert(dest);

    char *const new_data = alloc_and_copy(self);

    if (!new_data) {
        return TREE_NO_MEMORY;
    }

    if (dest->data) {
        free(dest->data);
    }

    dest->data = new_data;
    dest->length = self.length;

    return TREE_SUCCESS;
}

TreeErrorE String_init(String *self) {
    assert(self);

    char *const allocated = calloc(1, sizeof(char));

    if (!allocated) {
        return TREE_NO_MEMORY;
    }

    self->data = allocated;
    self->length = 0;

    return TREE_SUCCESS;
}

TreeErrorE String_destroy(String *self) {
    assert(self);

    free(self->data);
    self->data = NULL;
    self->length = 0;

    return TREE_SUCCESS;
}
