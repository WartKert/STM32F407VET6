/*
 * ILI9341.h
 *
 *  Created on: 19 апр. 2020 г.
 *      Author: SerV
 *
 *
 *      Файл описания работы с TFT дисплеем на контроллере ILI9341
 *      Работа осуществляется с помощью модуля FSMC (режим NOR, 16 бит)
 */



#ifndef INC_ILI9341_H_
#define INC_ILI9341_H_

#include "stm32f4xx.h"
#include "cmsis_gcc.h"
#include "FSMC.h"
#include <cassert>
#include "DMA.h"
#include <algorithm>
#include <iterator>
#include "Ifont.h"
#include <stdint.h>
#include  <math.h>

//#include "arm_math.h"
/*	Флаги установки характера передаваемой информации	*/
enum class CR : uint32_t {
	Tr_8_Inc = 0x10291,			/*	Передача массива данных шириной 8 бит с инкрементом адреса		*/
	Tr_16_Inc = 0x12A91,		/*	Передача массива данных шириной 16 бит с инкрементом адреса	*/
	Tr_16_noInc = 0x12891,		/*	Передача массива данных шириной 16 бит без инкремента адреса)	*/
	Tr_8_noInc = 0x10091,		/*	Передача массива данных шириной 8 бит без инкремента адреса		*/
};

enum class WD : uint32_t {
	width16bit = 1,
	width18bit = 2
};

/* Шаблонный класс инициализации ILI9341
 * Работает только с портами "D" и "E"
 */

//template<SubBank_Bank1 SubBank, bool bit16, uint32_t pin_A, GPIO_TypeDef* Port_RST = static_cast<GPIO_TypeDef*>(0), uint32_t pin_RST = 0>
template<WD widthdata>
class ILI9341 final : public FSMC
{
private:

