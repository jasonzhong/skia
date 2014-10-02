/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrLayerHoister_DEFINED
#define GrLayerHoister_DEFINED

#include "SkPicture.h"
#include "SkTDArray.h"

class GrAccelData;
struct GrCachedLayer;
class GrReplacements;
struct SkRect;

// This class collects the layer hoisting functionality in one place.
// For each picture rendering:
//  FindLayersToHoist should be called once to collect the required layers
//  DrawLayers should be called once to render them
//  UnlockLayers should be called once to allow the texture resources to be recycled
class GrLayerHoister {
public:
    struct HoistedLayer {
        const SkPicture* fPicture;
        GrCachedLayer*   fLayer;
        SkIPoint         fOffset;
        SkMatrix         fCTM;
    };

    /** Find the layers in 'topLevelPicture' that need hoisting. Note that the discovered
        layers can be inside nested sub-pictures.
        @param topLevelPicture The top-level picture that is about to be rendered
        @param query       The rectangle that is about to be drawn.
        @param atlased     Out parameter storing the layers that should be hoisted to the atlas
        @param nonAtlased  Out parameter storing the layers that should be hoisted stand alone
        @param recycled    Out parameter storing layers that need hoisting but not rendering
        @param layerCache The source of new layers
        Return true if any layers are suitable for hoisting; false otherwise
    */
    static bool FindLayersToHoist(const SkPicture* topLevelPicture,
                                  const SkRect& query,
                                  SkTDArray<HoistedLayer>* altased,
                                  SkTDArray<HoistedLayer>* nonAtlased,
                                  SkTDArray<HoistedLayer>* recycled,
                                  GrLayerCache* layerCache);

    /** Draw the specified layers into either the atlas or free floating textures.
        @param atlased      The layers to be drawn into the atlas
        @param nonAtlased   The layers to be drawn into their own textures
        @param recycled     Layers that don't need rendering but do need to go into the 
                            replacements object
        @param replacements The replacement structure to fill in with the rendered layer info
    */
    static void DrawLayers(const SkTDArray<HoistedLayer>& atlased,
                           const SkTDArray<HoistedLayer>& nonAtlased,
                           const SkTDArray<HoistedLayer>& recycled,
                           GrReplacements* replacements);

    /** Unlock unneeded layers in the layer cache.
        @param layerCache holder of the locked layers
        @param atlased    Unneeded layers in the atlas
        @param nonAtlased Unneeded layers in their own textures
        @param recycled   Unneeded layers that did not require rendering
    */
    static void UnlockLayers(GrLayerCache* layerCache,
                             const SkTDArray<HoistedLayer>& atlased,
                             const SkTDArray<HoistedLayer>& nonAtlased,
                             const SkTDArray<HoistedLayer>& recycled);
};

#endif