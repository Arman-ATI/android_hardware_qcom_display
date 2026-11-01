/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "QtiAllocator.h"

#include <aidl/android/hardware/graphics/allocator/AllocationError.h>
#include <aidl/android/hardware/graphics/allocator/AllocationResult.h>
#include <aidlcommonsupport/NativeHandle.h>
#include <android/hardware/graphics/mapper/4.0/types.h>
#include <cutils/native_handle.h>

#include <vector>

namespace vendor::qti::hardware::display::allocator::V4_0::implementation {

using ::aidl::android::hardware::graphics::allocator::AllocationError;
using ::aidl::android::hardware::graphics::allocator::AllocationResult;
using ::android::hardware::hidl_handle;
using ::android::hardware::Void;
using MapperError = ::android::hardware::graphics::mapper::V4_0::Error;

static MapperError ToMapperError(const ndk::ScopedAStatus &status) {
  if (status.getExceptionCode() != EX_SERVICE_SPECIFIC) {
    return MapperError::NO_RESOURCES;
  }

  switch (static_cast<AllocationError>(status.getServiceSpecificError())) {
    case AllocationError::BAD_DESCRIPTOR:
      return MapperError::BAD_DESCRIPTOR;
    case AllocationError::UNSUPPORTED:
      return MapperError::UNSUPPORTED;
    default:
      return MapperError::NO_RESOURCES;
  }
}

QtiAllocator::QtiAllocator(std::shared_ptr<QtiAllocatorAIDL> allocator)
    : allocator_(std::move(allocator)) {}

Return<void> QtiAllocator::allocate(const hidl_vec<uint8_t> &descriptor, uint32_t count,
                                    allocate_cb hidl_cb) {
  AllocationResult result;
  std::vector<uint8_t> aidl_descriptor(descriptor.begin(), descriptor.end());

  auto status = allocator_->allocate(aidl_descriptor, static_cast<int32_t>(count), &result);
  if (!status.isOk()) {
    hidl_cb(ToMapperError(status), 0, hidl_vec<hidl_handle>());
    return Void();
  }

  std::vector<native_handle_t *> handles;
  hidl_vec<hidl_handle> buffers(result.buffers.size());
  handles.reserve(result.buffers.size());
  for (size_t i = 0; i < result.buffers.size(); i++) {
    handles.push_back(::android::dupFromAidl(result.buffers[i]));
    buffers[i] = handles[i];
  }

  hidl_cb(MapperError::NONE, static_cast<uint32_t>(result.stride), buffers);

  for (native_handle_t *handle : handles) {
    native_handle_close(handle);
    native_handle_delete(handle);
  }

  return Void();
}

}  // namespace vendor::qti::hardware::display::allocator::V4_0::implementation
