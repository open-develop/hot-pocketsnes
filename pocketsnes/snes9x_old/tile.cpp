/*
 * Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.
 *
 * (c) Copyright 1996 - 2001 Gary Henderson (gary.henderson@ntlworld.com) and
 *                           Jerremy Koot (jkoot@snes9x.com)
 *
 * Super FX C emulator code 
 * (c) Copyright 1997 - 1999 Ivar (ivar@snes9x.com) and
 *                           Gary Henderson.
 * Super FX assembler emulator code (c) Copyright 1998 zsKnight and _Demo_.
 *
 * DSP1 emulator code (c) Copyright 1998 Ivar, _Demo_ and Gary Henderson.
 * C4 asm and some C emulation code (c) Copyright 2000 zsKnight and _Demo_.
 * C4 C code (c) Copyright 2001 Gary Henderson (gary.henderson@ntlworld.com).
 *
 * DOS port code contains the works of other authors. See headers in
 * individual files.
 *
 * Snes9x homepage: http://www.snes9x.com
 *
 * Permission to use, copy, modify and distribute Snes9x in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Snes9x is freeware for PERSONAL USE only. Commercial users should
 * seek permission of the copyright holders first. Commercial use includes
 * charging money for Snes9x or software derived from Snes9x.
 *
 * The copyright holders request that bug fixes and improvements to the code
 * should be forwarded to them so everyone can benefit from the modifications
 * in future versions.
 *
 * Super NES and Super Nintendo Entertainment System are trademarks of
 * Nintendo Co., Limited and its subsidiary companies.
 */
#include "snes9x.h"

#include "memmap.h"
#include "ppu.h"
#include "display.h"
#include "gfx.h"
#include "tile.h"

#ifdef USE_GLIDE
#include "3d.h"
#endif

extern uint32 HeadMask [4];
extern uint32 TailMask [5];

uint8 ConvertTile (uint8 *pCache, uint32 TileAddr)
{
    register uint8 *tp = &Memory.VRAM[TileAddr];
    uint32 *p = (uint32 *) pCache;
    uint32 non_zero = 0;
    uint8 line;
	uint32 p1;
	uint32 p2;
	register uint8 pix;

    switch (BG.BitShift)
    {
    case 8:
	for (line = 8; line != 0; line--, tp += 2)
	{
	    p1 = p2 = 0;
		if ((pix = *(tp + 0)))
	    {
		p1 |= odd_high[0][pix >> 4];
		p2 |= odd_low[0][pix & 0xf];
	    }
	    if ((pix = *(tp + 1)))
	    {
		p1 |= even_high[0][pix >> 4];
		p2 |= even_low[0][pix & 0xf];
	    }
	    if ((pix = *(tp + 16)))
	    {
		p1 |= odd_high[1][pix >> 4];
		p2 |= odd_low[1][pix & 0xf];
	    }
	    if ((pix = *(tp + 17)))
	    {
		p1 |= even_high[1][pix >> 4];
		p2 |= even_low[1][pix & 0xf];
	    }
	    if ((pix = *(tp + 32)))
	    {
		p1 |= odd_high[2][pix >> 4];
		p2 |= odd_low[2][pix & 0xf];
	    }
	    if ((pix = *(tp + 33)))
	    {
		p1 |= even_high[2][pix >> 4];
		p2 |= even_low[2][pix & 0xf];
	    }
	    if ((pix = *(tp + 48)))
	    {
		p1 |= odd_high[3][pix >> 4];
		p2 |= odd_low[3][pix & 0xf];
	    }
	    if ((pix = *(tp + 49)))
	    {
		p1 |= even_high[3][pix >> 4];
		p2 |= even_low[3][pix & 0xf];
	    }
	    *p++ = p1;
	    *p++ = p2;
	    non_zero |= p1 | p2;
	}
	break;

    case 4:
	for (line = 8; line != 0; line--, tp += 2)
	{
	    p1 = p2 = 0;
	    if ((pix = *(tp + 0)))
	    {
		p1 |= odd_high[0][pix >> 4];
		p2 |= odd_low[0][pix & 0xf];
	    }
	    if ((pix = *(tp + 1)))
	    {
		p1 |= even_high[0][pix >> 4];
		p2 |= even_low[0][pix & 0xf];
	    }
	    if ((pix = *(tp + 16)))
	    {
		p1 |= odd_high[1][pix >> 4];
		p2 |= odd_low[1][pix & 0xf];
	    }
	    if ((pix = *(tp + 17)))
	    {
		p1 |= even_high[1][pix >> 4];
		p2 |= even_low[1][pix & 0xf];
	    }
	    *p++ = p1;
	    *p++ = p2;
	    non_zero |= p1 | p2;
	}
	break;

    case 2:
	for (line = 8; line != 0; line--, tp += 2)
	{
	    p1 = p2 = 0;
	    if ((pix = *(tp + 0)))
	    {
		p1 |= odd_high[0][pix >> 4];
		p2 |= odd_low[0][pix & 0xf];
	    }
	    if ((pix = *(tp + 1)))
	    {
		p1 |= even_high[0][pix >> 4];
		p2 |= even_low[0][pix & 0xf];
	    }
	    *p++ = p1;
	    *p++ = p2;
	    non_zero |= p1 | p2;
	}
	break;
    }
    return (non_zero ? TRUE : BLANK_TILE);
}

