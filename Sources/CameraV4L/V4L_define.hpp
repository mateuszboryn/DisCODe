#ifndef V4LDEF_HPP_
#define V4LDEF_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/types.h>

#include <linux/videodev.h>
/*

 */

// for video_capability v4L
#define	CAPTURE		VID_TYPE_CAPTURE
#define	TUNER		VID_TYPE_TUNER
#define	TELETEXT	VID_TYPE_TELETEXT
#define	OVERLAY		VID_TYPE_OVERLAY
#define	CHROMAKEY	VID_TYPE_CHROMAKEY
#define	CLIPPING	VID_TYPE_CLIPPING
#define	FRAMERAM	VID_TYPE_FRAMERAM
#define	SCALES		VID_TYPE_SCALES
#define	MONOCHROME	VID_TYPE_MONOCHROME
#define	SUBCAPTURE	VID_TYPE_SUBCAPTURE

// for video_channel
#define VC_TUNER  	VIDEO_VC_TUNER
#define VC_AUDIO	VIDEO_VC_AUDIO
#define VC_NORM		VIDEO_VC_NORM
#define TV			VIDEO_TYPE_TV
#define CAMERA		VIDEO_TYPE_CAMERA

#define GREY		VIDEO_PALETTE_GREY
#define HI240		VIDEO_PALETTE_HI240
#define RGB565		VIDEO_PALETTE_RGB565
#define RGB555		VIDEO_PALETTE_RGB555
#define RGB24		VIDEO_PALETTE_RGB24
#define RGB32		VIDEO_PALETTE_RGB32
#define YUV422		VIDEO_PALETTE_YUV422
#define YUYV		VIDEO_PALETTE_YUYV
#define UYVY		VIDEO_PALETTE_UYVY
#define YUV420		VIDEO_PALETTE_YUV420
#define YUV411		VIDEO_PALETTE_YUV411
#define RAW			VIDEO_PALETTE_RAW
#define YUV422P		VIDEO_PALETTE_YUV422P
#define YUV411P		VIDEO_PALETTE_YUV411P

#define NTSC		VIDEO_MODE_NTSC
#define PAL			VIDEO_MODE_PAL
#define SECAM		VIDEO_MODE_SECAM
#define AUTO		VIDEO_MODE_AUTO

#define COLOUR		24
#define B_W			8

#define GREY_2 		V4L2_PIX_FMT_GREY
#define YUYV_2 		V4L2_PIX_FMT_YUYV
#define UYVY_2		V4L2_PIX_FMT_UYVY
#define Y41P_2 		V4L2_PIX_FMT_Y41P
#define YVU420_2	V4L2_PIX_FMT_YVU420
#define YUV420_2 	V4L2_PIX_FMT_YUV420
#define YUV422P_2 	V4L2_PIX_FMT_YUV422P
#define YUV411P_2 	V4L2_PIX_FMT_YUV411P
#define NV12_2 		V4L2_PIX_FMT_NV12
#define NV21_2 		V4L2_PIX_FMT_NV21
#define YYUV_2		V4L2_PIX_FMT_YYUV
#define HI240_2 	V4L2_PIX_FMT_HI240
#define RGB332_2 	V4L2_PIX_FMT_RGB332
#define RGB555_2	V4L2_PIX_FMT_RGB555
#define RGB565_2 	V4L2_PIX_FMT_RGB565
#define RGB555X_2	V4L2_PIX_FMT_RGB555X
#define RGB565X_2 	V4L2_PIX_FMT_RGB565X
#define BGR24_2 	V4L2_PIX_FMT_BGR24
#define RGB24_2 	V4L2_PIX_FMT_RGB24
#define BGR32_2 	V4L2_PIX_FMT_BGR32
#define RGB32_2 	V4L2_PIX_FMT_RGB32

#endif /* V4LDEF_HPP_ */
