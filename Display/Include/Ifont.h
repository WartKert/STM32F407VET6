/*
 * Ifont.h
 *
 *  Created on: 4 июн. 2020 г.
 *      Author: SerV
 */

#ifndef IFONT_H_
#define IFONT_H_

#include <stdint.h>

class Ifont {
public:
		virtual ~Ifont() {};
		virtual uint16_t* GetFont(const char16_t &sym) = 0;
		virtual uint16_t GetSize() = 0;
		virtual uint16_t GetLength() = 0;

};

#endif /* IFONT_H_ */
