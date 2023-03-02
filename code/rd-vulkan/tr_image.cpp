/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2005 - 2015, ioquake3 contributors
Copyright (C) 2013 - 2015, OpenJK contributors

This file is part of the OpenJK source code.

OpenJK is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

// tr_image.c

#include "../server/exe_headers.h"

#include "tr_local.h"
#include "../rd-common/tr_common.h"
#include <png.h>
#include <map>

static byte			 s_intensitytable[256];
static unsigned char s_gammatable[256];

int		gl_filter_min = 0;
int		gl_filter_max = 1;

#define FILE_HASH_SIZE		1024	// actually, the shader code needs this (from another module, great).
//static	image_t*		hashTable[FILE_HASH_SIZE];

/*
** R_GammaCorrect
*/
void R_GammaCorrect( byte *buffer, int bufSize ) {
	int i;

	for ( i = 0; i < bufSize; i++ ) {
		buffer[i] = s_gammatable[buffer[i]];
	}
}

typedef struct {
	const char *name;
	int	minimize, maximize;
} textureMode_t;

/*textureMode_t modes[] = {
	{"GL_NEAREST", GL_NEAREST, GL_NEAREST},
	{"GL_LINEAR", GL_LINEAR, GL_LINEAR},
	{"GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST},
	{"GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR},
	{"GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST},
	{"GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR}
};*/
textureMode_t modes[] = {
	{"GL_NEAREST", VK_FILTER_NEAREST, VK_FILTER_NEAREST},
	{"GL_LINEAR", VK_FILTER_LINEAR, VK_FILTER_LINEAR},
	{"GL_NEAREST_MIPMAP_NEAREST", VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_FILTER_NEAREST},
	{"GL_LINEAR_MIPMAP_NEAREST", VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_FILTER_LINEAR},
	{"GL_NEAREST_MIPMAP_LINEAR", VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_FILTER_NEAREST},
	{"GL_LINEAR_MIPMAP_LINEAR", VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_FILTER_LINEAR}
};

static const size_t numTextureModes = ARRAY_LEN(modes);

/*
================
return a hash value for the filename
================
*/
long generateHashValue( const char *fname ) {
	int		i;
	long	hash;
	char	letter;

	hash = 0;
	i = 0;
	while (fname[i] != '\0') {
		letter = tolower(fname[i]);
		if (letter =='.') break;				// don't include extension
		if (letter =='\\') letter = '/';		// damn path names
		hash+=(long)(letter)*(i+119);
		i++;
	}
	hash &= (FILE_HASH_SIZE-1);
	return hash;
}

// makeup a nice clean, consistant name to query for and file under, for map<> usage...
//
char *GenerateImageMappingName( const char *name )
{
	static char sName[MAX_QPATH];
	int		i=0;
	char	letter;

	while (name[i] != '\0' && i<MAX_QPATH-1)
	{
		letter = tolower(name[i]);
		if (letter =='.') break;				// don't include extension
		if (letter =='\\') letter = '/';		// damn path names
		sName[i++] = letter;
	}
	sName[i]=0;

	return &sName[0];
}

/*
===============
GL_TextureMode
===============
*/
void GL_TextureMode( const char *string ) {
	size_t	i;
	image_t	*glt;

	for ( i = 0; i < numTextureModes ; i++ ) {
		if ( !Q_stricmp( modes[i].name, string ) ) {
			break;
		}
	}

	if ( i == numTextureModes ) {
		ri.Printf (PRINT_ALL, "bad filter name\n");
		for ( i = 0; i < numTextureModes ; i++ ) {
			ri.Printf( PRINT_ALL, "%s\n", modes[i].name);
		}
		return;
	}

	i = 0;

	// If the level they requested is less than possible, set the max possible...
	if ( r_ext_texture_filter_anisotropic->value > glConfig.maxTextureFilterAnisotropy )
	{
		ri.Cvar_SetValue( "r_ext_texture_filter_anisotropic", glConfig.maxTextureFilterAnisotropy );
	}

	// change all the existing mipmap texture objects
//	int iNumImages =
	   				 R_Images_StartIteration();
	while ( (glt   = R_Images_GetNextIteration()) != NULL)
	{
        VK_CHECK(vkDeviceWaitIdle(vk.device));
		
        //image_t* glt = tr.images[i];
        if (glt->mipmap) {
            Vk_Image& image = vk_world.images[glt->index];
            //vk_update_descriptor_set(image.descriptor_set, image.view, true, glt->wrapClampMode );
        }
        i++;
	}
}

static float R_BytesPerTex (int format)
{
	switch ( format ) {
	case 1:
		//"I    "
		return 1;
		break;
	case 2:
		//"IA   "
		return 2;
		break;
	case 3:
		//"RGB  "
		return glConfig.colorBits/8.0f;
		break;
	case 4:
		//"RGBA "
		return glConfig.colorBits/8.0f;
		break;

/*	case GL_RGBA4:
		//"RGBA4"
		return 2;
		break;
	case GL_RGB5:
		//"RGB5 "
		return 2;
		break;

	case GL_RGBA8:
		//"RGBA8"
		return 4;
		break;
	case GL_RGB8:
		//"RGB8"
		return 4;
		break;

	case GL_RGB4_S3TC:
		//"S3TC "
		return 0.33333f;
		break;
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		//"DXT1 "
		return 0.33333f;
		break;
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		//"DXT5 "
		return 1;
		break;*/
	default:
		//"???? "
		return 4;
	}
}

/*
===============
R_SumOfUsedImages
===============
*/
float R_SumOfUsedImages( qboolean bUseFormat )
{
	int	total = 0;
	image_t *pImage;

	   				  R_Images_StartIteration();
	while ( (pImage = R_Images_GetNextIteration()) != NULL)
	{
		if ( pImage->frameUsed == tr.frameCount- 1 ) {//it has already been advanced for the next frame, so...
			if (bUseFormat)
			{
				float  bytePerTex = R_BytesPerTex (pImage->internalFormat);
				total += bytePerTex * (pImage->width * pImage->height);
			}
			else
			{
				total += pImage->width * pImage->height;
			}
		}
	}

	return total;
}

