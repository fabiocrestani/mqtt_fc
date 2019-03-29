/*
 * utils.h
 * Author: Fabio Crestani
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#define GET_MSB(x) ((x >> 8) & 0xFF)
#define GET_LSB(x) (x & 0xFF)
#define PACK_MSB_LSB(x, y) (((x & 0xFF) << 8) | (y & 0xFF))


#endif // __UTILS_H__
