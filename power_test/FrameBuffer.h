/*
 * FrameBuffer.h
 *
 *  Created on: 26 ���. 2017 �.
 *      Author: Bekir
 */

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

unsigned long pixel_color(unsigned char r, unsigned char g, unsigned char b, struct fb_var_screeninfo *vinfo);

void Fill_Buffer(unsigned char r, unsigned char g, unsigned char b);

#endif /* FRAMEBUFFER_H_ */