	constexpr static uint32_t D0 = 14;
	constexpr static uint32_t D1 = 15;
	constexpr static uint32_t D2 = 0;
	constexpr static uint32_t D3 = 1;
	constexpr static uint32_t D4 = 7;
	constexpr static uint32_t D5 = 8;
	constexpr static uint32_t D6 = 9;
	constexpr static uint32_t D7 = 10;
	constexpr static uint32_t D8 = 11;
	constexpr static uint32_t D9 = 12;
	constexpr static uint32_t D10 = 13;
	constexpr static uint32_t D11 = 14;
	constexpr static uint32_t D12 = 15;
	constexpr static uint32_t D13 = 8;
	constexpr static uint32_t D14 = 9;
	constexpr static uint32_t D15 = 10;
	constexpr static uint32_t CS = 7;
	constexpr static uint32_t RD = 4;
	constexpr static uint32_t WR = 5;
	constexpr static uint32_t A = 18;//pin_A;
//	constexpr static uint32_t RST = pin_RST;
	constexpr static uint32_t A0 = 0;
	constexpr static uint32_t A1 = 1;
	constexpr static uint32_t A2 = 2;
	constexpr static uint32_t A3 = 3;
	constexpr static uint32_t A4 = 4;
	constexpr static uint32_t A5 = 5;
	constexpr static uint32_t A6 = 12;
	constexpr static uint32_t A7 = 13;
	constexpr static uint32_t A8 = 14;
	constexpr static uint32_t A9 = 15;
	constexpr static uint32_t A10 = 0;
	constexpr static uint32_t A11 = 1;
	constexpr static uint32_t A12 = 2;
	constexpr static uint32_t A13 = 3;
	constexpr static uint32_t A14 = 4;
	constexpr static uint32_t A15 = 5;
	constexpr static uint32_t A16 = 11;
	constexpr static uint32_t A17 = 12;
	constexpr static uint32_t A18 = 13;
	constexpr static uint32_t A19 = 3;
	constexpr static uint32_t A20 = 4;
	constexpr static uint32_t A21 = 5;
	constexpr static uint32_t A22 = 6;
	constexpr static uint32_t A23 = 2;
	constexpr static uint32_t A24 = 13;
	constexpr static uint32_t A25 = 14;
	/**********************************************/
	constexpr static uint32_t PD0 = D2;
	constexpr static uint32_t PD1 = D3;
	constexpr static uint32_t PD4 = RD;
	constexpr static uint32_t PD5 = WR;
	constexpr static uint32_t PD7 = CS;
	constexpr static uint32_t PD8 = D13;
	constexpr static uint32_t PD9 = D14;
	constexpr static uint32_t PD10 = D15;
	constexpr static uint32_t PD11 = A16;
	constexpr static uint32_t PD12 = A17;
	constexpr static uint32_t PD13 = A18;
	constexpr static uint32_t PD14 = D0;
	constexpr static uint32_t PD15 = D1;
	constexpr static uint32_t PE2 = A23;
	constexpr static uint32_t PE3 = A19;
	constexpr static uint32_t PE4 = A20;
	constexpr static uint32_t PE5 = A21;
	constexpr static uint32_t PE6 = A22;
	constexpr static uint32_t PE7 = D4;
	constexpr static uint32_t PE8 = D5;
	constexpr static uint32_t PE9 = D6;
	constexpr static uint32_t PE10 = D7;
	constexpr static uint32_t PE11 = D8;
	constexpr static uint32_t PE12 = D9;
	constexpr static uint32_t PE13 = D10;
	constexpr static uint32_t PE14 = D11;
	constexpr static uint32_t PE15 = D12;
	constexpr static uint32_t PG0 = A10;
	constexpr static uint32_t PG1 = A11;
	constexpr static uint32_t PG2 = A12;
	constexpr static uint32_t PG3 = A13;
	constexpr static uint32_t PG4 = A14;
	constexpr static uint32_t PG5 = A15;
	constexpr static uint32_t PG13 = A24;
	constexpr static uint32_t PG14 = A25;
	constexpr static uint32_t PF0 = A0;
	constexpr static uint32_t PF1 = A1;
	constexpr static uint32_t PF2 = A2;
	constexpr static uint32_t PF3 = A3;
	constexpr static uint32_t PF4 = A4;
	constexpr static uint32_t PF5 = A5;
	constexpr static uint32_t PF12 = A6;
	constexpr static uint32_t PF13 = A7;
	constexpr static uint32_t PF14 = A8;
	constexpr static uint32_t PF15 = A9;
	/**********************************************/
	constexpr static uint32_t AF12 = 0xC;
	constexpr static uint32_t MODERAF = 0x2;
	constexpr static uint32_t ZERO = 0;
	/**********************************************/


	/***********	Адреса доступа к FSMC	*******/
//	static uint32_t Num_Device;
	const uint16_t* REG;
	const uint16_t* RAM;
	volatile DMA_Stream_TypeDef* pDMA_Stream;
	volatile DMA_TypeDef* pDMA;


/**************	Функция установки адреса строки или страницы	***************************/
	void Set_Adr (const void* pcom , const uint16_t& sc_bits, const uint16_t& ec_bits) {
		WR_Transfer(static_cast<const uint32_t*>(pcom), 1, false, CR::Tr_8_noInc);
		volatile uint32_t pack;
		asm volatile (
				"ORR %0, %1, %2, LSL#16 \n"
				"REV16 %0, %0"
				: "=r" (pack)
				: "r" (sc_bits), "r" (ec_bits)
		);

		//uint32_t* ppack = &pack;
		WR_Transfer(const_cast<const uint32_t*>(&(pack)), 4, true, CR::Tr_8_Inc);
	}

	void R_Transfer (const uint8_t* data, uint32_t&& num_data){
		/* Конфигурация DMA */
		while(ILI9341::ReadyDMA);
		pDMA_Stream -> PAR = reinterpret_cast<uint32_t>(RAM);
		pDMA_Stream -> M0AR = reinterpret_cast<uint32_t>(data);
		pDMA_Stream -> NDTR = num_data;
		ILI9341::ReadyDMA = true;
		pDMA_Stream -> CR = 0x491;//0x81;//0x481;//0x2881;
	}

/**************	Запись данных	************************************************************/
	void WR_Transfer (const void* data, const uint32_t& num_data, bool is_data, CR type_tr){
		/* Конфигурация DMA */
		while(ILI9341::ReadyDMA){};
		pDMA_Stream -> PAR = reinterpret_cast<uint32_t>(data);
		pDMA_Stream -> M0AR = reinterpret_cast<uint32_t>((is_data)? RAM : REG);
		pDMA_Stream -> NDTR = num_data;
		ILI9341::ReadyDMA = true;
		pDMA_Stream -> CR = static_cast<uint32_t>(type_tr);
	}

