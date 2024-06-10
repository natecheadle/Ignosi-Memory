#pragma once

#include <boost/pool/object_pool.hpp>

#include "boost_pool_allocator.h"
#include "dll_object_pool.h"

namespace ignosi::memory::detail {

class DllObjectPoolWrapper {
  boost::pool<BoostPoolAllocator> m_Pool;

 public:
  DllObjectPoolWrapper(BoostPoolAllocator::size_type size) : m_Pool(size) {}
};

}  // namespace ignosi::memory::detail
