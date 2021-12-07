/********* Заголовочный файл для массива изображения в формате uint16 *********/
/********* На конце файла находится структура с указателями на массивы ********/
#ifndef IMAGEMAIN_H
#define IMAGEMAIN_H

#include <stdint.h>
#include "Iimage.h"

namespace Imagemain {
	/* Массив изображения */
	extern const uint16_t imageArray [ 115200 ];

	/* Длина масссива */
	extern const uint32_t length;

}
class pointImagemain : public Iimage {
public:
	/* Указатель на ' imageArray ' */
	const uint16_t *pimageArray;

	/* Указатель на ' length ' */
	const uint32_t *plength;

	virtual ~pointImagemain() {};
	pointImagemain() :
		pimageArray(Imagemain::imageArray),plength(&Imagemain::length)
		{}

	// Метод возвращает длину массива для одного символа символа
	virtual uint32_t GetLength() {
		return *(this -> plength);
	}
};
#endif /* IMAGEMAIN_H */