	/*	Подготовка цвета пикселя к передачи	*/
	uint16_t* Prepare_Pixel(uint16_t arr[], uint16_t num, const uint32_t &color, const uint16_t &length) {
		if (widthdata == WD::width18bit) {
			uint16_t rcol = static_cast<uint16_t>((color & 0x3F000) >> 10);
			uint16_t gcol = static_cast<uint16_t>((color & 0xFC0) >> 4);
			uint16_t bcol = static_cast<uint16_t>((color & 0x3F) << 2);
			//num = ((length >> 1) + length);// * width;
			//uint16_t arr;
			for(int16_t cnt = num - 1; cnt >= 0; cnt -= 3) {
				arr[cnt] = (gcol << 8) | bcol;
				arr[cnt-1] = (bcol << 8) | rcol;
				arr[cnt-2] = (rcol << 8) | gcol;
			}
			//Data_to_Mem(arr, num, CR::Tr_16_Inc);
		}
		return arr;


	}
public:

/**************	Чтение данных	*************************************************************/
	/***********	Флаг разрешения DMA	***********/
	volatile static bool ReadyDMA;
	/***********	Список команд	***************/
	constexpr static uint32_t cNOP = 0x0;
	constexpr static uint32_t cRST = 0x1;
	constexpr static uint32_t cDispOff = 0x28;
	constexpr static uint32_t cDispOn = 0x29;
	constexpr static uint32_t cDispInvOff = 0x20;
	constexpr static uint32_t cDispInvOn = 0x21;
	constexpr static uint32_t cDispId = 0x04;
	constexpr static uint32_t cDispStatus = 0x09;
	constexpr static uint32_t cColorSet = 0x2D;
	constexpr static uint32_t cColumnAdrSet = 0x2A;
	constexpr static uint32_t cPageAdrSet = 0x2B;
	constexpr static uint32_t cMemWrite = 0x2C;
	constexpr static uint32_t cMemWriteCont = 0x3C;
	constexpr static uint32_t cColmod = 0x3A;
	constexpr static uint32_t cSleepOut = 0x11;
	constexpr static uint32_t cMemAccCtr = 0x36;