/*
===============
R_ImageList_f
===============
*/
void R_ImageList_f( void ) {
	int		i=0;
	image_t	*image;
	int		texels = 0;
//	int		totalFileSizeK = 0;
	float	texBytes = 0.0f;
	const char *yesno[] = {"no ", "yes"};

	ri.Printf (PRINT_ALL, "\n      -w-- -h-- -fsK- -mm- -if- wrap --name-------\n");

	int iNumImages = R_Images_StartIteration();
	while ( (image = R_Images_GetNextIteration()) != NULL)
	{
		texels   += image->width*image->height;
		texBytes += image->width*image->height * R_BytesPerTex (image->internalFormat);
//		totalFileSizeK += (image->imgfileSize+1023)/1024;
		//ri.Printf (PRINT_ALL,  "%4i: %4i %4i %5i  %s ",
		//	i, image->width, image->height,(image->fileSize+1023)/1024, yesno[image->mipmap] );
		ri.Printf (PRINT_ALL,  "%4i: %4i %4i  %s ",
			i, image->width, image->height,yesno[image->mipmap] );

		switch ( image->internalFormat ) {
		case 1:
			ri.Printf( PRINT_ALL, "I    " );
			break;
		case 2:
			ri.Printf( PRINT_ALL, "IA   " );
			break;
		case 3:
			ri.Printf( PRINT_ALL, "RGB  " );
			break;
		case 4:
			ri.Printf( PRINT_ALL, "RGBA " );
			break;
		/*case GL_RGBA8:
			ri.Printf( PRINT_ALL, "RGBA8" );
			break;
		case GL_RGB8:
			ri.Printf( PRINT_ALL, "RGB8 " );
			break;
		case GL_RGB4_S3TC:
			ri.Printf( PRINT_ALL, "S3TC " );
			break;
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			ri.Printf( PRINT_ALL, "DXT1 " );
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			ri.Printf( PRINT_ALL, "DXT5 " );
			break;
		case GL_RGBA4:
			ri.Printf( PRINT_ALL, "RGBA4" );
			break;
		case GL_RGB5:
			ri.Printf( PRINT_ALL, "RGB5 " );
			break;*/
		default:
			ri.Printf( PRINT_ALL, "???? " );
		}

		/*switch ( image->wrapClampMode ) {
		case GL_REPEAT:
			ri.Printf( PRINT_ALL, "rept " );
			break;
		case GL_CLAMP:
			ri.Printf( PRINT_ALL, "clmp " );
			break;
		case GL_CLAMP_TO_EDGE:
			ri.Printf( PRINT_ALL, "clpE " );
			break;
		default:
			ri.Printf( PRINT_ALL, "%4i ", image->wrapClampMode );
			break;
		}*/

		ri.Printf( PRINT_ALL, "%s\n", image->imgName );
		i++;
	}
	ri.Printf (PRINT_ALL, " ---------\n");
	ri.Printf (PRINT_ALL, "      -w-- -h-- -mm- -if- wrap --name-------\n");
	ri.Printf (PRINT_ALL, " %i total texels (not including mipmaps)\n", texels );
//	ri.Printf (PRINT_ALL, " %iMB total filesize\n", (totalFileSizeK+1023)/1024 );
	ri.Printf (PRINT_ALL, " %.2fMB total texture mem (not including mipmaps)\n", texBytes/1048576.0f );
	ri.Printf (PRINT_ALL, " %i total images\n\n", iNumImages );
}

//=======================================================================

/*
================
ResampleTexture

Used to resample images in a more general than quartering fashion.

This will only be filtered properly if the resampled size
is greater than half the original size.

If a larger shrinking is needed, use the mipmap function 
before or after.
================
*/
static void ResampleTexture( unsigned *in, int inwidth, int inheight, unsigned *out,  
							int outwidth, int outheight ) {
	int		i, j;
	unsigned	*inrow, *inrow2;
	unsigned	frac, fracstep;
	unsigned	p1[2048], p2[2048];
	byte		*pix1, *pix2, *pix3, *pix4;

	if (outwidth>2048)
		ri.Error(ERR_DROP, "ResampleTexture: max width");
								
	fracstep = inwidth*0x10000/outwidth;

	frac = fracstep>>2;
	for ( i=0 ; i<outwidth ; i++ ) {
		p1[i] = 4*(frac>>16);
		frac += fracstep;
	}
	frac = 3*(fracstep>>2);
	for ( i=0 ; i<outwidth ; i++ ) {
		p2[i] = 4*(frac>>16);
		frac += fracstep;
	}

	for (i=0 ; i<outheight ; i++, out += outwidth) {
		inrow = in + inwidth*(int)((i+0.25)*inheight/outheight);
		inrow2 = in + inwidth*(int)((i+0.75)*inheight/outheight);
		frac = fracstep >> 1;
		for (j=0 ; j<outwidth ; j++) {
			pix1 = (byte *)inrow + p1[j];
			pix2 = (byte *)inrow + p2[j];
			pix3 = (byte *)inrow2 + p1[j];
			pix4 = (byte *)inrow2 + p2[j];
			((byte *)(out+j))[0] = (pix1[0] + pix2[0] + pix3[0] + pix4[0])>>2;
			((byte *)(out+j))[1] = (pix1[1] + pix2[1] + pix3[1] + pix4[1])>>2;
			((byte *)(out+j))[2] = (pix1[2] + pix2[2] + pix3[2] + pix4[2])>>2;
			((byte *)(out+j))[3] = (pix1[3] + pix2[3] + pix3[3] + pix4[3])>>2;
		}
	}
}


/*
================
R_LightScaleTexture

Scale up the pixel values in a texture to increase the
lighting range
================
*/
static void R_LightScaleTexture (unsigned *in, int inwidth, int inheight, qboolean only_gamma )
{
	if ( only_gamma )
	{
		if ( !glConfig.deviceSupportsGamma )
		{
			int		i, c;
			byte	*p;

			p = (byte *)in;

			c = inwidth*inheight;
			for (i=0 ; i<c ; i++, p+=4)
			{
				p[0] = s_gammatable[p[0]];
				p[1] = s_gammatable[p[1]];
				p[2] = s_gammatable[p[2]];
			}
		}
	}
	else
	{
		int		i, c;
		byte	*p;

		p = (byte *)in;

		c = inwidth*inheight;

		if ( glConfig.deviceSupportsGamma )
		{
			for (i=0 ; i<c ; i++, p+=4)
			{
				p[0] = s_intensitytable[p[0]];
				p[1] = s_intensitytable[p[1]];
				p[2] = s_intensitytable[p[2]];
			}
		}
		else
		{
			for (i=0 ; i<c ; i++, p+=4)
			{
				p[0] = s_gammatable[s_intensitytable[p[0]]];
				p[1] = s_gammatable[s_intensitytable[p[1]]];
				p[2] = s_gammatable[s_intensitytable[p[2]]];
			}
		}
	}
}


