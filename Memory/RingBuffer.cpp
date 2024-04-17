#include "stdafx.h"

#include "RingBuffer.h"
#include "..\Diagnostics\Debugging.h"

void RingBuffer::Initialize(uint32_t size)
{
	m_Size = size;
	m_StartAllocated = 0;
	m_EndAllocated = 0;
}

uint32_t RingBuffer::Allocate(uint32_t size, uint32_t&endOffset)
{
	Assert(size < m_Size);
	uint32_t startFree = m_StartAllocated + m_Size;	// ring buffer
	// Check if we have enough space
	if (m_EndAllocated + size <= startFree)
	{
		// check for wrap
		if ((m_EndAllocated / m_Size) < ((m_EndAllocated + size) / m_Size))
		{
			m_EndAllocated = (m_EndAllocated / m_Size + 1) * m_Size; // wrap to start of buffer
			if (m_EndAllocated + size > startFree) // check again for enough space
			{
				return -1;
			}
		}
		uint32_t res = m_EndAllocated % m_Size; // convert to buffer space
		m_EndAllocated += size;
		endOffset = m_EndAllocated;	
		return res;
	}
	return -1;
}

void RingBuffer::Free(uint32_t offset)
{
	Assert(offset > m_StartAllocated);
	Assert(offset <= m_EndAllocated);
	m_StartAllocated = offset;
	if (m_StartAllocated + m_Size < m_StartAllocated) // 32 bit wrap
	{
		m_EndAllocated = m_EndAllocated % (m_Size * 2);	// * 2 to make sure m_EndAllocated is always ahead of m_StartAllocated
		m_StartAllocated = m_StartAllocated % (m_Size * 2);
	}
}

void RingBuffer::Test()
{	
	RingBuffer* pRingBuffer = new RingBuffer();
	pRingBuffer->Initialize(64);

	// Test normal allocation
	uint32_t endOffset;
	uint32_t offset = pRingBuffer->Allocate(32, endOffset);
	Assert(offset == 0);

	// Test correct buffer offset after 2nd allocation
	uint32_t endOffset1;
	uint32_t offset1 = pRingBuffer->Allocate(16, endOffset1);
	Assert(offset1 - offset == 32);

	// Test out of buffer space
	uint32_t endOffset2;
	uint32_t offset2 = pRingBuffer->Allocate(32, endOffset2);
	Assert(offset2 == -1);

	// Test freeing buffer space and allocating with wrap around
	pRingBuffer->Free(endOffset);
	uint32_t offset3;
	uint32_t endOffset3;
	offset3 = pRingBuffer->Allocate(32, endOffset3);
	Assert(offset3 == 0); // Should have wrapped around

	// Test for wrap and failed allocation after wrap
	pRingBuffer->Initialize(64);
	uint32_t offset4;
	uint32_t endOffset4;
	offset4 = pRingBuffer->Allocate(16, endOffset4);
	uint32_t offset5;
	uint32_t endOffset5;
	offset5 = pRingBuffer->Allocate(32, endOffset5);
	pRingBuffer->Free(endOffset4);
	uint32_t offset6;
	uint32_t endOffset6;
	offset6 = pRingBuffer->Allocate(32, endOffset6);
	Assert(offset6 == -1);
	delete pRingBuffer;
}