	template<SubBank_Bank1 SubBank, bool bit16, uint32_t pin_A, uint32_t rclkBus, uint32_t rclkData, uint32_t rclkAdr, \
			DMA_Num num_DMA , DMA_Stream ch_DMA>
	void Config (){
		/*	Назначение используемого канала DMA	*/
		pDMA_Stream = DMA_Conv(ch_DMA);
		pDMA = DMA_Conv(num_DMA);

		/*	Конфигурация FSMC (mode 2)	*/
		RAM = (reinterpret_cast<uint16_t*>((2 << pin_A) | FSMC::ConfBank1<SubBank, Bank1_Mode::Mode12B, rclkBus, rclkData, rclkAdr>()));
		REG = RAM - static_cast<uint16_t>(1);

		/*	Конфигурация GPIO	*/
		/*	PORTD	*/
		GPIOD -> MODER |= (MODERAF << (PD0 << 1)) | (MODERAF << (PD1 << 1)) | (MODERAF << (PD4 << 1)) | (MODERAF << (PD5 << 1)) | \
				(MODERAF << (PD7 << 1)) | (MODERAF << (PD8 << 1)) | (MODERAF << (PD9 << 1)) | (MODERAF << (PD10 << 1)) | \
				(MODERAF << (PD14 << 1)) | (MODERAF << (PD15 << 1)) | ((A == 16)? (MODERAF << (PD11 << 1)) : (A == 17)? (MODERAF << (PD12 << 1)) : \
				(A == 18)? (MODERAF << (PD13 << 1)) : 0);
		GPIOD -> OSPEEDR |= (3 << (PD0 << 1)) | (3 << (PD1 << 1)) | (3 << (PD4 << 1)) | (3 << (PD5 << 1)) | \
				(3 << (PD7 << 1)) | (3 << (PD8 << 1)) | (3 << (PD9 << 1)) | (3 << (PD10 << 1)) | \
				(3 << (PD14 << 1)) | (3 << (PD15 << 1)) | ((A == 16)? (3 << (PD11 << 1)) : (A == 17)? (3 << (PD12 << 1)) : \
				(A == 18)? (3 << (PD13 << 1)) : 0);
		GPIOD -> AFR[0] |= (AF12 << (PD0 << 2)) | (AF12 << (PD1 << 2)) | (AF12 << (PD4 << 2)) | (AF12 << (PD5 << 2)) | \
				(AF12 << (PD7 << 2));
		GPIOD -> AFR[1] |= AF12 | (AF12 << 4) | (AF12 << 8) | (AF12 << 24) | (AF12 << 28) | \
				((A == 16)? (AF12 << 12) : (A == 17)? (AF12 << 16) : (A == 18)? (AF12 << 20) : 0);
		/*	PORTE	*/
		GPIOE -> MODER |= (MODERAF << (PE7 << 1)) | (MODERAF << (PE8 << 1)) | (MODERAF << (PE9 << 1)) | \
				(MODERAF << (PE10 << 1)) | (MODERAF << (PE11 << 1)) | (MODERAF << (PE12 << 1)) | (MODERAF << (PE13 << 1)) | \
				(MODERAF << (PE14 << 1)) | (MODERAF << (PE15 << 1)) | ((A == 19)? (MODERAF << (PE3 << 1)) : (A == 20)? (MODERAF << (PE4 << 1)) : \
				(A == 21)? (MODERAF << (PE5 << 1)) : (A == 22)? (MODERAF << (PE6 << 1)) : (A == 23)? (MODERAF << (PE2 << 1)) : 0);
		GPIOE -> OSPEEDR |= (3 << (PE7 << 1)) | (3 << (PE8 << 1)) | (3 << (PE9 << 1)) | \
				(3 << (PE10 << 1)) | (3 << (PE11 << 1)) | (3 << (PE12 << 1)) | (3 << (PE13 << 1)) | \
				(3 << (PE14 << 1)) | (3 << (PE15 << 1)) | ((A == 19)? (3 << (PE3 << 1)) : (A == 20)? (3 << (PE4 << 1)) : \
				(A == 21)? (3 << (PE5 << 1)) : (A == 22)? (3 << (PE6 << 1)) : (A == 23)? (3 << (PE2 << 1)) : 0);
		GPIOE -> AFR[0] |= (AF12 << (PE7 << 2)) | ((A == 19)? (AF12 << (PE3 << 2)) : (A == 20)? (AF12 << (PE4 << 2)) : \
				(A == 21)? (AF12 << (PE5 << 2)) : (A == 22)? (AF12 << (PE6 << 2)) : (A == 23)? (AF12 << (PE2 << 2)) : 0);
		GPIOE -> AFR[1] |= AF12 | (AF12 << 4) | (AF12 << 8) | (AF12 << 12) | (AF12 << 16) | (AF12 << 20) | (AF12 << 24) | (AF12 << 28);
	}


	/*	Проверка освобождения канала DMA	*/
	void is_Free_DMA (){
		while (!(DMA2 -> LISR & DMA_LISR_TCIF0)){
		}
		pDMA -> LIFCR |= DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0;
	}