/*
================
R_MipMap2

Uses temp mem, but then copies back to input, quartering the size of the texture
Proper linear filter
================
*/
static void R_MipMap2( unsigned *in, int inWidth, int inHeight ) {
	int			i, j, k;
	byte		*outpix;
	int			inWidthMask, inHeightMask;
	int			total;
	int			outWidth, outHeight;
	unsigned	*temp;

	outWidth = inWidth >> 1;
	outHeight = inHeight >> 1;
	temp = (unsigned int *) R_Malloc( outWidth * outHeight * 4, TAG_TEMP_WORKSPACE, qfalse );

	inWidthMask = inWidth - 1;
	inHeightMask = inHeight - 1;

	for ( i = 0 ; i < outHeight ; i++ ) {
		for ( j = 0 ; j < outWidth ; j++ ) {
			outpix = (byte *) ( temp + i * outWidth + j );
			for ( k = 0 ; k < 4 ; k++ ) {
				total =
					1 * ((byte *)&in[ ((i*2-1)&inHeightMask)*inWidth + ((j*2-1)&inWidthMask) ])[k] +
					2 * ((byte *)&in[ ((i*2-1)&inHeightMask)*inWidth + ((j*2)&inWidthMask) ])[k] +
					2 * ((byte *)&in[ ((i*2-1)&inHeightMask)*inWidth + ((j*2+1)&inWidthMask) ])[k] +
					1 * ((byte *)&in[ ((i*2-1)&inHeightMask)*inWidth + ((j*2+2)&inWidthMask) ])[k] +

					2 * ((byte *)&in[ ((i*2)&inHeightMask)*inWidth + ((j*2-1)&inWidthMask) ])[k] +
					4 * ((byte *)&in[ ((i*2)&inHeightMask)*inWidth + ((j*2)&inWidthMask) ])[k] +
					4 * ((byte *)&in[ ((i*2)&inHeightMask)*inWidth + ((j*2+1)&inWidthMask) ])[k] +
					2 * ((byte *)&in[ ((i*2)&inHeightMask)*inWidth + ((j*2+2)&inWidthMask) ])[k] +

					2 * ((byte *)&in[ ((i*2+1)&inHeightMask)*inWidth + ((j*2-1)&inWidthMask) ])[k] +
					4 * ((byte *)&in[ ((i*2+1)&inHeightMask)*inWidth + ((j*2)&inWidthMask) ])[k] +
					4 * ((byte *)&in[ ((i*2+1)&inHeightMask)*inWidth + ((j*2+1)&inWidthMask) ])[k] +
					2 * ((byte *)&in[ ((i*2+1)&inHeightMask)*inWidth + ((j*2+2)&inWidthMask) ])[k] +

					1 * ((byte *)&in[ ((i*2+2)&inHeightMask)*inWidth + ((j*2-1)&inWidthMask) ])[k] +
					2 * ((byte *)&in[ ((i*2+2)&inHeightMask)*inWidth + ((j*2)&inWidthMask) ])[k] +
					2 * ((byte *)&in[ ((i*2+2)&inHeightMask)*inWidth + ((j*2+1)&inWidthMask) ])[k] +
					1 * ((byte *)&in[ ((i*2+2)&inHeightMask)*inWidth + ((j*2+2)&inWidthMask) ])[k];
				outpix[k] = total / 36;
			}
		}
	}

	memcpy( in, temp, outWidth * outHeight * 4 );
	R_Free( temp );
}

/*
================
R_MipMap

Operates in place, quartering the size of the texture
================
*/
static void R_MipMap (byte *in, int width, int height) {
	int		i, j;
	byte	*out;
	int		row;

	if ( width == 1 && height == 1 ) {
		return;
	}

	if ( !r_simpleMipMaps->integer ) {
		R_MipMap2( (unsigned *)in, width, height );
		return;
	}

	row = width * 4;
	out = in;
	width >>= 1;
	height >>= 1;

	if ( width == 0 || height == 0 ) {
		width += height;	// get largest
		for (i=0 ; i<width ; i++, out+=4, in+=8 ) {
			out[0] = ( in[0] + in[4] )>>1;
			out[1] = ( in[1] + in[5] )>>1;
			out[2] = ( in[2] + in[6] )>>1;
			out[3] = ( in[3] + in[7] )>>1;
		}
		return;
	}

	for (i=0 ; i<height ; i++, in+=row) {
		for (j=0 ; j<width ; j++, out+=4, in+=8) {
			out[0] = (in[0] + in[4] + in[row+0] + in[row+4])>>2;
			out[1] = (in[1] + in[5] + in[row+1] + in[row+5])>>2;
			out[2] = (in[2] + in[6] + in[row+2] + in[row+6])>>2;
			out[3] = (in[3] + in[7] + in[row+3] + in[row+7])>>2;
		}
	}
}


/*
==================
R_BlendOverTexture

Apply a color blend over a set of pixels
==================
*/
static void R_BlendOverTexture( byte *data, int pixelCount, byte blend[4] ) {
	int		i;
	int		inverseAlpha;
	int		premult[3];

	inverseAlpha = 255 - blend[3];
	premult[0] = blend[0] * blend[3];
	premult[1] = blend[1] * blend[3];
	premult[2] = blend[2] * blend[3];

	for ( i = 0 ; i < pixelCount ; i++, data+=4 ) {
		data[0] = ( data[0] * inverseAlpha + premult[0] ) >> 9;
		data[1] = ( data[1] * inverseAlpha + premult[1] ) >> 9;
		data[2] = ( data[2] * inverseAlpha + premult[2] ) >> 9;
	}
}

byte	mipBlendColors[16][4] = {
	{0,0,0,0},
	{255,0,0,128},
	{0,255,0,128},
	{0,0,255,128},
	{255,0,0,128},
	{0,255,0,128},
	{0,0,255,128},
	{255,0,0,128},
	{0,255,0,128},
	{0,0,255,128},
	{255,0,0,128},
	{0,255,0,128},
	{0,0,255,128},
	{255,0,0,128},
	{0,255,0,128},
	{0,0,255,128},
};


