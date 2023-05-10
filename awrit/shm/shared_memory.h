// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_SHARED_MEMORY_H
#define AWRIT_SHARED_MEMORY_H

#include <string>

namespace shm {

/**
 * Produces a shared memory or temporary file with the contents of the provided
 * buffer.
 *
 * @returns empty if the write was a failure, otherwise returns the filename
 */
std::string write(const void* buffer, size_t size);

}  // namespace shm

#endif  // AWRIT_SHARED_MEMORY_H
