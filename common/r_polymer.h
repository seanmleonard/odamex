// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id$
//
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 2006-2015 by The Odamex Team.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//
// Span rasterization data structures
//
//-----------------------------------------------------------------------------


#ifndef __R_POLYMER_H__
#define __R_POLYMER_H__

#include "r_intrin.h"
#include "r_defs.h"

//
// SpanRasterizer
//
// Rasterizes a span of texture-mapped pixels horizontally to the frame buffer.
//
template<typename SOURCE_PIXEL_T, typename DEST_PIXEL_T>
class SpanRasterizer
{
public:
	const SOURCE_PIXEL_T*	source;
	DEST_PIXEL_T*			destination;

	int						pitch_in_pixels;

	shaderef_t				colormap;

	int						y;
	int						x1;
	int						x2;

	// Unused for non-orthogonal surfaces
	dsfixed_t				xfrac;
	dsfixed_t				yfrac;
	dsfixed_t				xstep;
	dsfixed_t				ystep;

	float					iu;
	float					iv;
	float					id;
	float					iustep;
	float					ivstep;
	float					idstep;

	fixed_t					translevel;

	shaderef_t				slopelighting[MAXWIDTH];

	palindex_t				color;

	int						lerp_range;
	float					inv_lerp_range;


	SpanRasterizer() :
		lerp_range(SPANJUMP),
		inv_lerp_range(1.0f/lerp_range)
	{}


	SpanRasterizer(const drawspan_t& dspan) :
		lerp_range(SPANJUMP),
		inv_lerp_range(1.0f/lerp_range)
	{
		source = (SOURCE_PIXEL_T*)dspan.source;
		destination = (DEST_PIXEL_T*)dspan.destination;
		y = dspan.y;
		x1 = dspan.x1;
		x2 = dspan.x2;
		iu = dspan.iu;
		iv = dspan.iv;
		id = dspan.id;
		iustep = dspan.iustep;
		ivstep = dspan.ivstep;
		idstep = dspan.idstep;
		pitch_in_pixels = dspan.pitch_in_pixels;
		colormap = dspan.colormap;

		// Unused for non-orthogonal surfaces
		xfrac = dspan.xfrac;
		yfrac = dspan.yfrac;
		xstep = dspan.xstep;
		ystep = dspan.ystep;

		translevel = dspan.translevel;
		color = dspan.color;

		memcpy(slopelighting, dspan.slopelighting, sizeof(shaderef_t) * (x2 - x1 + 1));
	}


	//
	// SpanRasterizer::calculateSpanDestination
	//
	// Determines the location in the destination frame buffer where the span
	// will begin.
	//
	DEST_PIXEL_T* calculateSpanDestination() const
	{
		return (DEST_PIXEL_T*)destination + y * pitch_in_pixels + x1;
	}


	//
	// SpanRasterizer::rasterize
	//
	// Texture maps a non-orthogonal surface using affine texture-mapping
	// for each row of the span.  Not as pretty as a perfect texture-mapping
	// but should be much faster.
	//
	// Based on R_DrawSlope_8_64 from Eternity Engine, written by SoM/Quasar
	//
	// The data type of the destination pixels and a color-remapping functor
	// are passed as template parameters.
	//
	template<typename COLORFUNC>
	void rasterize() const
	{
		#ifdef RANGECHECK
		if (x2 < x1 || x1 < 0 || x2 >= viewwidth || y >= viewheight || y < 0)
		{
			Printf(PRINT_HIGH, "SpanRasterizer::rasterize: %i to %i at %i", x1, x2, y);
			return;
		}
		#endif

		int count = x2 - x1 + 1;
		if (count <= 0)
			return;

		const SOURCE_PIXEL_T* src = source;
		DEST_PIXEL_T* dest = calculateSpanDestination();
		COLORFUNC colorfunc(*this);
	
		float _iu = iu, _iv = iv;
		const float _ius = iustep, _ivs = ivstep;
		float _id = id, _ids = idstep;
		float d = 65536.0f / _id;

		while (count >= lerp_range)
		{
			const float ustart = _iu * d;
			const float vstart = _iv * d;

			_id += _ids * lerp_range;
			d = 65536.0f / _id;

			_iu += _ius * lerp_range;
			_iv += _ivs * lerp_range;

			const float uend = _iu * d;
			const float vend = _iv * d;

			fixed_t ufrac = (fixed_t)ustart;
			fixed_t vfrac = (fixed_t)vstart;
			fixed_t ustep = (fixed_t)((uend - ustart) * inv_lerp_range);
			fixed_t vstep = (fixed_t)((vend - vstart) * inv_lerp_range);

			int incount = lerp_range;
			while (incount--)
			{
				const int spot = ((vfrac >> 10) & 0xFC0) | ((ufrac >> 16) & 63);
				colorfunc(src[spot], dest);
				dest++;
				ufrac += ustep;
				vfrac += vstep;
			}

			count -= lerp_range;
		}

		if (count > 0)
		{
			const float ustart = _iu * d;
			const float vstart = _iv * d;

			_id += _ids * count;
			d = 65536.0f / _id;

			_iu += _ius * count;
			_iv += _ivs * count;

			const float uend = _iu * d;
			const float vend = _iv * d;

			fixed_t ufrac = (fixed_t)ustart;
			fixed_t vfrac = (fixed_t)vstart;
			fixed_t ustep = (fixed_t)((uend - ustart) / count);
			fixed_t vstep = (fixed_t)((vend - vstart) / count);

			int incount = count;
			while (incount--)
			{
				const int spot = ((vfrac >> 10) & 0xFC0) | ((ufrac >> 16) & 63);
				colorfunc(src[spot], dest);
				dest++;
				ufrac += ustep;
				vfrac += vstep;
			}
		}
	}
};


class DirectSlopeColormapFunc
{
public:
	DirectSlopeColormapFunc(const SpanRasterizer<palindex_t, argb_t>& rasterizer) :
			colormap(rasterizer.slopelighting) { }

	forceinline void operator()(byte c, argb_t* dest)
	{
		*dest = colormap->shade(c);
		colormap++;
	}

private:
	const shaderef_t* colormap;
};


#endif // __R_POLYMER_H__