/*
===============
Upload32

===============
*/
static void Upload32( unsigned *data,
						  int img_width, int img_height, 
						  qboolean mipmap,
						  qboolean picmip,
						  qboolean isLightmap,
						  qboolean allowTC,
						  int *pformat,
						  word *pUploadWidth, word *pUploadHeight )
{
	/*if (format == GL_RGBA)
	{
	    int			samples;
	    int			i, c;
	    byte		*scan;
	    float		rMax = 0, gMax = 0, bMax = 0;
	    int			width = *pUploadWidth;
	    int			height = *pUploadHeight;

	    //
	    // perform optional picmip operation
	    //
	    if ( picmip ) {
		    for(i = 0; i < r_picmip->integer; i++) {
			    R_MipMap( (byte *)data, width, height );
			    width >>= 1;
			    height >>= 1;
			    if (width < 1) {
				    width = 1;
			    }
			    if (height < 1) {
				    height = 1;
			    }
		    }
	    }

	    //
	    // clamp to the current upper OpenGL limit
	    // scale both axis down equally so we don't have to
	    // deal with a half mip resampling
	    //
	    while ( width > glConfig.maxTextureSize	|| height > glConfig.maxTextureSize ) {
		    R_MipMap( (byte *)data, width, height );
		    width >>= 1;
		    height >>= 1;
	    }

	    //
	    // scan the texture for each channel's max values
	    // and verify if the alpha channel is being used or not
	    //
	    c = width*height;
	    scan = ((byte *)data);
	    samples = 3;
	    for ( i = 0; i < c; i++ )
	    {
		    if ( scan[i*4+0] > rMax )
		    {
			    rMax = scan[i*4+0];
		    }
		    if ( scan[i*4+1] > gMax )
		    {
			    gMax = scan[i*4+1];
		    }
		    if ( scan[i*4+2] > bMax )
		    {
			    bMax = scan[i*4+2];
		    }
		    if ( scan[i*4 + 3] != 255 )
		    {
			    samples = 4;
			    break;
		    }
	    }

	    // select proper internal format
	    if ( samples == 3 )
	    {
		    if ( glConfig.textureCompression == TC_S3TC && allowTC )
		    {
			    *pformat = GL_RGB4_S3TC;
		    }
		    else if ( glConfig.textureCompression == TC_S3TC_DXT && allowTC )
		    {	// Compress purely color - no alpha
			    if ( r_texturebits->integer == 16 ) {
				    *pformat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;	//this format cuts to 16 bit
			    }
			    else {//if we aren't using 16 bit then, use 32 bit compression
				    *pformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			    }
		    }
		    else if ( isLightmap && r_texturebitslm->integer > 0 )
		    {
			    // Allow different bit depth when we are a lightmap
			    if ( r_texturebitslm->integer == 16 )
			    {
				    *pformat = GL_RGB5;
			    }
			    else if ( r_texturebitslm->integer == 32 )
			    {
				    *pformat = GL_RGB8;
			    }
		    }
		    else if ( r_texturebits->integer == 16 )
		    {
			    *pformat = GL_RGB5;
		    }
		    else if ( r_texturebits->integer == 32 )
		    {
			    *pformat = GL_RGB8;
		    }
		    else
		    {
			    *pformat = 3;
		    }
	    }
	    else if ( samples == 4 )
	    {
		    if ( glConfig.textureCompression == TC_S3TC_DXT && allowTC)
		    {	// Compress both alpha and color
			    *pformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		    }
		    else if ( r_texturebits->integer == 16 )
		    {
			    *pformat = GL_RGBA4;
		    }
		    else if ( r_texturebits->integer == 32 )
		    {
			    *pformat = GL_RGBA8;
		    }
		    else
		    {
			    *pformat = 4;
		    }
	    }

		*pUploadWidth = width;
		*pUploadHeight = height;

	    // copy or resample data as appropriate for first MIP level
	    if (!mipmap)
	    {
		    qglTexImage2D (GL_TEXTURE_2D, 0, *pformat, width, height, 0, GL_UNSIGNED_BYTE, data);
		    goto done;
	    }

		R_LightScaleTexture (data, width, height, (qboolean)!mipmap);

	    qglTexImage2D (GL_TEXTURE_2D, 0, *pformat, width, height, 0, GL_UNSIGNED_BYTE, data );

	    if (mipmap)
	    {
		    int		miplevel;

		    miplevel = 0;
		    while (width > 1 || height > 1)
		    {
			    R_MipMap( (byte *)data, width, height );
			    width >>= 1;
			    height >>= 1;
			    if (width < 1)
				    width = 1;
			    if (height < 1)
				    height = 1;
			    miplevel++;

			    if ( r_colorMipLevels->integer )
			    {
				    R_BlendOverTexture( (byte *)data, width * height, mipBlendColors[miplevel] );
			    }

			    qglTexImage2D (GL_TEXTURE_2D, miplevel, *pformat, width, height, 0, GL_UNSIGNED_BYTE, data );
		    }
	    }
	}
	else
	{

	}

done:

	if (mipmap)
	{
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_min);
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
		if( r_ext_texture_filter_anisotropic->integer > 1 && glConfig.maxTextureFilterAnisotropy > 0 )
		{
			qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, r_ext_texture_filter_anisotropic->value );
		}
	}
	else
	{
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}

	GL_CheckErrors();*/
}

class CStringComparator
{
public:
	bool operator()(const char *s1, const char *s2) const { return(Q_stricmp(s1, s2) < 0); }
};

typedef std::map <const char *, image_t *, CStringComparator>	AllocatedImages_t;
AllocatedImages_t AllocatedImages;
AllocatedImages_t::iterator itAllocatedImages;

int giTextureBindNum = 1024;	// will be set to this anyway at runtime, but wtf?

struct Image_Upload_Data {
	byte* buffer;
	int buffer_size;
	int mip_levels;
	int base_level_width;
	int base_level_height;
};


static Vk_Image upload_vk_image(const Image_Upload_Data& upload_data, bool repeat_texture, int index) {
	int w = upload_data.base_level_width;
	int h = upload_data.base_level_height;

	bool has_alpha = false;
	for (int i = 0; i < w * h; i++) {
		if (upload_data.buffer[i*4 + 3] != 255)  {
			has_alpha = true;
			break;
		}
	}

	byte* buffer = upload_data.buffer;
	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
	int bytes_per_pixel = 4;

	if (r_texturebits->integer <= 16) {
		buffer = (byte*) //ri.Hunk_AllocateTempMemory( upload_data.buffer_size / 2 );
                R_Malloc(upload_data.buffer_size / 2 , TAG_TEMP_WORKSPACE, qfalse);  	
		format = has_alpha ? VK_FORMAT_B4G4R4A4_UNORM_PACK16 : VK_FORMAT_A1R5G5B5_UNORM_PACK16;
		bytes_per_pixel = 2;
	}

	if (format == VK_FORMAT_A1R5G5B5_UNORM_PACK16) {
		auto p = (uint16_t*)buffer;
		for (int i = 0; i < upload_data.buffer_size; i += 4, p++) {
			byte r = upload_data.buffer[i+0];
			byte g = upload_data.buffer[i+1];
			byte b = upload_data.buffer[i+2];

			*p = uint32_t((b/255.0) * 31.0 + 0.5) |
				(uint32_t((g/255.0) * 31.0 + 0.5) << 5) |
				(uint32_t((r/255.0) * 31.0 + 0.5) << 10) |
				(1 << 15);
		}
	} else if (format == VK_FORMAT_B4G4R4A4_UNORM_PACK16) {
		auto p = (uint16_t*)buffer;
		for (int i = 0; i < upload_data.buffer_size; i += 4, p++) {
			byte r = upload_data.buffer[i+0];
			byte g = upload_data.buffer[i+1];
			byte b = upload_data.buffer[i+2];
			byte a = upload_data.buffer[i+3];

			*p = uint32_t((a/255.0) * 15.0 + 0.5) |
				(uint32_t((r/255.0) * 15.0 + 0.5) << 4) |
				(uint32_t((g/255.0) * 15.0 + 0.5) << 8) |
				(uint32_t((b/255.0) * 15.0 + 0.5) << 12);
		}
	}

	Vk_Image image = vk_create_image(w, h, format, upload_data.mip_levels, repeat_texture, index);

	vk_upload_image_data(image.handle, w, h, upload_data.mip_levels > 1, buffer, bytes_per_pixel);

	if (bytes_per_pixel == 2)
		R_Free(buffer);//ri.Hunk_FreeTempMemory(buffer);

	return image;
}


