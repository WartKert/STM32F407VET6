/*
 * FSMC.h
 *
 *  Created on: 11 апр. 2020 г.
 *      Author: SerV
 *      Файл описания интерфейса работы контоллера FSMC.
 *      Шина адреса и данных не совмещена.
 *
 *
 */
#include <type_traits>
#include <typeinfo>
#include "stm32f4xx.h"
#include <cassert>

#ifndef INC_FSMC_H_
#define INC_FSMC_H_

enum class SubBank_Bank1 : uint32_t {
	SubBank1	= 0,//0x60000000,
	SubBank2	= 2,//0x64000000,
	SubBank3	= 4,//0x68000000,
	SubBank4	= 6//0x6C000000
};
enum class Bank1_Mode : uint32_t {
	Mode1A	= 0,
	Mode12B	= 1 << 28,
	Mode1C	= 2 << 28,
	Mode1D	= 3 << 28
};


class FSMC
{
private:
	static constexpr uint32_t adr_SubBank1 = 0x60000000;
	static constexpr uint32_t adr_SubBank2 = 0x64000000;
	static constexpr uint32_t adr_SubBank3 = 0x68000000;
	static constexpr uint32_t adr_SubBank4 = 0x6C000000;




public:

	virtual ~FSMC(){}

	FSMC(){}

	/* Конфигурация банка 1. Работа в асинхронном режиме	*/
	template <SubBank_Bank1 subBank, Bank1_Mode mode, uint32_t rclkBus, uint32_t rclkData, uint32_t rclkAdr>
	uint32_t ConfBank1 (){
		/* Конфигурация FSMC (mode 2) */
		FSMC_Bank1 -> BTCR[static_cast<uint32_t>(subBank)] = 0x1059;
		FSMC_Bank1 -> BTCR[static_cast<uint32_t>(subBank) + 1] = FSMC_BTR1_ACCMOD_0 | (rclkBus << 16) | (rclkData << 8) | (rclkAdr << 0);
		//FSMC_Bank1E -> BWTR[static_cast<uint32_t>(subBank)] = FSMC_BWTR1_ACCMOD_0 | (wclkBus << 16) | (wclkData << 8) | (wclkAdr << 0);

		return (subBank == SubBank_Bank1::SubBank1)? adr_SubBank1 : (subBank == SubBank_Bank1::SubBank2)? adr_SubBank2 : \
				(subBank == SubBank_Bank1::SubBank3)? adr_SubBank3 : (subBank == SubBank_Bank1::SubBank4)? adr_SubBank4 : 0u;
	}
protected:


};


#endif /* INC_FSMC_H_ */