INLINE void WRITE_4PIXELS (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint8 Pixel;
    uint8 *Screen = gfx->S + Offset;
    uint8 *Depth = gfx->DB + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[N])) \
    { \
	Screen [N] = (uint8) gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

INLINE void WRITE_4PIXELS_FLIPPED (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint8 Pixel;
    uint8 *Screen = gfx->S + Offset;
    uint8 *Depth = gfx->DB + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[3 - N])) \
    { \
	Screen [N] = (uint8) gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

inline void WRITE_4PIXELSHI16 (uint32 Offset, uint8 *Pixels)
{
    uint32 Pixel;
    uint16 *Screen = (uint16 *) GFX.S + Offset;
    uint8  *Depth = GFX.DB + Offset;

#define FN(N) \
    if (GFX.Z1 > Depth [N] && (Pixel = Pixels[2*N])) \
    { \
	Screen [N] = GFX.ScreenColors [Pixel]; \
	Depth [N] = GFX.Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

inline void WRITE_4PIXELSHI16_FLIPPED (uint32 Offset, uint8 *Pixels)
{
    uint32 Pixel;
    uint16 *Screen = (uint16 *) GFX.S + Offset;
    uint8  *Depth = GFX.DB + Offset;

#define FN(N) \
    if (GFX.Z1 > Depth [N] && (Pixel = Pixels[6 - 2*N])) \
    { \
	Screen [N] = GFX.ScreenColors [Pixel]; \
	Depth [N] = GFX.Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

INLINE void WRITE_4PIXELSx2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint8 Pixel;
    uint8 *Screen = gfx->S + Offset;
    uint8 *Depth = gfx->DB + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [0] && (Pixel = Pixels[N])) \
    { \
	Screen [N * 2] = Screen [N * 2 + 1] = (uint8) gfx->ScreenColors [Pixel]; \
	Depth [N * 2] = Depth [N * 2 + 1] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

INLINE void WRITE_4PIXELS_FLIPPEDx2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint8 Pixel;
    uint8 *Screen = gfx->S + Offset;
    uint8 *Depth = gfx->DB + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N * 2] && (Pixel = Pixels[3 - N])) \
    { \
	Screen [N * 2] = Screen [N * 2 + 1] = (uint8) gfx->ScreenColors [Pixel]; \
	Depth [N * 2] = Depth [N * 2 + 1] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

INLINE void WRITE_4PIXELSx2x2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint8 Pixel;
    uint8 *Screen = gfx->S + Offset;
    uint8 *Depth = gfx->DB + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N * 2] && (Pixel = Pixels[N])) \
    { \
	Screen [N * 2] = Screen [N * 2 + 1] = Screen [GFX_PITCH + N * 2] =  \
	    Screen [GFX_PITCH + N * 2 + 1] = (uint8) gfx->ScreenColors [Pixel]; \
	Depth [N * 2] = Depth [N * 2 + 1] = Depth [GFX_PITCH + N * 2] = \
	    Depth [GFX_PITCH + N * 2 + 1] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

INLINE void WRITE_4PIXELS_FLIPPEDx2x2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint8 Pixel;
    uint8 *Screen = gfx->S + Offset;
    uint8 *Depth = gfx->DB + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N * 2] && (Pixel = Pixels[3 - N])) \
    { \
	Screen [N * 2] = Screen [N * 2 + 1] = Screen [GFX_PITCH + N * 2] =  \
	    Screen [GFX_PITCH + N * 2 + 1] = (uint8) gfx->ScreenColors [Pixel]; \
	Depth [N * 2] = Depth [N * 2 + 1] = Depth [GFX_PITCH + N * 2] = \
	    Depth [GFX_PITCH + N * 2 + 1] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

void DrawTile (uint32 Tile, uint32 Offset, uint32 StartLine,
	       uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE

    register uint8 *bp;

    RENDER_TILE(WRITE_4PIXELS, WRITE_4PIXELS_FLIPPED, 4)
}

void DrawClippedTile (uint32 Tile, uint32 Offset,
		      uint32 StartPixel, uint32 Width,
		      uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILE(WRITE_4PIXELS, WRITE_4PIXELS_FLIPPED, 4)
}

void DrawTilex2 (uint32 Tile, uint32 Offset, uint32 StartLine,
		 uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE

    register uint8 *bp;

    RENDER_TILE(WRITE_4PIXELSx2, WRITE_4PIXELS_FLIPPEDx2, 8)
}

void DrawClippedTilex2 (uint32 Tile, uint32 Offset,
			uint32 StartPixel, uint32 Width,
			uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILE(WRITE_4PIXELSx2, WRITE_4PIXELS_FLIPPEDx2, 8)
}

void DrawTilex2x2 (uint32 Tile, uint32 Offset, uint32 StartLine,
		   uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE

    register uint8 *bp;

    RENDER_TILE(WRITE_4PIXELSx2x2, WRITE_4PIXELS_FLIPPEDx2x2, 8)
}

void DrawClippedTilex2x2 (uint32 Tile, uint32 Offset,
			  uint32 StartPixel, uint32 Width,
			  uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILE(WRITE_4PIXELSx2x2, WRITE_4PIXELS_FLIPPEDx2x2, 8)
}

void DrawLargePixel (uint32 Tile, uint32 Offset,
		     uint32 StartPixel, uint32 Pixels,
		     uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE

    register uint8 *sp = gfx->S + Offset;
    uint8  *Depth = gfx->DB + Offset;
    uint8 pixel;
#define PLOT_PIXEL(screen, pixel) (pixel)

    RENDER_TILE_LARGE (((uint8) gfx->ScreenColors [pixel]), PLOT_PIXEL)
}

INLINE void WRITE_4PIXELS16 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->DB + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[N])) \
    { \
	Screen [N] = gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

INLINE void WRITE_4PIXELS16_FLIPPED (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->DB + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[3 - N])) \
    { \
	Screen [N] = gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

INLINE void WRITE_4PIXELS16x2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->DB + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N * 2] && (Pixel = Pixels[N])) \
    { \
	Screen [N * 2] = Screen [N * 2 + 1] = gfx->ScreenColors [Pixel]; \
	Depth [N * 2] = Depth [N * 2 + 1] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

INLINE void WRITE_4PIXELS16_FLIPPEDx2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->DB + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N * 2] && (Pixel = Pixels[3 - N])) \
    { \
	Screen [N * 2] = Screen [N * 2 + 1] = gfx->ScreenColors [Pixel]; \
	Depth [N * 2] = Depth [N * 2 + 1] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

INLINE void WRITE_4PIXELS16x2x2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->DB + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N * 2] && (Pixel = Pixels[N])) \
    { \
	Screen [N * 2] = Screen [N * 2 + 1] = Screen [(GFX_PITCH >> 1) + N * 2] = \
	    Screen [(GFX_PITCH >> 1) + N * 2 + 1] = gfx->ScreenColors [Pixel]; \
	Depth [N * 2] = Depth [N * 2 + 1] = Depth [(GFX_PITCH >> 1) + N * 2] = \
	    Depth [(GFX_PITCH >> 1) + N * 2 + 1] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

INLINE void WRITE_4PIXELS16_FLIPPEDx2x2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->DB + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N * 2] && (Pixel = Pixels[3 - N])) \
    { \
	Screen [N * 2] = Screen [N * 2 + 1] = Screen [(GFX_PITCH >> 1) + N * 2] = \
	    Screen [(GFX_PITCH >> 1) + N * 2 + 1] = gfx->ScreenColors [Pixel]; \
	Depth [N * 2] = Depth [N * 2 + 1] = Depth [(GFX_PITCH >> 1) + N * 2] = \
	    Depth [(GFX_PITCH >> 1) + N * 2 + 1] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)