static Image_Upload_Data generate_image_upload_data(const byte* data, int width, int height, qboolean mipmap, qboolean picmip) {
	//
	// convert to exact power of 2 sizes
	//
	int scaled_width, scaled_height;

	for (scaled_width = 1 ; scaled_width < width ; scaled_width<<=1)
		;
	for (scaled_height = 1 ; scaled_height < height ; scaled_height<<=1)
		;
	if ( r_roundImagesDown->integer && scaled_width > width )
		scaled_width >>= 1;
	if ( r_roundImagesDown->integer && scaled_height > height )
		scaled_height >>= 1;

	Image_Upload_Data upload_data;
	upload_data.buffer = //(byte*) ri.Hunk_AllocateTempMemory(2 * 4 * scaled_width * scaled_height);
                        (byte*)R_Malloc(2 * 4 * scaled_width * scaled_height, TAG_TEMP_WORKSPACE, qfalse);  	
	byte* resampled_buffer = nullptr;
	if ( scaled_width != width || scaled_height != height ) {
		resampled_buffer = //(byte*) ri.Hunk_AllocateTempMemory( scaled_width * scaled_height * 4 );
                            (byte*)R_Malloc(scaled_width * scaled_height * 4, TAG_TEMP_WORKSPACE, qfalse);  	
		ResampleTexture ((unsigned*)data, width, height, (unsigned*)resampled_buffer, scaled_width, scaled_height);
		data = resampled_buffer;
		width = scaled_width;
		height = scaled_height;
	}

	//
	// perform optional picmip operation
	//
	if ( picmip ) {
		scaled_width >>= r_picmip->integer;
		scaled_height >>= r_picmip->integer;
	}

	//
	// clamp to minimum size
	//
	if (scaled_width < 1) {
		scaled_width = 1;
	}
	if (scaled_height < 1) {
		scaled_height = 1;
	}

	//
	// clamp to the current upper OpenGL limit
	// scale both axis down equally so we don't have to
	// deal with a half mip resampling
	//
	int max_texture_size = 2048;
	while ( scaled_width > max_texture_size
		|| scaled_height > max_texture_size ) {
		scaled_width >>= 1;
		scaled_height >>= 1;
	}

	upload_data.base_level_width = scaled_width;
	upload_data.base_level_height = scaled_height;

	if (scaled_width == width && scaled_height == height && !mipmap) {
		upload_data.mip_levels = 1;
		upload_data.buffer_size = scaled_width * scaled_height * 4;
		Com_Memcpy(upload_data.buffer, data, upload_data.buffer_size);
		if (resampled_buffer != nullptr)
			//ri.Hunk_FreeTempMemory(resampled_buffer);
			R_Free(resampled_buffer);
		return upload_data;
	}

	// Use the normal mip-mapping to go down from [width, height] to [scaled_width, scaled_height] dimensions.
	while (width > scaled_width || height > scaled_height) {
		R_MipMap((byte *)data, width, height);

		width >>= 1;
		if (width < 1) width = 1;

		height >>= 1;
		if (height < 1) height = 1; 
	}

	// At this point width == scaled_width and height == scaled_height.

	unsigned* scaled_buffer = //(unsigned int*) ri.Hunk_AllocateTempMemory( sizeof( unsigned ) * scaled_width * scaled_height );
                            (unsigned int*) R_Malloc(sizeof( unsigned ) * scaled_width * scaled_height, TAG_TEMP_WORKSPACE, qfalse);
	Com_Memcpy(scaled_buffer, data, scaled_width * scaled_height * 4);
	R_LightScaleTexture(scaled_buffer, scaled_width, scaled_height, (qboolean) !mipmap);

	int miplevel = 0;
	int mip_level_size = scaled_width * scaled_height * 4;

	Com_Memcpy(upload_data.buffer, scaled_buffer, mip_level_size);
	upload_data.buffer_size = mip_level_size;
	
	if (mipmap) {
		while (scaled_width > 1 || scaled_height > 1) {
			R_MipMap((byte *)scaled_buffer, scaled_width, scaled_height);

			scaled_width >>= 1;
			if (scaled_width < 1) scaled_width = 1;

			scaled_height >>= 1;
			if (scaled_height < 1) scaled_height = 1;

			miplevel++;
			mip_level_size = scaled_width * scaled_height * 4;

			if ( r_colorMipLevels->integer ) {
				R_BlendOverTexture( (byte *)scaled_buffer, scaled_width * scaled_height, mipBlendColors[miplevel] );
			}

			Com_Memcpy(&upload_data.buffer[upload_data.buffer_size], scaled_buffer, mip_level_size);
			upload_data.buffer_size += mip_level_size;
		}
	}
	upload_data.mip_levels = miplevel + 1;

	//ri.Hunk_FreeTempMemory(scaled_buffer);
	R_Free(scaled_buffer);
	if (resampled_buffer != nullptr){
        //ri.Hunk_FreeTempMemory(resampled_buffer);
        R_Free(resampled_buffer);
	}
		

	return upload_data;
}

int R_Images_StartIteration(void)
{
	itAllocatedImages = AllocatedImages.begin();
	return AllocatedImages.size();
}

image_t *R_Images_GetNextIteration(void)
{
	if (itAllocatedImages == AllocatedImages.end())
		return NULL;

	image_t *pImage = (*itAllocatedImages).second;
	++itAllocatedImages;
	return pImage;
}

// clean up anything to do with an image_t struct, but caller will have to clear the internal to an image_t struct ready for either struct free() or overwrite...
//
static void R_Images_DeleteImageContents( image_t *pImage )
{
	assert(pImage);	// should never be called with NULL
	if (pImage)
	{
		//qglDeleteTextures( 1, &pImage->texnum );
		R_Free(pImage);
	}
}

static void GL_ResetBinds(void)
{
	memset( glState.currenttextures, 0, sizeof( glState.currenttextures ) );
	//if ( qglActiveTextureARB ) {
		GL_SelectTexture( 1 );
		//qglBindTexture( GL_TEXTURE_2D, 0 );
		GL_SelectTexture( 0 );
		/*qglBindTexture( GL_TEXTURE_2D, 0 );
	} else {
		qglBindTexture( GL_TEXTURE_2D, 0 );
	}*/
}

// special function used in conjunction with "devmapbsp"...
//
void R_Images_DeleteLightMaps(void)
{
	for (AllocatedImages_t::iterator itImage = AllocatedImages.begin(); itImage != AllocatedImages.end(); /* empty */)
	{
		image_t *pImage = (*itImage).second;

		if (pImage->imgName[0] == '$' /*&& strstr(pImage->imgName,"lightmap")*/)	// loose check, but should be ok
		{
			R_Images_DeleteImageContents(pImage);

			AllocatedImages.erase(itImage++);
		}
		else
		{
			++itImage;
		}
	}

	GL_ResetBinds();
}

// special function currently only called by Dissolve code...
//
void R_Images_DeleteImage(image_t *pImage)
{
	// Even though we supply the image handle, we need to get the corresponding iterator entry...
	//
	AllocatedImages_t::iterator itImage = AllocatedImages.find(pImage->imgName);
	if (itImage != AllocatedImages.end())
	{
		R_Images_DeleteImageContents(pImage);
		AllocatedImages.erase(itImage);
	}
	else
	{
		assert(0);
	}
}

