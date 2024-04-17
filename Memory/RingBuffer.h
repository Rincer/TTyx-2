#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

class RingBuffer
{
	public:
		void Initialize(uint32_t size);
		uint32_t Allocate(uint32_t size, uint32_t& endOffset);
		void Free(uint32_t handle);

		static void Test();

	private:		
		uint32_t m_Size;
		uint32_t m_StartAllocated;
		uint32_t m_EndAllocated;
};

#endif
