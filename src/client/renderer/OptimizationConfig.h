#ifndef MCPE_OPTIMIZATION_CONFIG_H
#define MCPE_OPTIMIZATION_CONFIG_H

// Aggressive defaults for the 0.6.1 renderer. Potato Mode can be changed
// at runtime from Graphics settings.
#ifndef MCPE_POTATO_MODE
#define MCPE_POTATO_MODE 1
#endif

extern bool g_mcpePotatoMode;

#define MCPE_MAX_RENDER_DISTANCE 192
#define MCPE_MAX_ENTITY_RENDER_DISTANCE_SQ (96.0f * 96.0f)
#define MCPE_DIRTY_SCAN_PER_FRAME 4
#define MCPE_NEAR_CHUNK_REBUILDS_PER_FRAME 1
#define MCPE_FAR_CHUNK_REBUILDS_PER_FRAME 1
#define MCPE_CHUNK_REBUILD_BUDGET_MS 3.0f

static inline unsigned int MCPE_getMaxParticlesPerTexture() {
    return g_mcpePotatoMode ? 128u : 2048u;
}

#endif