#undef FN
}

void DrawTile16_OBJ (uint32 Tile, uint32 Offset, uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
//	TILE_PREAMBLE

    uint8 *pCache;
    uint32 TileAddr = BG.TileAddress + ((Tile & 0x3ff) << 5);
    if (Tile & 0x100){
			TileAddr += BG.NameSelect;
		}

    TileAddr &= 0xffff;

    uint32 TileNumber;
    pCache = &BG.Buffer[(TileNumber = (TileAddr >> 5)) << 6];

    if (!BG.Buffered [TileNumber]){
		BG.Buffered[TileNumber] = ConvertTile (pCache, TileAddr);
	}

    if (BG.Buffered [TileNumber] == BLANK_TILE){
		TileBlank = Tile & 0xFFFFFFFF;
		return;
	}

	gfx->ScreenColors = &IPPU.ScreenColors [(((Tile >> 10) & 7) << 4) + 128];

    register uint8*	bp;
	register int	inc;

    if (Tile & V_FLIP){
		bp  = pCache + 56 - StartLine;
		inc = -8;
	} else {
		bp  = pCache + StartLine;
		inc = 8;
	}

    uint16*	Screen = (uint16 *) gfx->S + Offset;
	uint16* Colors =  gfx->ScreenColors;
    uint8*	Depth = gfx->DB + Offset;
	int		GFX_Z1  = GFX.Z1;
	int		GFX_Z2  = GFX.Z2;

    if (!(Tile & H_FLIP)){
#define FN(N) \
    if (GFX_Z1 > Depth[N] && bp[N]){ \
		Screen[N] = Colors[bp[N]]; \
		Depth[N]  = GFX_Z2; \
	}
		while ( LineCount-- ){
		    if ( *(uint32*)bp ){
				FN(0); FN(1); FN(2); FN(3);
			}

		    if ( *(uint32 *)(bp + 4) ){
				FN(4); FN(5); FN(6); FN(7);
			}
			bp += inc;
			Screen += GFX_PPL;
			Depth  += GFX_PPL;
		}
#undef FN
	} else {
#define FN(N, B) \
	if (GFX_Z1 > Depth[N] && bp[B]){ \
		Screen[N] = Colors[bp[B]]; \
		Depth[N]  = GFX_Z2; \
    }
		while ( LineCount-- ){
		    if ( *(uint32 *)(bp + 4) ){
				FN(0, 7); FN(1, 6); FN(2, 5); FN(3, 4);
			}

		    if ( *(uint32*)bp ){
				FN(4, 3); FN(5, 2); FN(6, 1); FN(7, 0);
			}
			bp += inc;
			Screen += GFX_PPL;
			Depth  += GFX_PPL;
		}
#undef FN
	}
}

