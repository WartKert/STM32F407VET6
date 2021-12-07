/*
 * DMA.h
 *
 *  Created on: 2 мая 2020 г.
 *      Author: SerV
 */

#ifndef INC_DMA_H_
#define INC_DMA_H_

#include "stm32f407xx.h"
#include "stdint.h"

enum class DMA_Num : uint32_t {
	DMA1_ = DMA1_BASE,
	DMA2_ = DMA2_BASE
};


enum class DMA_Stream : uint32_t {
	DMA1_Stream0_ = DMA1_Stream0_BASE,
	DMA1_Stream1_ = DMA1_Stream1_BASE,
	DMA1_Stream2_ = DMA1_Stream2_BASE,
	DMA1_Stream3_ = DMA1_Stream3_BASE,
	DMA1_Stream4_ = DMA1_Stream4_BASE,
	DMA1_Stream5_ = DMA1_Stream5_BASE,
	DMA1_Stream6_ = DMA1_Stream6_BASE,
	DMA1_Stream7_ = DMA1_Stream7_BASE,

	DMA2_Stream0_ = DMA2_Stream0_BASE,
	DMA2_Stream1_ = DMA2_Stream1_BASE,
	DMA2_Stream2_ = DMA2_Stream2_BASE,
	DMA2_Stream3_ = DMA2_Stream3_BASE,
	DMA2_Stream4_ = DMA2_Stream4_BASE,
	DMA2_Stream5_ = DMA2_Stream5_BASE,
	DMA2_Stream6_ = DMA2_Stream6_BASE,
	DMA2_Stream7_ = DMA2_Stream7_BASE
};


/*	Функции преобразования типов	*/
constexpr auto DMA_Conv (DMA_Stream ch) {
	return reinterpret_cast<DMA_Stream_TypeDef*>(ch);
}
constexpr auto DMA_Conv (DMA_Num ch) {
	return reinterpret_cast<DMA_TypeDef*>(ch);
}



#endif /* INC_DMA_H_ */
