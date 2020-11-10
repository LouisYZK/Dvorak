#include <stdlib.h>

 
#define FIRST_LEVEL_BIT_ 16
#define SECOND_LEVEL_BIT_ 16
enum AccessMode
{
    MEMORY = 0,
    DIS = 1
};

template <class T>
class MemBuffer
{
    public:
        MemBuffer(const char* fileName = NULL,
                  uint16_t firstLevel = FIRST_LEVEL_BIT_,
                  uint16_t secondLevel = SECOND_LEVEL_BIT_,
                  int mode = MEMORY);

        MemBuffer(uint16_t firstLevel, uint16_t secondLevel);

        ~MemBuffer();

        MemBuffer(const MemBuffer& rhs) = delete;

        uint32_t AddObject(const T& obj);
        uint32_t AddObjects(const T* objs, int nLength);

        T* GetObj(uint32_t nIndex);
        const T* GetObj(uint32_t nIndex);

        bool IsEmpty() const;

        void Init(const T& val, uint32_t size);
}