void DrawTile16 (uint32 Tile, uint32 Offset, uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
//	TILE_PREAMBLE

    uint8 *pCache;
    uint32 TileAddr = BG.TileAddress + ((Tile & 0x3ff) << BG.TileShift);

    TileAddr &= 0xffff;

    uint32 TileNumber;
    pCache = &BG.Buffer[(TileNumber = (TileAddr >> BG.TileShift)) << 6];

    if (!BG.Buffered [TileNumber]){
		BG.Buffered[TileNumber] = ConvertTile (pCache, TileAddr);
	}

    if (BG.Buffered [TileNumber] == BLANK_TILE){
		TileBlank = Tile & 0xFFFFFFFF;
		return;
	}

    if (BG.DirectColourMode){
		if (IPPU.DirectColourMapsNeedRebuild){
			S9xBuildDirectColourMaps ();
		}
        gfx->ScreenColors = DirectColourMaps [(Tile >> 10) & BG.PaletteMask];
    } else {
		gfx->ScreenColors = &IPPU.ScreenColors [(((Tile >> 10) & BG.PaletteMask) << BG.PaletteShift) + BG.StartPalette];
	}

    register uint8*	bp;
	register int	inc;

    if (Tile & V_FLIP){
		bp  = pCache + 56 - StartLine;
		inc = -8;
	} else {
		bp  = pCache + StartLine;
		inc = 8;
	}

    uint16*	Screen = (uint16 *) gfx->S + Offset;
	uint16* Colors =  gfx->ScreenColors;
    uint8*	Depth = gfx->DB + Offset;
	int		GFX_Z1  = gfx->Z1;
//	int		GFX_Z2  = GFX.Z2;

    if (!(Tile & H_FLIP)){
#define FN(N) \
    if (GFX_Z1 > Depth[N] && bp[N]){ \
		Screen[N] = Colors[bp[N]]; \
		Depth[N]  = GFX_Z1; \
	}
		while ( LineCount-- ){
		    if ( *(uint32*)bp ){
				FN(0); FN(1); FN(2); FN(3);
			}

		    if ( *(uint32 *)(bp + 4) ){
				FN(4); FN(5); FN(6); FN(7);
			}
			bp += inc;
			Screen += GFX_PPL;
			Depth  += GFX_PPL;
		}
#undef FN
	} else {
#define FN(N, B) \
	if (GFX_Z1 > Depth[N] && bp[B]){ \
		Screen[N] = Colors[bp[B]]; \
		Depth[N]  = GFX_Z1; \
    }
		while ( LineCount-- ){
		    if ( *(uint32 *)(bp + 4) ){
				FN(0, 7); FN(1, 6); FN(2, 5); FN(3, 4);
			}

		    if ( *(uint32*)bp ){
				FN(4, 3); FN(5, 2); FN(6, 1); FN(7, 0);
			}
			bp += inc;
			Screen += GFX_PPL;
			Depth  += GFX_PPL;
		}
#undef FN
	}
}

