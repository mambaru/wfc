#include <cassert>
#include <string>

namespace wfc{

namespace detail
{
  void* allocate_aligned_memory(size_t align, size_t size)
  {
    assert(align >= sizeof(void*));
    assert((align & (align - 1)) == 0);

    if (size == 0)
    {
      return nullptr;
    }

    void* ptr = nullptr;
    int rc = posix_memalign(&ptr, align, size);
    if (rc != 0)
    {
      return nullptr;
    }

    return ptr;
  }

  void deallocate_aligned_memory(void* ptr) noexcept
  {
    return free(ptr);
  }
}

}
