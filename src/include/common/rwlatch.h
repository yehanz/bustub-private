//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// rwmutex.h
//
// Identification: src/include/common/rwlatch.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <mutex>  // NOLINT
#include <shared_mutex>

#include "common/macros.h"

namespace bustub {

/**
 * Reader-Writer latch backed by std::mutex.
 */
class ReaderWriterLatch {
 public:
  /**
   * Acquire a write latch.
   */
  void WLock() { mutex_.lock(); }

  /**
   * Release a write latch.
   */
  void WUnlock() { mutex_.unlock(); }

  /**
   * Acquire a read latch.
   */
  void RLock() { mutex_.lock_shared(); }

  /**
   * Release a read latch.
   */
  void RUnlock() { mutex_.unlock_shared(); }

 private:
  std::shared_mutex mutex_;
};

class AutoReaderLatch {
 public:
  AutoReaderLatch(ReaderWriterLatch* latch): latch_(latch) {
      latch_->RLock();
  }

  ~AutoReaderLatch() {
      latch_->RUnlock();
  }
 private:
  ReaderWriterLatch* latch_;
};

class AutoWriterLatch {
 public:
  AutoWriterLatch(ReaderWriterLatch* latch): latch_(latch) {
      latch_->WLock();
  }

  ~AutoWriterLatch() {
      latch_->WUnlock();
  }
 private:
  ReaderWriterLatch* latch_;
};


}  // namespace bustub
