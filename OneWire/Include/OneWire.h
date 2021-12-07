/*
 * OneWire.h
 *
 *  Created on: 30 апр. 2019 г.
 *      Author: Константин
 */
/*********************************************************************************************************************************************************************************************/
/*			ОПИСАНИЕ			*/
/*********************************************************************************************************************************************************************************************/
/* Файл для работы с шиной 1-WIRE ]
 *
 *
 *
 *
 *
 */


#ifndef ONEWIRE_H_
#define ONEWIRE_H_
#ifdef FREERTOS_CONFIG_H
#include "task.h"
#endif
//#include "stm32l1xx.h"
#include "stm32f4xx.h"
#include "cmsis_gcc.h"
#include "stdint.h"
#include <cassert>
#include <cmath>

const uint32_t tye = 5;




enum class enum_USART : uint32_t
{
	USART_1,
	USART_2,
	USART_3,
	noun
};

enum class enum_USART_PIN : uint32_t
{
	USART1_PA9				= 9,
	USART1_PB6				= 6,
	USART2_PA2				= 2,
	USART2_PD5				= 5,
	USART3_PB10				= 10,
	USART3_PC10				= 10,
	USART3_PD8				= 8
};

#if defined (STM32L152xx)

#endif
#if defined (STM32F407xx)
	DMA_TypeDef* DMA_Adr = DMA2;
	DMA_Stream_TypeDef* DMA_Stream_TX_Adr = DMA2_Stream7;
	DMA_Stream_TypeDef* DMA_Stream_RX_Adr = DMA2_Stream2;
#endif



template <uint32_t pclk, uint32_t over8>
struct test12
{
	constexpr static double_t* intpart {nullptr};
	constexpr static double_t value = pclk / (16 * 166000);
	double fractpart =  modf((pclk / (16 * 166000)), const_cast <double*>(intpart));
	uint32_t der = static_cast <uint32_t> (fractpart);
};




