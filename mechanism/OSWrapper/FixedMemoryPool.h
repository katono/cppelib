#ifndef OS_WRAPPER_FIXED_MEMORY_POOL_H_INCLUDED
#define OS_WRAPPER_FIXED_MEMORY_POOL_H_INCLUDED

#include <cstddef>
#include "Timeout.h"
#include "OSWrapperError.h"

namespace OSWrapper {

class FixedMemoryPoolFactory;

/*!
 * @brief Register the FixedMemoryPoolFactory
 * @param factory Pointer of the object of the concrete class derived from FixedMemoryPoolFactory
 *
 * @note You need to call this function only one time when the application is initialized.
 */
void registerFixedMemoryPoolFactory(FixedMemoryPoolFactory* factory);

/*!
 * @brief Abstract class that has functions of common RTOS's fixed-size memory pool
 */
class FixedMemoryPool {
protected:
	virtual ~FixedMemoryPool() {}

public:
	/*!
	 * @brief Create a FixedMemoryPool object
	 * @param blockSize Number of bytes of one fixed-size memory block
	 * @param memoryPoolSize Number of bytes of memory pool
	 * @param memoryPoolAddress Memory pool address
	 * @return If this method succeeds then returns a pointer of FixedMemoryPool object, else returns null pointer
	 *
	 * @note If memoryPoolAddress is null pointer, using memory pool area that RTOS prepared.
	 */
	static FixedMemoryPool* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress = 0);

	/*!
	 * @brief Destroy a FixedMemoryPool object
	 * @param p Pointer of FixedMemoryPool object created by FixedMemoryPool::create()
	 *
	 * @note If p is null pointer, do nothing.
	 */
	static void destroy(FixedMemoryPool* p);

	/*!
	 * @brief Get the required total memory size for allocation of (blockSize * numBlocks)
	 * @param blockSize Number of bytes of one fixed-size memory block
	 * @param numBlocks Number of blocks
	 * @return Required total memory size
	 *
	 * @note Return value does not always equal to (blockSize * numBlocks).
	 */
	static std::size_t getRequiredMemorySize(std::size_t blockSize, std::size_t numBlocks);

	/*!
	 * @brief Allocate a block from this FixedMemoryPool
	 * @return If free blocks exist in the memory pool then returns a pointer of allocated block, else returns null pointer
	 */
	virtual void* allocate() = 0;

	/*!
	 * @brief Release the allocated block
	 * @param p Pointer of allocated block
	 *
	 * @note If p is null pointer, do nothing.
	 */
	virtual void deallocate(void* p) = 0;

	/*!
	 * @brief Get the block size
	 * @return Number of bytes of one fixed-size memory block
	 */
	virtual std::size_t getBlockSize() const = 0;

	/*!
	 * @brief Block the current thread until a memory block is allocated from this FixedMemoryPool
	 * @param[out] memory Pointer of pointer that stores the allocated memory
	 * @retval OK Success. A memory is allocated
	 * @retval InvalidParameter Parameter memory is null pointer
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 * @retval OtherError Not implemented in concrete class
	 *
	 * @note Call deallocate() to release a memory block allocated by this method
	 * @note Same as timedAllocateMemory(memory, Timeout::FOREVER)
	 */
	virtual Error allocateMemory(void** memory);

	/*!
	 * @brief Try to allocate a memory block from this FixedMemoryPool without blocking
	 * @param[out] memory Pointer of pointer that stores the allocated memory
	 * @retval OK Success. A memory is allocated
	 * @retval TimedOut Failed. No memory blocks available
	 * @retval InvalidParameter Parameter memory is null pointer
	 * @retval OtherError Not implemented in concrete class
	 *
	 * @note Call deallocate() to release a memory block allocated by this method
	 * @note Same as timedAllocateMemory(memory, Timeout::POLLING)
	 */
	virtual Error tryAllocateMemory(void** memory);

	/*!
	 * @brief Block the current thread until a memory block is allocated from this FixedMemoryPool but only within the limited time
	 * @param[out] memory Pointer of pointer that stores the allocated memory
	 * @retval OK Success. A memory is allocated
	 * @retval TimedOut The limited time was elapsed
	 * @retval InvalidParameter Parameter memory is null pointer
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 * @retval OtherError Not implemented in concrete class
	 *
	 * @note Call deallocate() to release a memory block allocated by this method
	 * @note If tmout is Timeout::POLLING then this method tries to allocate a memory block without blocking.
	 * @note If tmout is Timeout::FOREVER then this method waits forever until a memory block is allocated.
	 */
	virtual Error timedAllocateMemory(void** memory, Timeout tmout);

	/*!
	 * @brief Get the remaining number of available blocks in this FixedMemoryPool
	 * @return Remaining number of available blocks
	 *
	 * @note If the concrete class does not implement this method, this method returns 0
	 */
	virtual std::size_t getNumberOfAvailableBlocks() const;

	/*!
	 * @brief Get the max number of blocks in this FixedMemoryPool
	 * @return Max number of blocks
	 *
	 * @note If the concrete class does not implement this method, this method returns 0
	 */
	virtual std::size_t getMaxNumberOfBlocks() const;
};

}

#endif // OS_WRAPPER_FIXED_MEMORY_POOL_H_INCLUDED