// called only at app startup, vid_restart, app-exit
//
void R_Images_Clear(void)
{
	image_t *pImage;
	//	int iNumImages =
	   				  R_Images_StartIteration();
	while ( (pImage = R_Images_GetNextIteration()) != NULL)
	{
		R_Images_DeleteImageContents(pImage);
	}

	AllocatedImages.clear();
	giTextureBindNum = 1024;
    
    vk_clear_images();
}


void RE_RegisterImages_Info_f( void )
{
	image_t *pImage	= NULL;
	int iImage		= 0;
	int iTexels		= 0;

	int iNumImages	= R_Images_StartIteration();
	while ( (pImage	= R_Images_GetNextIteration()) != NULL)
	{
		ri.Printf( PRINT_ALL, "%d: (%4dx%4dy) \"%s\"",iImage, pImage->width, pImage->height, pImage->imgName);
		ri.Printf( PRINT_ALL, ", levused %d",pImage->iLastLevelUsedOn);
		ri.Printf( PRINT_ALL, "\n");

		iTexels += pImage->width * pImage->height;
		iImage++;
	}
	ri.Printf( PRINT_ALL, "%d Images. %d (%.2fMB) texels total, (not including mipmaps)\n",iNumImages, iTexels, (float)iTexels / 1024.0f / 1024.0f);
	ri.Printf( PRINT_DEVELOPER, "RE_RegisterMedia_GetLevel(): %d",RE_RegisterMedia_GetLevel());
}

// currently, this just goes through all the images and dumps any not referenced on this level...
//
qboolean RE_RegisterImages_LevelLoadEnd(void)
{
	//ri.Printf( PRINT_DEVELOPER, "RE_RegisterImages_LevelLoadEnd():\n");

	qboolean imageDeleted = qtrue;
	for (AllocatedImages_t::iterator itImage = AllocatedImages.begin(); itImage != AllocatedImages.end(); /* blank */)
	{
		qboolean bEraseOccured = qfalse;

		image_t *pImage = (*itImage).second;

		// don't un-register system shaders (*fog, *dlight, *white, *default), but DO de-register lightmaps ("$<mapname>/lightmap%d")
		if (pImage->imgName[0] != '*')
		{
			// image used on this level?
			//
			if ( pImage->iLastLevelUsedOn != RE_RegisterMedia_GetLevel() )
			{	// nope, so dump it...
				//ri.Printf( PRINT_DEVELOPER, "Dumping image \"%s\"\n",pImage->imgName);
				R_Images_DeleteImageContents(pImage);

				AllocatedImages.erase(itImage++);
				bEraseOccured = qtrue;
				imageDeleted = qtrue;
			}
		}

		if ( !bEraseOccured )
		{
			++itImage;
		}
	}

	//ri.Printf( PRINT_DEVELOPER, "RE_RegisterImages_LevelLoadEnd(): Ok\n");

	GL_ResetBinds();

	return imageDeleted;
}



// returns image_t struct if we already have this, else NULL. No disk-open performed
//	(important for creating default images).
//
// This is called by both R_FindImageFile and anything that creates default images...
//
static image_t *R_FindImageFile_NoLoad(const char *name, qboolean mipmap, qboolean allowPicmip, qboolean allowTC, bool repeat_texture )
{
	if (!name) {
		return NULL;
	}

	char *pName = GenerateImageMappingName(name);

	//
	// see if the image is already loaded
	//
	AllocatedImages_t::iterator itAllocatedImage = AllocatedImages.find(pName);
	if (itAllocatedImage != AllocatedImages.end())
	{
		image_t *pImage = (*itAllocatedImage).second;

		// the white image can be used with any set of parms, but other mismatches are errors...
		//
		if ( strcmp( pName, "*white" ) ) {
			if ( pImage->mipmap != !!mipmap ) {
				ri.Printf( PRINT_WARNING, "WARNING: reused image %s with mixed mipmap parm\n", pName );
			}
			if ( pImage->allowPicmip != !!allowPicmip ) {
				ri.Printf( PRINT_WARNING, "WARNING: reused image %s with mixed allowPicmip parm\n", pName );
			}
			/*if ( pImage->wrapClampMode != glWrapClampMode ) {
				ri.Printf( PRINT_WARNING, "WARNING: reused image %s with mixed glWrapClampMode parm\n", pName );
			}*/
		}

		pImage->iLastLevelUsedOn = RE_RegisterMedia_GetLevel();

		return pImage;
	}

	return NULL;
}


/*
================
R_CreateImage

This is the only way any image_t are created
================
*/
image_t *R_CreateImage( const char *name, const byte *pic, int width, int height, 
					   qboolean mipmap, qboolean allowPicmip, qboolean allowTC, bool repeat_texture
					   )
{
	image_t		*image;
	qboolean	isLightmap = qfalse;

	if (strlen(name) >= MAX_QPATH ) {
		Com_Error (ERR_DROP, "R_CreateImage: \"%s\" is too long\n", name);
	}

	/*if(glConfig.clampToEdgeAvailable && glWrapClampMode == GL_CLAMP) {
		glWrapClampMode = GL_CLAMP_TO_EDGE;
	}*/

	if (name[0] == '$')
	{
		isLightmap = qtrue;
	}

	if ( (width&(width-1)) || (height&(height-1)) )
	{
		Com_Error( ERR_FATAL, "R_CreateImage: %s dimensions (%i x %i) not power of 2!\n",name,width,height);
	}

	image = R_FindImageFile_NoLoad(name, mipmap, allowPicmip, allowTC, repeat_texture );
	if (image) {
		return image;
	}

	image = (image_t*) R_Malloc( sizeof( image_t ), TAG_IMAGE_T, qtrue );

	//image->imgfileSize=fileSize;

	image->texnum = 1024 + giTextureBindNum++;	// ++ is of course staggeringly important...

	// record which map it was used on...
	//
	image->iLastLevelUsedOn = RE_RegisterMedia_GetLevel();

	image->mipmap = !!mipmap;
	image->allowPicmip = !!allowPicmip;

	Q_strncpyz(image->imgName, name, sizeof(image->imgName));

	image->width = width;
	image->height = height;
	image->wrapClampMode = repeat_texture ? 0 : 1;
	
	GL_SelectTexture( 0 );


    image->index = vk_world.num_images++;
    ri.Printf( PRINT_DEVELOPER, "Loaded Image %s (%i x %i) vkidx = %i \n", name,width,height, image->index );
	Image_Upload_Data upload_data = generate_image_upload_data(pic, width, height, mipmap, allowPicmip);
	glState.currenttextures[glState.currenttmu] = 0;
	vk_world.images[image->index] = upload_vk_image(upload_data, repeat_texture, image->index);
    
	GL_Bind(image);

	R_Free(upload_data.buffer);

	const char *psNewName = GenerateImageMappingName(name);
	Q_strncpyz(image->imgName, psNewName, sizeof(image->imgName));
	AllocatedImages[ image->imgName ] = image;

	return image;
}