void DrawClippedTile16 (uint32 Tile, uint32 Offset,
			uint32 StartPixel, uint32 Width,
			uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILE(WRITE_4PIXELS16, WRITE_4PIXELS16_FLIPPED, 4)
}

void DrawTile16x2 (uint32 Tile, uint32 Offset, uint32 StartLine,
		   uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    RENDER_TILE(WRITE_4PIXELS16x2, WRITE_4PIXELS16_FLIPPEDx2, 8)
}

void DrawClippedTile16x2 (uint32 Tile, uint32 Offset,
			  uint32 StartPixel, uint32 Width,
			  uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILE(WRITE_4PIXELS16x2, WRITE_4PIXELS16_FLIPPEDx2, 8)
}

void DrawTile16x2x2 (uint32 Tile, uint32 Offset, uint32 StartLine,
		     uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    RENDER_TILE(WRITE_4PIXELS16x2x2, WRITE_4PIXELS16_FLIPPEDx2x2, 8)
}

void DrawClippedTile16x2x2 (uint32 Tile, uint32 Offset,
			    uint32 StartPixel, uint32 Width,
			    uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILE(WRITE_4PIXELS16x2x2, WRITE_4PIXELS16_FLIPPEDx2x2, 8)
}

void DrawLargePixel16 (uint32 Tile, uint32 Offset,
		       uint32 StartPixel, uint32 Pixels,
		       uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE

    register uint16 *sp = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->DB + Offset;
    uint16 pixel;

    RENDER_TILE_LARGE (gfx->ScreenColors [pixel], PLOT_PIXEL)
}

INLINE void WRITE_4PIXELS16_ADD (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint8  *SubDepth = gfx->SubZBuffer + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[N])) \
    { \
	if (SubDepth [N]) \
	{ \
	    if (SubDepth [N] != 1) \
		Screen [N] = COLOR_ADD (gfx->ScreenColors [Pixel], \
					Screen [gfx->Delta + N]); \
	    else \
		Screen [N] = COLOR_ADD (gfx->ScreenColors [Pixel], \
					gfx->FixedColour); \
	} \
	else \
	    Screen [N] = gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)

#undef FN
}

INLINE void WRITE_4PIXELS16_FLIPPED_ADD (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint8  *SubDepth = gfx->SubZBuffer + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[3 - N])) \
    { \
	if (SubDepth [N]) \
	{ \
	    if (SubDepth [N] != 1) \
		Screen [N] = COLOR_ADD (gfx->ScreenColors [Pixel], \
					Screen [gfx->Delta + N]); \
	    else \
		Screen [N] = COLOR_ADD (gfx->ScreenColors [Pixel], \
					gfx->FixedColour); \
	} \
	else \
	    Screen [N] = gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)

