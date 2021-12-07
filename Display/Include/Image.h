/********* Заголовочный файл для массива изображения в формате uint16 *********/
/********* На конце файла находится структура с указателями на массивы ********/
#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

constexpr auto sizeimageArray = 115200u;
/* Массив изображения */
extern const uint16_t imageArray [ 115200 ];

struct pointImage {
	/* Указатель на массив ' imageArray ' */
	const uint16_t *pimageArray;

	pointImage() :
		pimageArray(imageArray)
		{}
};

#endif /* IMAGE_H */