/*
===============
R_FindImageFile

Finds or loads the given image.
Returns NULL if it fails, not a default image.
==============
*/
image_t	*R_FindImageFile( const char *name, qboolean mipmap, qboolean allowPicmip, qboolean allowTC, bool repeat_texture ) {
	image_t	*image;
	int		width, height;
	byte	*pic;

	if (!name) {
		return NULL;
	}

	// need to do this here as well as in R_CreateImage, or R_FindImageFile_NoLoad() may complain about
	//	different clamp parms used...
	//
	/*if(glConfig.clampToEdgeAvailable && glWrapClampMode == GL_CLAMP) {
		glWrapClampMode = GL_CLAMP_TO_EDGE;
	}*/

	image = R_FindImageFile_NoLoad(name, mipmap, allowPicmip, allowTC, repeat_texture );
	if (image) {
		return image;
	}

	//
	// load the pic from disk
	//
	R_LoadImage( name, &pic, &width, &height );
	if ( !pic ) {
        return NULL;
	}

	image = R_CreateImage( ( char * ) name, pic, width, height, mipmap, allowPicmip, allowTC, repeat_texture );
	R_Free( pic );
	return image;
}


/*
================
R_CreateDlightImage
================
*/
#define	DLIGHT_SIZE	64
static void R_CreateDlightImage( void )
{
#ifdef JK2_MODE
	int		x,y;
	byte	data[DLIGHT_SIZE][DLIGHT_SIZE][4];
	int		xs, ys;
	int b;

	// The old code claims to have made a centered inverse-square falloff blob for dynamic lighting
	//	and it looked nasty, so, just doing something simpler that seems to have a much softer result
	for ( x = 0; x < DLIGHT_SIZE; x++ )
	{
		for ( y = 0; y < DLIGHT_SIZE; y++ )
		{
			xs = (DLIGHT_SIZE * 0.5f - x);
			ys = (DLIGHT_SIZE * 0.5f - y);

            b = 255 - sqrt((double) xs * xs + ys * ys ) * 9.0f; // try and generate numbers in the range of 255-0

			// should be close, but clamp anyway
			if ( b > 255 )
			{
				b = 255;
			}
			else if ( b < 0 )
			{
				b = 0;
			}
			data[y][x][0] =
			data[y][x][1] =
			data[y][x][2] = b;
			data[y][x][3] = 255;
		}
	}
	tr.dlightImage = R_CreateImage("*dlight", (byte *)data, DLIGHT_SIZE, DLIGHT_SIZE, qfalse, qfalse, qfalse, false );
#else
	int		width, height;
	byte	*pic;

	R_LoadImage("gfx/2d/dlight", &pic, &width, &height);
	if (pic)
	{
		tr.dlightImage = R_CreateImage("*dlight", pic, width, height, qfalse, qfalse, qfalse, false );
		R_Free(pic);
	}
	else
	{	// if we dont get a successful load
		int		x,y;
		byte	data[DLIGHT_SIZE][DLIGHT_SIZE][4];
		int		b;

		// make a centered inverse-square falloff blob for dynamic lighting
		for (x=0 ; x<DLIGHT_SIZE ; x++) {
			for (y=0 ; y<DLIGHT_SIZE ; y++) {
				float	d;

				d = ( DLIGHT_SIZE/2 - 0.5f - x ) * ( DLIGHT_SIZE/2 - 0.5f - x ) +
					( DLIGHT_SIZE/2 - 0.5f - y ) * ( DLIGHT_SIZE/2 - 0.5f - y );
				b = 4000 / d;
				if (b > 255) {
					b = 255;
				} else if ( b < 75 ) {
					b = 0;
				}
				data[y][x][0] =
					data[y][x][1] =
					data[y][x][2] = b;
				data[y][x][3] = 255;
			}
		}
		tr.dlightImage = R_CreateImage("*dlight", (byte *)data, DLIGHT_SIZE, DLIGHT_SIZE, qfalse, qfalse, qfalse, false );
	}
#endif
}

/*
=================
R_InitFogTable
=================
*/
void R_InitFogTable( void ) {
	int		i;
	float	d;
	float	exp;

	exp = 0.5;

	for ( i = 0 ; i < FOG_TABLE_SIZE ; i++ ) {
		d = pow ( (float)i/(FOG_TABLE_SIZE-1), exp );

		tr.fogTable[i] = d;
	}
}

/*
================
R_FogFactor

Returns a 0.0 to 1.0 fog density value
This is called for each texel of the fog texture on startup
and for each vertex of transparent shaders in fog dynamically
================
*/
float	R_FogFactor( float s, float t ) {
	float	d;

	s -= 1.0/512;
	if ( s < 0 ) {
		return 0;
	}
	if ( t < 1.0/32 ) {
		return 0;
	}
	if ( t < 31.0/32 ) {
		s *= (t - 1.0f/32.0f) / (30.0f/32.0f);
	}

	// we need to leave a lot of clamp range
	s *= 8;

	if ( s > 1.0 ) {
		s = 1.0;
	}

	d = tr.fogTable[ (int)(s * (FOG_TABLE_SIZE-1)) ];

	return d;
}

/*
================
R_CreateFogImage
================
*/
#define	FOG_S	256
#define	FOG_T	32
static void R_CreateFogImage( void ) {
	int		x,y;
	byte	*data;
	float	d;
	//float	borderColor[4];

	data = (byte*) R_Malloc( FOG_S * FOG_T * 4, TAG_TEMP_WORKSPACE, qfalse );

	// S is distance, T is depth
	for (x=0 ; x<FOG_S ; x++) {
		for (y=0 ; y<FOG_T ; y++) {
			d = R_FogFactor( ( x + 0.5f ) / FOG_S, ( y + 0.5f ) / FOG_T );

			data[(y*FOG_S+x)*4+0] =
			data[(y*FOG_S+x)*4+1] =
			data[(y*FOG_S+x)*4+2] = 255;
			data[(y*FOG_S+x)*4+3] = 255*d;
		}
	}
	// standard openGL clamping doesn't really do what we want -- it includes
	// the border color at the edges.  OpenGL 1.2 has clamp-to-edge, which does
	// what we want.
	tr.fogImage = R_CreateImage("*fog", (byte *)data, FOG_S, FOG_T, qfalse, qfalse, qfalse, false);
	R_Free( data );

	//qglTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );
}