template <enum_USART_PIN pin>
void Init_Pin_for_USART ()
{
	// Настройка RCC
	(pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? RCC -> APB2ENR |= RCC_APB2ENR_USART1EN : \
		(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? RCC -> APB1ENR |= RCC_APB1ENR_USART2EN : \
				(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? RCC -> APB1ENR |= RCC_APB1ENR_USART3EN : 0;

	// Настройка GPIO
	GPIO_TypeDef* p_adr = (pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART2_PA2)? GPIOA : \
								(pin == enum_USART_PIN::USART1_PB6 || pin == enum_USART_PIN::USART3_PB10)? GPIOB : \
										(pin == enum_USART_PIN::USART3_PC10)? GPIOC : \
												(pin == enum_USART_PIN::USART2_PD5 || pin == enum_USART_PIN::USART3_PD8)? GPIOD : 0;
	// GPIOx_MODER |= Альтернативный режим работы необходимого пина
	p_adr -> MODER |= 2 << static_cast <uint32_t> (pin) * 2;
	// GPIOx_OTYPER |= пин open-drain
	p_adr -> OTYPER |= 1 << static_cast <uint32_t> (pin);
	// GPIOx_OSPEEDR |= скорость работы пина
	p_adr -> OSPEEDR |= 1 << static_cast <uint32_t> (pin) * 2;
	// GPIOx_PUPDR |= резистор pull-up
	p_adr -> PUPDR &= ~(1 << static_cast <uint32_t> (pin) * 2);
	// GPIOx_AFRL |= выбор альтернативной функции для пина (0 - 7)
	// GPIOx_AFRH |= выбор альтернативной функции для пина (8 -15)
	(static_cast <uint32_t> (pin) < 8)? p_adr -> AFR[0] |= 7u << (static_cast <uint8_t> (pin) * 4) : p_adr -> AFR[1] |= 7u << ((static_cast <uint8_t> (pin) - 8) *4);

}
template <enum_USART number>
void Init_USART ()
{
	static_assert (number, "no usart");
}
template <enum_USART_PIN pin, uint32_t clk_apb, uint32_t speed_usart = 170000>
void Init_USART ()
{
	// Настройка USART
	USART_TypeDef* USART_adr = (pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? USART1 : \
						(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? USART2 : \
								(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? USART3 : 0;
	// USART_BRR = установка скорости работы USART
	constexpr double_t usart_div = clk_apb / (16.0 * speed_usart);
	constexpr double_t div_fract = ((usart_div - static_cast<uint32_t> (usart_div)) * 16.0);
	constexpr uint32_t fract = ((static_cast <uint32_t> (div_fract) + 0.5) >= div_fract)? static_cast <uint32_t> (div_fract) \
																						: static_cast <uint32_t> (div_fract) + 1;
	constexpr uint32_t mantis = static_cast <uint32_t> (usart_div);

	USART_adr -> BRR = mantis << 4 | fract;//(static_cast <uint32_t> (intpart) << 4) | static_cast <uint32_t> (fractpart);
	// USART_CR3 = включение DMA на запись и чтение, включение single-wire
	USART_adr -> CR3 = USART_CR3_DMAT | USART_CR3_DMAR | USART_CR3_HDSEL;;
	// USART_CR2 = выбор полярности сигнала в режиме готовность как '1'
	USART_adr -> CR2 = USART_CR2_CPOL;// | USART_CR2_STOP_0;
	// USART_CR1 |= старт USART
	USART_adr -> CR1 |= USART_CR1_UE | USART_CR1_TE;// | USART_CR1_RE;

}
template <uint32_t clk_apb, enum_USART_PIN pin>
class ONE_WIRE
{

private:

	// Настройка USART
	 USART_TypeDef* USART = (pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? USART1 : \
						(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? USART2 : \
						(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? USART3 : 0;

	constexpr static uint8_t Data_Bit_Init = 0xF0;
	constexpr static uint8_t Data_Bit_0 = 0x00;
	constexpr static uint8_t Data_Bit_1 = 0xFF;

	const uint8_t Packet_Init { Data_Bit_Init };
	const uint8_t Packet_0 { Data_Bit_0 };
	const uint8_t Packet_1 { Data_Bit_1 };


	// команды для работы с устройствами 1-WIRE
	constexpr static uint8_t Search_Rom = 0xF0;
	constexpr static uint8_t Read_Rom = 0x33;
	constexpr static uint8_t Match_Rom = 0x55;
	constexpr static uint8_t Skip_Rom = 0xCC;
	constexpr static uint8_t Alarm_Search = 0xEC;

	// Проверка и сброс завершения посылки данных
	constexpr void Wait_Finish_Send ()
	{
#if defined (STM32L152xx)
		while(! ((DMA1 -> ISR & ((pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? DMA_ISR_TCIF4 : \
						(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? DMA_ISR_TCIF7 : \
				(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? DMA_ISR_TCIF2 : 0)) \
			&& (USART -> SR & USART_SR_TC)))
			{

			}
		DMA1 -> IFCR |= ((pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? DMA_IFCR_CTCIF4 : \
				(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? DMA_IFCR_CTCIF7 : \
		(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? DMA_IFCR_CTCIF2 : 0);
		DMA1_Channel4->CCR &= ~DMA_CCR_EN;
#endif
#if defined (STM32F407xx)
		while(! ((DMA_Adr -> HISR & DMA_HISR_TCIF7) && (USART -> SR & USART_SR_TC)))
		{

		}
		DMA_Adr -> HIFCR |= DMA_HIFCR_CTCIF7 | DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CFEIF7;
		DMA_Stream_TX_Adr -> CR &= ~ DMA_SxCR_EN;
		USART -> SR &= ~USART_SR_TC;
#endif
	}

	// Проверка и сброс завершения приёма данных
	constexpr void Wait_Finish_Receive ()
	{
#if defined (STM32L152xx)
		while(! (DMA1 -> ISR & ((pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? DMA_ISR_TCIF5 : \
						(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? DMA_ISR_TCIF6 : \
				(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? DMA_ISR_TCIF3 : 0)))
		{
		}
		DMA1 -> IFCR |= ((pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? DMA_IFCR_CTCIF5 : \
								(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? DMA_IFCR_CTCIF6 : \
										(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? DMA_IFCR_CTCIF3 : 0);
		DMA1_Channel5->CCR &= ~DMA_CCR_EN;
		// Для USART_RX
		USART -> CR1 &= ~USART_CR1_RE;
#endif
#if defined (STM32F407xx)
		while(! (DMA_Adr -> LISR & DMA_LISR_TCIF2))
		{

		}
		DMA_Adr -> LIFCR |= DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CFEIF2;
		DMA_Stream_RX_Adr -> CR &= ~ DMA_SxCR_EN;
		USART -> CR1 &= ~USART_CR1_RE;
#endif
	}

	// Функция отправки данных по шине 1-WIRE
	void Send_Data (const uint8_t a_len, bool a_inc, const uint8_t* a_data)
	{
		USART1->SR &= ~USART_SR_TC;
#if  defined (STM32L152xx)
		// Настройка DMA
		// Для USART_TX
		DMA_Channel_TypeDef* DMA_adr = (pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? DMA1_Channel4 : \
									(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? DMA1_Channel7 : \
											(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? DMA1_Channel2 : DMA1_Channel2;
		// Останов DMA
		DMA_adr -> CCR &= ~DMA_CCR_EN;
		// Адрес памяти для передачи
		DMA_adr -> CPAR = reinterpret_cast <uint32_t>(&USART1 -> DR);
		DMA_adr -> CMAR = reinterpret_cast <uint32_t> (a_data);
		// Количество байтов для передачи
		DMA_adr -> CNDTR = a_len;
		// Настройка и старт DMA
		DMA_adr -> CCR = (a_inc)? DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_EN : DMA_CCR_DIR | DMA_CCR_EN;
#endif
#if defined (STM32F407xx)
		// Останов DMA
		DMA_Stream_TX_Adr -> CR &= ~ DMA_SxCR_EN;
		// Адрес памяти для передачи
		DMA_Stream_TX_Adr -> PAR = reinterpret_cast <uint32_t>(&USART1 -> DR);
		DMA_Stream_TX_Adr -> M0AR = reinterpret_cast <uint32_t> (a_data);
		// Количество байтов для передачи
		DMA_Stream_TX_Adr -> NDTR = a_len;
		// Настройка и старт DMA
		DMA_Stream_TX_Adr -> CR = (DMA_SxCR_CHSEL_2 | DMA_SxCR_DIR_0 | DMA_SxCR_EN | ((a_inc)? DMA_SxCR_MINC : 0));
#endif
	}

	// Функция приёма данных по шине 1-WIRE
	void Receive_Data (uint8_t a_len, bool a_inc, uint8_t* a_data)
	{
		// Для USART_RX
#if  defined (STM32L152xx)
		// Настройка DMA
		DMA_Channel_TypeDef* DMA_adr = (pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? DMA1_Channel5 : \
						(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? DMA1_Channel6 : \
				(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? DMA1_Channel3 : 0;
		// Останов DMA
		DMA_adr -> CCR &= ~DMA_CCR_EN;
		// Адрес памяти для передачи
		DMA_adr -> CPAR = reinterpret_cast <uint32_t>(&USART1 -> DR);
		DMA_adr -> CMAR = reinterpret_cast <uint32_t> (a_data);
		// Количество байтов для передачи
		DMA_adr -> CNDTR = a_len;
		// Настройка и старт DMA
		DMA_adr -> CCR = (a_inc)? DMA_CCR_MINC | DMA_CCR_EN: DMA_CCR_EN;
#endif
#if defined (STM32F407xx)
		// Останов DMA
		DMA_Stream_RX_Adr -> CR &= ~ DMA_SxCR_EN;
		// Адрес памяти для передачи
		DMA_Stream_RX_Adr -> PAR = reinterpret_cast <uint32_t>(&USART1 -> DR);
		DMA_Stream_RX_Adr -> M0AR = reinterpret_cast <uint32_t> (a_data);
		// Количество байтов для передачи
		DMA_Stream_RX_Adr -> NDTR = a_len;
		// Настройка и старт DMA
		DMA_Stream_RX_Adr -> CR = (DMA_SxCR_CHSEL_2 | DMA_SxCR_EN | ((a_inc)? DMA_SxCR_MINC : 0));
		USART -> CR1 |= USART_CR1_RE;
#endif
	}

	// Проверка и сброс завершения пересылки и приёма данных
/*	inline void Wait_End_Transfer ()
	{
		while(! (DMA1 -> ISR & ((pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? DMA_ISR_TCIF4 : \
									(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? DMA_ISR_TCIF7 : \
									(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? DMA_ISR_TCIF2 : 0)\
							&& (pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? DMA_ISR_TCIF5 : \
									(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? DMA_ISR_TCIF6 : \
									(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? DMA_ISR_TCIF3 : 0))
		{
		}
		DMA1 -> IFCR |= ((pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? DMA_IFCR_CTCIF4 : \
							(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? DMA_IFCR_CTCIF7 : \
							(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? DMA_IFCR_CTCIF2 : 0)\
						| ((pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? DMA_IFCR_CTCIF5 : \
							(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? DMA_IFCR_CTCIF6 : \
							(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? DMA_IFCR_CTCIF3 : 0);
	}*/

	// Функция пересылки данных по шине 1-Wire
	/*__noinline void Transfer_Data (const uint8_t* a_txdata, const bool a_txinc, uint8_t* a_rxdata, const bool a_rxinc, const uint8_t a_len)
	{
		// Настройка DMA
		// Адрес канала для USART_TX
		DMA_Channel_TypeDef* DMA_TX_adr = (pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? DMA1_Channel4 : \
									(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? DMA1_Channel7 : \
											(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? DMA1_Channel2 : DMA1_Channel2;
		// Адрес канала для USART_RX
		DMA_Channel_TypeDef* DMA_RX_adr = (pin == enum_USART_PIN::USART1_PA9 || pin == enum_USART_PIN::USART1_PB6)? DMA1_Channel5 : \
									(pin == enum_USART_PIN::USART2_PA2 || pin == enum_USART_PIN::USART2_PD5)? DMA1_Channel6 : \
											(pin == enum_USART_PIN::USART3_PC10 || pin == enum_USART_PIN::USART3_PB10 || pin == enum_USART_PIN::USART3_PD8)? DMA1_Channel3 : 0;
		// Останов DMA TX
		DMA_TX_adr -> CCR &= ~DMA_CCR_EN;
		// Останов DMA RX
		DMA_RX_adr -> CCR &= ~DMA_CCR_EN;
		// Адрес памяти для передачи
		DMA_TX_adr -> CPAR = reinterpret_cast <uint32_t>(&USART1 -> DR);
		DMA_TX_adr -> CMAR = reinterpret_cast <uint32_t> (a_txdata);
		// Адрес памяти для передачи
		DMA_RX_adr -> CPAR = reinterpret_cast <uint32_t>(&USART1 -> DR);
		DMA_RX_adr -> CMAR = reinterpret_cast <uint32_t> (a_rxdata);
		// Количество байтов для передачи
		DMA_TX_adr -> CNDTR = a_len;
		// Количество байтов для передачи
		DMA_RX_adr -> CNDTR = a_len;
		__disable_irq();
		// Настройка и старт DMA TX
		DMA_TX_adr -> CCR = (a_txinc)? DMA_CCR_PL_0 | DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_EN : DMA_CCR_PL_0 | DMA_CCR_DIR | DMA_CCR_EN;
		// Настройка и старт DMA RX
		DMA_RX_adr -> CCR = (a_rxinc)? DMA_CCR_MINC | DMA_CCR_EN : DMA_CCR_EN;
		__enable_irq();
		Wait_End_Transfer();
	}
*/
	// Функция сброса всех устройств на шине 1-WIRE
	bool Reset_Presence ()
	{
		do
		{
			uint8_t get_data;
			// Сброс устройств на шине 1-WIRE
			Init_USART <pin, clk_apb, 9600>();
			#ifdef FREERTOS_CONFIG_H
					taskENTER_CRITICAL();
			#endif
			#ifndef FREERTOS_CONFIG_H
					__disable_irq();
			#endif
			Send_Data(1, false, p_Packet_Init);
			Receive_Data(1, false, &get_data);
			#ifndef FREERTOS_CONFIG_H
					__enable_irq();
			#endif
			#ifdef FREERTOS_CONFIG_H
					taskEXIT_CRITICAL();
			#endif
			Wait_Finish_Send();
			//Init_USART <pin, clk_apb, 60000>();
			//Send_Data(1, false, p_Packet_1);
			//	USART1 ->SR &= ~USART_SR_TC;
			//	USART1 -> CR1 &= ~USART_CR1_TE;
			// Проверка отклика от устройств на шине 1-WIRE
			//Wait_Finish_Send();
			Wait_Finish_Receive();
			if (get_data < 0xE6 && get_data > 0x96)
			{
				Init_USART <pin, clk_apb, 115200>();
				break;
			}
		//	else
			//{
				//return false;	// Устройств нет на шине 1-WIRE
			//}
		} while(1);
		return true;	// Устройства есть на шине 1-WIRE
	}


	// Функция приёма 64 битного ROM кода устройства 1-WIRE
	uint64_t Receive_ROM_Code (uint64_t& flag_of_rom, uint8_t& last_disp)
	{
		uint64_t temp_rom { 0 };
		uint8_t crc_value { 0 };

		for (uint8_t i = 0; i < 64; i++)
		{
			uint8_t bits_from_dev[2];
#ifdef FREERTOS_CONFIG_H
			taskENTER_CRITICAL();
#endif
#ifndef FREERTOS_CONFIG_H
			__disable_irq();
#endif
			Receive_Data (1, false, bits_from_dev);
			Send_Data (1, false, p_Packet_1);
#ifndef FREERTOS_CONFIG_H
			__enable_irq();
#endif
#ifdef FREERTOS_CONFIG_H
			taskEXIT_CRITICAL();
#endif
			Wait_Finish_Send();
			Wait_Finish_Receive();
#ifdef FREERTOS_CONFIG_H
			taskENTER_CRITICAL();
#endif
#ifndef FREERTOS_CONFIG_H
			__disable_irq();
#endif
			Receive_Data (1, false, bits_from_dev+1);
			Send_Data (1, false, p_Packet_1);
#ifndef FREERTOS_CONFIG_H
			__enable_irq();
#endif
#ifdef FREERTOS_CONFIG_H
			taskEXIT_CRITICAL();
#endif
			Wait_Finish_Send();
			Wait_Finish_Receive();
			uint8_t test_bit = ((bits_from_dev[0] > 0xFC)? 1 : 0) | ((bits_from_dev[1] > 0xFE)? 2 : 0);
			uint8_t packet;
			if (test_bit == 3)
			{
				return false;
			}
			else if (!test_bit)
				{
					if ((flag_of_rom & (1 << i))) packet = 1;
					else
					{
						packet = 0;
						last_disp = i;
					}
				}
			else packet = test_bit & 0x01;


			// Подсчёт CRC8
			crc_value = CRC8_One_Wire(packet, crc_value);

			temp_rom |= static_cast<uint64_t>(packet) << i;
			taskENTER_CRITICAL();
			Send_Data(1, false, (packet)? p_Packet_1 : p_Packet_0);
			taskEXIT_CRITICAL();

			Wait_Finish_Send();
		}
		if (!crc_value) return temp_rom;
		else return false;
	}

	// Функция посылки 64 битного ROM кода устройства 1-WIRE
	void Send_ROM_Code ()
	{

	}


public:

	ONE_WIRE ()
	{
		Init_Pin_for_USART <pin> ();
	}
	ONE_WIRE (bool old_init)
	{
	}

protected:

	const uint8_t* const p_Packet_Init { &Packet_Init };
	const uint8_t* const p_Packet_0 { &Packet_0 };
	const uint8_t* const p_Packet_1 { &Packet_1 };

	// Функция подсчёт контрольной суммы CRC8
	inline uint8_t CRC8_One_Wire (uint8_t a_bit, uint8_t a_crc_data)
	{
		constexpr uint8_t poly = 0x8C;
		//register uint8_t shift_bit = a_crc_data & 0x01;

		//a_crc_data >>= 1;
		if ((a_crc_data & 1u) ^ a_bit)
			{
			return (a_crc_data >> 1) ^ poly;
			}
		else return (a_crc_data >> 1);
	}

	// Функция посылки команды устройствам 1-WIRE
	constexpr void Send_Command (uint8_t a_com)
	{

		uint8_t command { a_com };
		uint8_t p_com[8];
		for (uint8_t i = 0; i < 8 ; i++)
		{
			if (command & (1 << i))
			{
				p_com[i] = Packet_1;
			}
			else
			{
				p_com[i] = Packet_0;
			}
		}
		//while (!(USART1->SR & USART_SR_TC))
	//	{

	//	};
		Send_Data(8, true, p_com);
		Wait_Finish_Send();
	}

	// Функция определения ROM, при подключении всего 1 устройства 1-WIRE
	bool Detect_One_Device (uint64_t* p_device_rom)
	{
		uint8_t last_disp;
		uint64_t flag_of_rom { 0 };

		if (!Reset_Presence()) return false;
		Send_Command(Read_Rom);
		*p_device_rom = Receive_ROM_Code(flag_of_rom, last_disp);
		if (*p_device_rom == 0) return false;
		else return true;
	}

	// Функция выбора устройства для дальнейшей работы с ним
	bool Select_Device (const uint64_t* a_device_rom)
	{
		if (!Reset_Presence()) return false;
		Send_Command(Match_Rom);
		for (uint64_t i = 1; i != 0; i <<= 1)
		{
			if (*a_device_rom & i) Send_Data(1, false, p_Packet_1);
			else Send_Data(1, false, p_Packet_0);
			Wait_Finish_Send();
		}
		return true;
	}

	// Функция для команды всем устройствам на шине 1-WIRE
	bool Select_All_Device ()
	{
		if (!Reset_Presence()) return false;
		Send_Command(Skip_Rom);
		return true;
	}

	// Функция определения ROM всех устройств на шине 1-WIRE
	bool Detect_Devices (uint64_t* p_device_rom, uint8_t a_type_device, uint8_t a_number_device)
	{
		uint8_t last_disp { 0 };
		uint64_t flag_of_rom { 0 };
		do
		{
			a_number_device--;
			if (!Reset_Presence()) return false;
			Send_Command(Search_Rom);


			uint64_t tmp = Receive_ROM_Code(flag_of_rom, last_disp);
			if (static_cast <uint8_t>(tmp) == a_type_device)
			{
				*(p_device_rom + a_number_device) = tmp;
			}
			flag_of_rom = (flag_of_rom | (1 << last_disp)) & (static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF) >> static_cast<uint64_t>(63 - last_disp));
		} while (a_number_device);
		return true;
	}

	// Функция ожидания завершения выполнения устройством посланной команды
	void Wait_End_Of_Work()
	{
		uint8_t testing;
		do
		{
#ifdef FREERTOS_CONFIG_H
			taskENTER_CRITICAL();
#endif
#ifndef FREERTOS_CONFIG_H
			__disable_irq();
#endif
			Receive_Data(1, false, &testing);
			Send_Data(1, false, p_Packet_1);
#ifndef FREERTOS_CONFIG_H
			__enable_irq();
#endif
#ifdef FREERTOS_CONFIG_H
			taskEXIT_CRITICAL();
#endif
			Wait_Finish_Send();
			Wait_Finish_Receive();
		} while(testing < 0xFE);
	}

	//
	uint8_t Read_Bit ()
	{
		uint8_t t_data;

#ifdef FREERTOS_CONFIG_H
		taskENTER_CRITICAL();
#endif
#ifndef FREERTOS_CONFIG_H
		__disable_irq();
#endif
		Receive_Data(1, false, &t_data);
		Send_Data(1, false, p_Packet_1);
#ifndef FREERTOS_CONFIG_H
		__enable_irq();
#endif
#ifdef FREERTOS_CONFIG_H
		taskEXIT_CRITICAL();
#endif
		Wait_Finish_Send();
		Wait_Finish_Receive();
		return (t_data > 0xFC)? 1 : 0;
	}
};

template <uint32_t clk_apb, enum_USART_PIN pin, uint8_t number_device>
class DS18S20 final : public ONE_WIRE <clk_apb, pin>
{

private:

	constexpr static uint8_t Type_Sensor = 0x10;
	// команды для работы с датчиком температоры
	constexpr static uint8_t Convert_Temp = 0x44;
	constexpr static uint8_t Write_Scratchpad = 0x4E;
	constexpr static uint8_t Read_Scratchpad = 0xBE;
	constexpr static uint8_t Copy_Scratchpad = 0x48;
	constexpr static uint8_t Recall = 0xB8;
	constexpr static uint8_t Read_Power = 0xB4;
	// массив ROM всех датчиков температуры
	uint64_t ROM_ds18s20[number_device];


	// Функция считывания памяти устройства
	uint64_t Read_Memory (const uint8_t num_device)//, const uint64_t* a_mem)
	{
		if (!DS18S20::Select_Device(ROM_ds18s20 + num_device)) return false;
		DS18S20::Send_Command(Read_Scratchpad);

		uint64_t data_from_mem { 0 };
		uint8_t crc_value { 0 };

		for (uint8_t i = 0; i < 72; i++)
		{
			uint8_t bit_from_dev = DS18S20::Read_Bit();
			// Подсчёт CRC8
			crc_value = DS18S20::CRC8_One_Wire(bit_from_dev, crc_value);
			// Компоновка считываемых данных
			if (i < 64) data_from_mem |= static_cast<uint64_t>(bit_from_dev) << i;
		}
		if (!crc_value) return data_from_mem;
		else return false;
	}

public:

	DS18S20 ()
	{
	}

	// Конструктор ищет устройства DS18B20 при создании объекта
	DS18S20 (bool)
	{
		DS18S20::Detect_Devices (ROM_ds18s20, Type_Sensor, number_device);
	}

	// Функция команды устройству - преобразовать температуру
	bool Prepare_Temperature_For_One (const uint8_t num_device)
	{
		if (!DS18S20::Select_Device(ROM_ds18s20 + num_device)) return false;
		DS18S20::Send_Command(Convert_Temp);
		DS18S20::Wait_End_Of_Work();
		return true;
	}

	// Функция команды всем устройствам - преобразовать температуру
	bool Prepare_Temperature_For_All ()
	{
		if (!DS18S20::Select_All_Device()) return false;
		DS18S20::Send_Command(Convert_Temp);
		return true;
	}

	// Функция считывания температуры из датчика
	bool Read_Temperature (const uint8_t num_device, int16_t out_temp[2])
	{
		if (!Prepare_Temperature_For_One(num_device)) return false;

		uint64_t temp_data = Read_Memory(num_device);
		if (temp_data)
		{
			out_temp[0] = (static_cast<int16_t>(temp_data) & 0xFF00) | (static_cast<uint8_t>(temp_data) >> 1);
			out_temp[1] = ((temp_data) & 0x0001)? 5u : 0u;
			return true;
		}
		else return false;

	}
	bool Read_Temperature (const uint8_t num_device, uint16_t* out_temp)
	{
		if (!Prepare_Temperature_For_One(num_device)) return false;
		if (DS18S20::Read_Temperature(num_device, out_temp)) {
			return true;
		}
		else {
			return false;
		}
	}

	// Функция доступа к переменной ROM_ds18s20
	uint64_t Get_ROM (uint8_t num_device) {
		return ROM_ds18s20[num_device];
	}






	//
};






































#endif /* ONEWIRE_H_ */
