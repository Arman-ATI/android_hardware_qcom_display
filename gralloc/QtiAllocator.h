/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __QTIALLOCATOR_H__
#define __QTIALLOCATOR_H__

#include <hidl/Status.h>
#include <vendor/qti/hardware/display/allocator/4.0/IQtiAllocator.h>

#include <memory>

#include "QtiAllocatorAIDL.h"

namespace vendor::qti::hardware::display::allocator::V4_0::implementation {

using ::aidl::android::hardware::graphics::allocator::impl::QtiAllocatorAIDL;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;

// Exposes the AIDL allocator over HIDL for blobs that still use IQtiAllocator.
class QtiAllocator : public IQtiAllocator {
 public:
  explicit QtiAllocator(std::shared_ptr<QtiAllocatorAIDL> allocator);

  Return<void> allocate(const hidl_vec<uint8_t> &descriptor, uint32_t count,
                        allocate_cb hidl_cb) override;

 private:
  std::shared_ptr<QtiAllocatorAIDL> allocator_;
};

}  // namespace vendor::qti::hardware::display::allocator::V4_0::implementation

#endif  // __QTIALLOCATOR_H__
