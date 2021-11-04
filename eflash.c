#include <stdint.h>
#include "ac78xx.h"
#include "ac78xx_eflash.h"
#include "eflash.h"


uint32_t eFlashVICode = 0xFF;

uint32_t EFLASH_ReadVICode(void)
{
    return EFLASH_ReadDWord(EFLASH_PAGE63_START);
}

void EFLASH_Init(void)//CAN初始化后再运行
{
    eFlashVICode = EFLASH_ReadVICode();
	//如果数据无效，上报读取EFLASH错误
}

void EFLASH_WriteVICode(uint32_t data)
{
	uint8_t ret;
	EFLASH_UnlockCtrl();
	ret = EFLASH_PageErase(EFLASH_PAGE63_START);     ///<erase page
	if (ret != EFLASH_STATUS_SUCCESS) return;
	ret = EFLASH_PageEraseVerify(EFLASH_PAGE63_START);   ///< verify erase state
	if (ret != EFLASH_STATUS_SUCCESS) return;
	//EFLASH_LockCtrl();      

	//EFLASH_UnlockCtrl();        
	ret = EFLASH_PageProgram(EFLASH_PAGE63_START, &data, 1);
	EFLASH_LockCtrl();
	
}


