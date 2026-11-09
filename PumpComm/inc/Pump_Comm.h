/**
  ******************************************************************************
  * @file       Pump_Comm.c
  * @author     qinxingpeng
  * @version    V1.0
  * @date       2023年043月23日21点47分
  * @brief      泵通讯层
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef Pump_COMM_H
#define Pump_COMM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif 

typedef void (*fpPump_CommTx)(uint8_t *pData,uint16_t size);

/**
 * @brief   初始化Pump协议接口
 * @param  	fpPump_CommTx fp		数据发送接口
 *			uint8_t addr		从机地址，协议将会使用这个数据作为接收帧头
 * @retval	None
 */
extern void Pump_CommInit(fpPump_CommTx fp,uint8_t addr);

/**
 * @brief   接口串口中断数据
 * @param  	uint8_t byte 		串口接收到的一共字节的数据
 * @retval	None
 */
extern void Pump_CommRxByte(uint8_t byte);

/**
 * @brief   周期性调用，用于处理接收到的数据
 * @param  	None
 * @retval	None
 */
extern void Pump_CommTaskEntry(void);

#ifdef __cplusplus
}
#endif 

#endif

/******************* (C) COPYRIGHT 2023 qinxingpeng *****END OF FILE****/
