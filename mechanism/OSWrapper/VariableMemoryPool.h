#ifndef OS_WRAPPER_VARIABLE_MEMORY_POOL_H_INCLUDED
#define OS_WRAPPER_VARIABLE_MEMORY_POOL_H_INCLUDED

#include <cstddef>

namespace OSWrapper {

class VariableMemoryPoolFactory;

/*!
 * @brief Register the VariableMemoryPoolFactory
 * @param factory Pointer of the object of the concrete class derived from VariableMemoryPoolFactory
 *
 * @note You need to call this function only one time when the application is initialized.
 */
void registerVariableMemoryPoolFactory(VariableMemoryPoolFactory* factory);

/*!
 * @brief Abstract class that has functions of common RTOS's variable-size memory pool
 */
class VariableMemoryPool {
protected:
	virtual ~VariableMemoryPool() {}

public:
	/*!
	 * @brief Create a VariableMemoryPool object
	 * @param memoryPoolSize Number of bytes of memory pool
	 * @param memoryPoolAddress Memory pool address
	 * @return If this method succeeds then returns a pointer of VariableMemoryPool object, else returns null pointer
	 *
	 * @note If memoryPoolAddress is null pointer, using memory pool area that RTOS prepared.
	 */
	static VariableMemoryPool* create(std::size_t memoryPoolSize, void* memoryPoolAddress = 0);

	/*!
	 * @brief Destroy a VariableMemoryPool object
	 * @param p Pointer of VariableMemoryPool object created by VariableMemoryPool::create()
	 *
	 * @note If p is null pointer, do nothing.
	 */
	static void destroy(VariableMemoryPool* p);

	/*!
	 * @brief Allocate a memory from this VariableMemoryPool
	 * @param size Number of bytes of allocation
	 * @return If required free areas exist in the memory pool then returns a pointer of allocated memory, else returns null pointer
	 */
	virtual void* allocate(std::size_t size) = 0;

	/*!
	 * @brief Release the allocated memory
	 * @param p Pointer of allocated memory
	 *
	 * @note If p is null pointer, do nothing.
	 */
	virtual void deallocate(void* p) = 0;

};

}

#endif // OS_WRAPPER_VARIABLE_MEMORY_POOL_H_INCLUDED