#undef FN
}

INLINE void WRITE_4PIXELS16_ADD1_2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint8  *SubDepth = gfx->SubZBuffer + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[N])) \
    { \
	if (SubDepth [N]) \
	{ \
	    if (SubDepth [N] != 1) \
		Screen [N] = (uint16) (COLOR_ADD1_2 (gfx->ScreenColors [Pixel], \
						     Screen [gfx->Delta + N])); \
	    else \
		Screen [N] = COLOR_ADD (gfx->ScreenColors [Pixel], \
					gfx->FixedColour); \
	} \
	else \
	    Screen [N] = gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)

#undef FN
}

INLINE void WRITE_4PIXELS16_FLIPPED_ADD1_2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint8  *SubDepth = gfx->SubZBuffer + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[3 - N])) \
    { \
	if (SubDepth [N]) \
	{ \
	    if (SubDepth [N] != 1) \
		Screen [N] = (uint16) (COLOR_ADD1_2 (gfx->ScreenColors [Pixel], \
						     Screen [gfx->Delta + N])); \
	    else \
		Screen [N] = COLOR_ADD (gfx->ScreenColors [Pixel], \
					gfx->FixedColour); \
	} \
	else \
	    Screen [N] = gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)

#undef FN
}

INLINE void WRITE_4PIXELS16_SUB (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint8  *SubDepth = gfx->SubZBuffer + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[N])) \
    { \
	if (SubDepth [N]) \
	{ \
	    if (SubDepth [N] != 1) \
		Screen [N] = (uint16) COLOR_SUB (gfx->ScreenColors [Pixel], \
					Screen [gfx->Delta + N]); \
	    else \
		Screen [N] = (uint16) COLOR_SUB (gfx->ScreenColors [Pixel], \
					gfx->FixedColour); \
	} \
	else \
	    Screen [N] = gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)

#undef FN
}

INLINE void WRITE_4PIXELS16_FLIPPED_SUB (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint8  *SubDepth = gfx->SubZBuffer + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[3 - N])) \
    { \
	if (SubDepth [N]) \
	{ \
	    if (SubDepth [N] != 1) \
		Screen [N] = (uint16) COLOR_SUB (gfx->ScreenColors [Pixel], \
					Screen [gfx->Delta + N]); \
	    else \
		Screen [N] = (uint16) COLOR_SUB (gfx->ScreenColors [Pixel], \
					gfx->FixedColour); \
	} \
	else \
	    Screen [N] = gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)

#undef FN
}

INLINE void WRITE_4PIXELS16_SUB1_2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint8  *SubDepth = gfx->SubZBuffer + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[N])) \
    { \
	if (SubDepth [N]) \
	{ \
	    if (SubDepth [N] != 1) \
		Screen [N] = (uint16) COLOR_SUB1_2 (gfx->ScreenColors [Pixel], \
					   Screen [gfx->Delta + N]); \
	    else \
		Screen [N] = (uint16) COLOR_SUB (gfx->ScreenColors [Pixel], \
					gfx->FixedColour); \
	} \
	else \
	    Screen [N] = gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)

#undef FN
}

INLINE void WRITE_4PIXELS16_FLIPPED_SUB1_2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint8  *SubDepth = gfx->SubZBuffer + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[3 - N])) \
    { \
	if (SubDepth [N]) \
	{ \
	    if (SubDepth [N] != 1) \
		Screen [N] = (uint16) COLOR_SUB1_2 (gfx->ScreenColors [Pixel], \
					   Screen [gfx->Delta + N]); \
	    else \
		Screen [N] = (uint16) COLOR_SUB (gfx->ScreenColors [Pixel], \
					gfx->FixedColour); \
	} \
	else \
	    Screen [N] = gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)

#undef FN
}


void DrawTile16Add (uint32 Tile, uint32 Offset, uint32 StartLine,
		    uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    RENDER_TILE(WRITE_4PIXELS16_ADD, WRITE_4PIXELS16_FLIPPED_ADD, 4)
}

void DrawClippedTile16Add (uint32 Tile, uint32 Offset,
			   uint32 StartPixel, uint32 Width,
			   uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILE(WRITE_4PIXELS16_ADD, WRITE_4PIXELS16_FLIPPED_ADD, 4)
}