	void ON (){
		volatile uint8_t t_com;
		WR_Transfer(const_cast<uint8_t*>(&(t_com = cRST)), 1, false, CR::Tr_16_noInc);
		//for (register volatile uint16_t c = 0; c < 1000; ++c);
		WR_Transfer(const_cast<uint8_t*>(&(t_com = cColmod)), 1, false, CR::Tr_8_noInc);
		//for (register volatile uint16_t c = 0; c < 1000; ++c);
		WR_Transfer(const_cast<uint8_t*>(&(t_com = 0x66)), 1, true, CR::Tr_8_noInc);
		//for (register volatile uint16_t c = 0; c < 1000; ++c);
		WR_Transfer(const_cast<uint8_t*>(&(t_com = cMemAccCtr)), 1, false, CR::Tr_8_noInc);
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		//for (register volatile uint16_t c = 0; c < 10; ++c);
		WR_Transfer(const_cast<uint8_t*>(&(t_com = 0x08)), 1, true, CR::Tr_8_noInc);
		//for (register volatile uint16_t c = 0; c < 10; ++c);
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		WR_Transfer(const_cast<uint8_t*>(&(t_com = cSleepOut)), 1, false, CR::Tr_8_noInc);
		for (volatile uint16_t c = 0; c < 17500; ++c);
		WR_Transfer(const_cast<uint8_t*>(&(t_com = cDispOn)), 1, false, CR::Tr_8_noInc);
		for (volatile uint16_t c = 0; c < 17500; ++c);
	}

	void Status (uint8_t* adr){
		//WR_Transfer(cDispStatus, 1, false);
		//R_Transfer(adr, 5);
	}



	/*	Функция установки адреса столбца	*/
	void Set_Column (const uint16_t& strt_bits, const uint16_t& end_bits) {
		volatile uint16_t t_com;

		Set_Adr(const_cast<uint16_t*>(&(t_com = cColumnAdrSet)), strt_bits, end_bits);
	}

	/*	Функция установки адреса строки	*/
	void Set_Page (const uint16_t& strt_bits, const uint16_t& end_bits) {
		volatile uint16_t t_com;

		Set_Adr(const_cast<uint16_t*>(&(t_com = cPageAdrSet)), strt_bits, end_bits);
	}

	/*	Функция записи данных в память дисплея с начального адреса или с последнего	*/
	void Data_to_Mem (const uint16_t* p_data, uint32_t &&num_data, CR opDMA, const bool &cont = false) {
		volatile uint8_t t_com;
		if(cont) t_com = cMemWriteCont;
		else t_com = cMemWrite;
		WR_Transfer(const_cast<uint8_t*>(&t_com), 1, false, CR::Tr_8_noInc);
		if (num_data > 65535) {

			WR_Transfer(p_data, 65535, true, opDMA);
			num_data -= 65535;
			if (opDMA == CR::Tr_16_Inc)	p_data += 65535;
		}
		WR_Transfer(p_data, num_data, true, opDMA);
	}

	/*	Вывод заданного текста на экран с заданным положением начала строки	*/
	void Set_Text (const char16_t text[], Ifont &fonttype, const uint16_t &col, const uint16_t &strtrow, \
			const uint16_t &endrow, const bool &linebreak) {
		// Установка расположение строки для текста
		uint16_t bottomcol = col + fonttype.GetSize() - 1;
		Set_Column(col, bottomcol);
		Set_Page(strtrow, strtrow + endrow);// (lengthrow * fonttype.GetSize()));
		// Вывод текста
		// Определение размера требуемого массива для вывода строки
		// Вместимость символов в строку
		uint16_t cpstrow {static_cast<uint16_t>((endrow - strtrow) / fonttype.GetSize())};
		// Массив символов для вывода строки
		uint16_t temparray[cpstrow * fonttype.GetLength()], *ptemparray {temparray};
		//
		//uint16_t cntsym {0};
		// Счётчик символов для передачи в строку
		uint16_t cntsym {0};

		while(1) {
			uint16_t *pfont = fonttype.GetFont(*text);
 			++text;
			++cntsym;
			uint16_t i {fonttype.GetLength()};
			do {
				*ptemparray = *pfont++;
				++ptemparray;
				--i;
			} while(i);
			if (*text == u'\0') {
				Data_to_Mem(temparray, cntsym * fonttype.GetLength(), CR::Tr_16_Inc);
				break;
			} else if (cpstrow == cntsym) {
				Data_to_Mem(temparray, cntsym * fonttype.GetLength(), CR::Tr_16_Inc);
				ptemparray = temparray;
				cntsym = 0;
				// При разрешении, то указываем новую строку
				if (linebreak) {
					Set_Column(bottomcol, bottomcol + fonttype.GetSize() - 1);
					bottomcol += fonttype.GetSize() - 1;
					Set_Page(strtrow, strtrow + endrow);
				}
			}
		}
	}