/*
==================
R_CreateDefaultImage
==================
*/
#define	DEFAULT_SIZE	16
static void R_CreateDefaultImage( void ) {
	int		x;
	byte	data[DEFAULT_SIZE][DEFAULT_SIZE][4];

	// the default image will be a box, to allow you to see the mapping coordinates
	memset( data, 32, sizeof( data ) );
	for ( x = 0 ; x < DEFAULT_SIZE ; x++ ) {
		data[0][x][0] =
		data[0][x][1] =
		data[0][x][2] =
		data[0][x][3] = 255;

		data[x][0][0] =
		data[x][0][1] =
		data[x][0][2] =
		data[x][0][3] = 255;

		data[DEFAULT_SIZE-1][x][0] =
		data[DEFAULT_SIZE-1][x][1] =
		data[DEFAULT_SIZE-1][x][2] =
		data[DEFAULT_SIZE-1][x][3] = 255;

		data[x][DEFAULT_SIZE-1][0] =
		data[x][DEFAULT_SIZE-1][1] =
		data[x][DEFAULT_SIZE-1][2] =
		data[x][DEFAULT_SIZE-1][3] = 255;
	}
	tr.defaultImage = R_CreateImage("*default", (byte *)data, DEFAULT_SIZE, DEFAULT_SIZE, qtrue, qfalse, qtrue, true);
}

/*
==================
R_CreateBuiltinImages
==================
*/
void R_UpdateSaveGameImage(const char *filename);

void R_CreateBuiltinImages( void ) {
	int		x,y;
	byte	data[DEFAULT_SIZE][DEFAULT_SIZE][4];

	R_CreateDefaultImage();

	// we use a solid white image instead of disabling texturing
	memset( data, 255, sizeof( data ) );

	tr.whiteImage = R_CreateImage("*white", (byte *)data, 8, 8, qfalse, qfalse, qtrue, true);

	tr.screenImage = R_CreateImage("*screen", (byte *)data, 8, 8, qfalse, qfalse, qfalse, true );


	// Create the scene glow image. - AReis
	/*tr.screenGlow = 1024 + giTextureBindNum++;
	qglDisable( GL_TEXTURE_2D );
	qglEnable( GL_TEXTURE_RECTANGLE_ARB );
	qglBindTexture( GL_TEXTURE_RECTANGLE_ARB, tr.screenGlow );
	qglTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA16, glConfig.vidWidth, glConfig.vidHeight, 0, GL_RGB, GL_FLOAT, 0 );
	qglTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	qglTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	qglTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	qglTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );*/

	// Create the scene image. - AReis
	/*tr.sceneImage = 1024 + giTextureBindNum++;
	qglBindTexture( GL_TEXTURE_RECTANGLE_ARB, tr.sceneImage );
	qglTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA16, glConfig.vidWidth, glConfig.vidHeight, 0, GL_RGB, GL_FLOAT, 0 );
	qglTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	qglTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	qglTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	qglTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );*/

	// Create the minimized scene blur image.
	/*if ( r_DynamicGlowWidth->integer > glConfig.vidWidth  )
	{
		r_DynamicGlowWidth->integer = glConfig.vidWidth;
	}
	if ( r_DynamicGlowHeight->integer > glConfig.vidHeight  )
	{
		r_DynamicGlowHeight->integer = glConfig.vidHeight;
	}
	tr.blurImage = 1024 + giTextureBindNum++;
	qglBindTexture( GL_TEXTURE_RECTANGLE_ARB, tr.blurImage );
	qglTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA16, r_DynamicGlowWidth->integer, r_DynamicGlowHeight->integer, 0, GL_RGB, GL_FLOAT, 0 );
	qglTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	qglTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	qglTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	qglTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	qglDisable( GL_TEXTURE_RECTANGLE_ARB );
	qglEnable( GL_TEXTURE_2D );*/


	// with overbright bits active, we need an image which is some fraction of full color,
	// for default lightmaps, etc
	for (x=0 ; x<DEFAULT_SIZE ; x++) {
		for (y=0 ; y<DEFAULT_SIZE ; y++) {
			data[y][x][0] =
			data[y][x][1] =
			data[y][x][2] = tr.identityLightByte;
			data[y][x][3] = 255;
		}
	}


	tr.identityLightImage = R_CreateImage("*identityLight", (byte *)data, 8, 8, qfalse, qfalse, qtrue, true);

	// scratchimage is usually used for cinematic drawing
	for(x=0;x<NUM_SCRATCH_IMAGES;x++) {
		// scratchimage is usually used for cinematic drawing
		tr.scratchImage[x] = R_CreateImage(va("*scratch%d",x), (byte *)data, DEFAULT_SIZE, DEFAULT_SIZE, qfalse, qfalse, qfalse, false);
	}

	R_CreateDlightImage();
	R_CreateFogImage();
}


/*
===============
R_SetColorMappings
===============
*/
void R_SetColorMappings( void ) {
	int		i, j;
	float	g;
	int		inf;
	int		shift;

	// setup the overbright lighting
	tr.overbrightBits = r_overBrightBits->integer;
	if ( !glConfig.deviceSupportsGamma ) {
		tr.overbrightBits = 0;		// need hardware gamma for overbright
	}

	// never overbright in windowed mode
	if ( !glConfig.isFullscreen )
	{
		tr.overbrightBits = 0;
	}

	if ( tr.overbrightBits > 1 ) {
		tr.overbrightBits = 1;
	}
	if ( tr.overbrightBits < 0 ) {
		tr.overbrightBits = 0;
	}

	tr.identityLight = 1.0 / ( 1 << tr.overbrightBits );
	tr.identityLightByte = 255 * tr.identityLight;


	if ( r_intensity->value < 1.0f ) {
		ri.Cvar_Set( "r_intensity", "1.0" );
	}

	if ( r_gamma->value < 0.5f ) {
		ri.Cvar_Set( "r_gamma", "0.5" );
	} else if ( r_gamma->value > 3.0f ) {
		ri.Cvar_Set( "r_gamma", "3.0" );
	}

	g = r_gamma->value;

	shift = tr.overbrightBits;

	for ( i = 0; i < 256; i++ ) {
		if ( g == 1 ) {
			inf = i;
		} else {
			inf = 255 * pow ( i/255.0f, 1.0f / g ) + 0.5f;
		}
		inf <<= shift;
		if (inf < 0) {
			inf = 0;
		}
		if (inf > 255) {
			inf = 255;
		}
		s_gammatable[i] = inf;
	}

	for (i=0 ; i<256 ; i++) {
		j = i * r_intensity->value;
		if (j > 255) {
			j = 255;
		}
		s_intensitytable[i] = j;
	}

	if ( glConfig.deviceSupportsGamma )
	{
		ri.WIN_SetGamma( &glConfig, s_gammatable, s_gammatable, s_gammatable );
	}
}

/*
===============
R_InitImages
===============
*/
void	R_InitImages( void ) {
	//memset(hashTable, 0, sizeof(hashTable));	// DO NOT DO THIS NOW (because of image cacheing)	-ste.

	// build brightness translation tables
	R_SetColorMappings();

	// create default texture and white texture
	R_CreateBuiltinImages();
}

/*
===============
R_DeleteTextures
===============
*/
// (only gets called during vid_restart now (and app exit), not during map load)
//
void R_DeleteTextures( void ) {

	R_Images_Clear();
	GL_ResetBinds();
}