void DrawTile16Add1_2 (uint32 Tile, uint32 Offset, uint32 StartLine,
		       uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    RENDER_TILE(WRITE_4PIXELS16_ADD1_2, WRITE_4PIXELS16_FLIPPED_ADD1_2, 4)
}

void DrawClippedTile16Add1_2 (uint32 Tile, uint32 Offset,
			      uint32 StartPixel, uint32 Width,
			      uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILE(WRITE_4PIXELS16_ADD1_2, WRITE_4PIXELS16_FLIPPED_ADD1_2, 4)
}

void DrawTile16Sub (uint32 Tile, uint32 Offset, uint32 StartLine,
		    uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    RENDER_TILE(WRITE_4PIXELS16_SUB, WRITE_4PIXELS16_FLIPPED_SUB, 4)
}

void DrawClippedTile16Sub (uint32 Tile, uint32 Offset,
			   uint32 StartPixel, uint32 Width,
			   uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILE(WRITE_4PIXELS16_SUB, WRITE_4PIXELS16_FLIPPED_SUB, 4)
}

void DrawTile16Sub1_2 (uint32 Tile, uint32 Offset, uint32 StartLine,
		       uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    RENDER_TILE(WRITE_4PIXELS16_SUB1_2, WRITE_4PIXELS16_FLIPPED_SUB1_2, 4)
}

void DrawClippedTile16Sub1_2 (uint32 Tile, uint32 Offset,
			      uint32 StartPixel, uint32 Width,
			      uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILE(WRITE_4PIXELS16_SUB1_2, WRITE_4PIXELS16_FLIPPED_SUB1_2, 4)
}

INLINE void WRITE_4PIXELS16_ADDF1_2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint8  *SubDepth = gfx->SubZBuffer + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[N])) \
    { \
	if (SubDepth [N] == 1) \
	    Screen [N] = (uint16) (COLOR_ADD1_2 (gfx->ScreenColors [Pixel], \
						 gfx->FixedColour)); \
	else \
	    Screen [N] = gfx->ScreenColors [Pixel];\
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)

#undef FN
}

INLINE void WRITE_4PIXELS16_FLIPPED_ADDF1_2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint8  *SubDepth = gfx->SubZBuffer + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[3 - N])) \
    { \
	if (SubDepth [N] == 1) \
	    Screen [N] = (uint16) (COLOR_ADD1_2 (gfx->ScreenColors [Pixel], \
						 gfx->FixedColour)); \
	else \
	    Screen [N] = gfx->ScreenColors [Pixel];\
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)

#undef FN
}

INLINE void WRITE_4PIXELS16_SUBF1_2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint8  *SubDepth = gfx->SubZBuffer + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[N])) \
    { \
	if (SubDepth [N] == 1) \
	    Screen [N] = (uint16) COLOR_SUB1_2 (gfx->ScreenColors [Pixel], \
						gfx->FixedColour); \
	else \
	    Screen [N] = gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)

#undef FN
}

INLINE void WRITE_4PIXELS16_FLIPPED_SUBF1_2 (uint32 Offset, uint8 *Pixels, struct SGFX * gfx)
{
    register uint32 Pixel;
    uint16 *Screen = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint8  *SubDepth = gfx->SubZBuffer + Offset;

#define FN(N) \
    if (gfx->Z1 > Depth [N] && (Pixel = Pixels[3 - N])) \
    { \
	if (SubDepth [N] == 1) \
	    Screen [N] = (uint16) COLOR_SUB1_2 (gfx->ScreenColors [Pixel], \
						gfx->FixedColour); \
	else \
	    Screen [N] = gfx->ScreenColors [Pixel]; \
	Depth [N] = gfx->Z2; \
    }

    FN(0)
    FN(1)
    FN(2)
    FN(3)

#undef FN
}

void DrawTile16FixedAdd1_2 (uint32 Tile, uint32 Offset, uint32 StartLine,
			    uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    RENDER_TILE(WRITE_4PIXELS16_ADDF1_2, WRITE_4PIXELS16_FLIPPED_ADDF1_2, 4)
}