	/*	Вывод прямой линии на экран	через две заданные точки находящихся по углом != 90 гр. к оси	*/
	void Set_Line(const uint16_t &x1, const uint16_t &y1, const uint16_t &x2, const uint16_t &y2, const uint32_t &grb18bit) {
		// расчёт уравнения прямой по двум точкам
		int16_t y;

		int16_t arg1 = (y2 * x1) + (y1 * x2);
		int16_t arg2 = (x2 + x1);
		int16_t arg3 = (y2 - y1);
		volatile uint16_t data = (grb18bit & 0x3F) << 2 | (grb18bit & 0xFC0) << 4 | (grb18bit & 0x3F000) << 14;
		for(int16_t c = x1; c < x2; ++c) {
			y = static_cast<int16_t>((c * arg3 - arg1) / arg2);
			Set_Column(y, y);
			Set_Page(c, c);
			Data_to_Mem(const_cast<uint16_t*>(&(data)), 2, CR::Tr_16_Inc);
		}
	}

	/*	Вывод горизонтальной линии на экран с заданным цветом	*/
	void Set_HLine(const uint16_t &x, const uint16_t &y, const uint16_t &length, uint16_t width = 1, const uint32_t &rgb18bit = 0) {
		/*volatile uint16_t data = (grb18bit & 0x3F) << 2 | (grb18bit & 0xFC0) << 4 | (grb18bit & 0x3F000) << 14;
		Set_Column(y, y + width - 1);
		Set_Page(x, x + length);
		Data_to_Mem(const_cast<uint16_t*>(&(data)), length, CR::Tr_16_noInc);
		*/
		uint16_t num = ((length >> 1) + length);// * width;
		uint16_t arr[num];
		do{
			Set_Column(y + width - 1, y + width - 1);
			Set_Page(x, x + length - 1);
			Data_to_Mem(Prepare_Pixel(arr, num, rgb18bit, length), num, CR::Tr_16_Inc);
		} while(--width);



	}

	/*	Вывод вертикальной линии на экран с заданным цветом	*/
	void Set_VLine(const uint16_t &x, const uint16_t &y, const uint16_t &length, uint16_t width = 1, const uint32_t &rgb18bit = 0) {
		uint16_t num = ((length >> 1) + length);// * width;
		uint16_t arr[num];
		do{
			Set_Column(y, y + length - 1);
			Set_Page(x + width - 1, x + width - 1);
			Data_to_Mem(Prepare_Pixel(arr, num, rgb18bit, length), num, CR::Tr_16_Inc);
		} while(--width);
	}

	/*	Вывод на дисплей окружность с заданным центром и радиусом окружности	*/
	void Set_Circ(uint16_t cx, const uint16_t &cy, const uint16_t &r, const uint32_t &grb18bit = 0) {
		volatile uint16_t data = (grb18bit & 0x3F) << 2 | (grb18bit & 0xFC0) << 4 | (grb18bit & 0x3F000) << 14;
		for (int x = r ; x >= -r; --x) {
			uint32_t y = r;
			float_t f;
			asm volatile (
					"mul			%0, %0	\n"
					"mls			%0, %2, %2, %0	\n"
					"vmov			%1, %0	\n"
					"vcvt.f32.u32	%1, %1	\n"
					"vsqrt.f32		%1, %1	\n"
					"vcvt.u32.f32	%1, %1	\n"
					"vmov			%0, %1	\n"
					: "+r" (y), "=t" (f)
					: "r" (static_cast<uint32_t>(x))
			);

			Set_Column(cy - y, cy + y);
			Set_Page(cx + x, cx + x);

			uint16_t num;
			if (widthdata == WD::width18bit) num = (y << 1) + 1 + y;
			else num = y;

			Data_to_Mem(const_cast<uint16_t*>(&(data)), num, CR::Tr_16_noInc);
		}
	}




protected:

};


template<WD widthdata>
volatile bool ILI9341<widthdata>::ReadyDMA { 0 };























#endif /* INC_ILI9341_H_ */
