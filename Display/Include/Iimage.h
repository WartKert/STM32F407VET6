/*
 * Iimage.h
 *
 *  Created on: 7 июн. 2020 г.
 *      Author: SerV
 */

#ifndef IIMAGE_H_
#define IIMAGE_H_

#include <stdint.h>

class Iimage {
public:
	virtual ~Iimage() {};
	virtual uint32_t GetLength() = 0;
};

#endif /* IIMAGE_H_ */