void DrawClippedTile16FixedAdd1_2 (uint32 Tile, uint32 Offset,
				   uint32 StartPixel, uint32 Width,
				   uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILE(WRITE_4PIXELS16_ADDF1_2, 
			WRITE_4PIXELS16_FLIPPED_ADDF1_2, 4)
}

void DrawTile16FixedSub1_2 (uint32 Tile, uint32 Offset, uint32 StartLine,
			    uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    RENDER_TILE(WRITE_4PIXELS16_SUBF1_2, WRITE_4PIXELS16_FLIPPED_SUBF1_2, 4)
}

void DrawClippedTile16FixedSub1_2 (uint32 Tile, uint32 Offset,
				   uint32 StartPixel, uint32 Width,
				   uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILE(WRITE_4PIXELS16_SUBF1_2, 
			WRITE_4PIXELS16_FLIPPED_SUBF1_2, 4)
}

void DrawLargePixel16Add (uint32 Tile, uint32 Offset,
			  uint32 StartPixel, uint32 Pixels,
			  uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE

    register uint16 *sp = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    register uint16 pixel;

#define LARGE_ADD_PIXEL(s, p) \
(Depth [z + gfx->DepthDelta] ? (Depth [z + gfx->DepthDelta] != 1 ? \
			       COLOR_ADD (p, *(s + gfx->Delta))    : \
			       COLOR_ADD (p, gfx->FixedColour)) \
			    : p)
			      
    RENDER_TILE_LARGE (gfx->ScreenColors [pixel], LARGE_ADD_PIXEL)
}

void DrawLargePixel16Add1_2 (uint32 Tile, uint32 Offset,
			     uint32 StartPixel, uint32 Pixels,
			     uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE

    register uint16 *sp = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    register uint16 pixel;

#define LARGE_ADD_PIXEL1_2(s, p) \
((uint16) (Depth [z + gfx->DepthDelta] ? (Depth [z + gfx->DepthDelta] != 1 ? \
			       COLOR_ADD1_2 (p, *(s + gfx->Delta))    : \
			       COLOR_ADD (p, gfx->FixedColour)) \
			    : p))
			      
    RENDER_TILE_LARGE (gfx->ScreenColors [pixel], LARGE_ADD_PIXEL1_2)
}

void DrawLargePixel16Sub (uint32 Tile, uint32 Offset,
			  uint32 StartPixel, uint32 Pixels,
			  uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE

    register uint16 *sp = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    register uint16 pixel;

#define LARGE_SUB_PIXEL(s, p) \
(Depth [z + gfx->DepthDelta] ? (Depth [z + gfx->DepthDelta] != 1 ? \
			       COLOR_SUB (p, *(s + gfx->Delta))    : \
			       COLOR_SUB (p, gfx->FixedColour)) \
			    : p)
			      
    RENDER_TILE_LARGE (gfx->ScreenColors [pixel], LARGE_SUB_PIXEL)
}

void DrawLargePixel16Sub1_2 (uint32 Tile, uint32 Offset,
			     uint32 StartPixel, uint32 Pixels,
			     uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE

    register uint16 *sp = (uint16 *) gfx->S + Offset;
    uint8  *Depth = gfx->ZBuffer + Offset;
    uint16 pixel;

#define LARGE_SUB_PIXEL1_2(s, p) \
(Depth [z + gfx->DepthDelta] ? (Depth [z + gfx->DepthDelta] != 1 ? \
			       COLOR_SUB1_2 (p, *(s + gfx->Delta))    : \
			       COLOR_SUB (p, gfx->FixedColour)) \
			    : p)
			      
    RENDER_TILE_LARGE (gfx->ScreenColors [pixel], LARGE_SUB_PIXEL1_2)
}

void DrawHiResTile16 (uint32 Tile, uint32 Offset, uint32 StartLine,
	         uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    RENDER_TILEHI(WRITE_4PIXELSHI16, WRITE_4PIXELSHI16_FLIPPED, 4)
}

void DrawHiResClippedTile16 (uint32 Tile, uint32 Offset,
			uint32 StartPixel, uint32 Width,
			uint32 StartLine, uint32 LineCount, struct SGFX * gfx)
{
    TILE_PREAMBLE
    register uint8 *bp;

    TILE_CLIP_PREAMBLE
    RENDER_CLIPPED_TILEHI(WRITE_4PIXELSHI16, WRITE_4PIXELSHI16_FLIPPED, 4)
}
