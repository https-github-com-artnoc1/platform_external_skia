
/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrGpuResourceCacheAccess_DEFINED
#define GrGpuResourceCacheAccess_DEFINED

#include "GrGpuResource.h"
#include "GrGpuResourcePriv.h"

namespace skiatest {
    class Reporter;
}

/**
 * This class allows GrResourceCache increased privileged access to GrGpuResource objects.
 */
class GrGpuResource::CacheAccess {
private:
    /**
     * Is the resource currently cached as scratch? This means it is cached, has a valid scratch
     * key, and does not have a content key.
     */
    bool isScratch() const {
        return !fResource->getContentKey().isValid() && fResource->fScratchKey.isValid() &&
                fResource->resourcePriv().isBudgeted();
    }

    /**
     * Is the resource object wrapping an externally allocated GPU resource?
     */
    bool isWrapped() const { return GrGpuResource::kWrapped_LifeCycle == fResource->fLifeCycle; }
 
    /**
     * Called by the cache to delete the resource under normal circumstances.
     */
    void release() {
        fResource->release();
        if (fResource->isPurgeable()) {            
            SkDELETE(fResource);
        }
    }

    /**
     * Called by the cache to delete the resource when the backend 3D context is no longer valid.
     */
    void abandon() {
        fResource->abandon();
        if (fResource->isPurgeable()) {            
            SkDELETE(fResource);
        }
    }

    uint32_t timestamp() const { return fResource->fTimestamp; }
    void setTimestamp(uint32_t ts) { fResource->fTimestamp = ts; }

    int* accessCacheIndex() const { return &fResource->fCacheArrayIndex; }

    CacheAccess(GrGpuResource* resource) : fResource(resource) {}
    CacheAccess(const CacheAccess& that) : fResource(that.fResource) {}
    CacheAccess& operator=(const CacheAccess&); // unimpl

    // No taking addresses of this type.
    const CacheAccess* operator&() const;
    CacheAccess* operator&();

    GrGpuResource* fResource;

    friend class GrGpuResource; // to construct/copy this type.
    friend class GrResourceCache; // to use this type
    friend void test_unbudgeted_to_scratch(skiatest::Reporter* reporter); // for unit testing
};

inline GrGpuResource::CacheAccess GrGpuResource::cacheAccess() { return CacheAccess(this); }

inline const GrGpuResource::CacheAccess GrGpuResource::cacheAccess() const {
    return CacheAccess(const_cast<GrGpuResource*>(this));
}

#endif
