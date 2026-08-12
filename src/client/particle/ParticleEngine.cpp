#include "ParticleEngine.h"
#include "Particle.h"
#include "TerrainParticle.h"
#include "../renderer/Textures.h"
#include "../renderer/OptimizationConfig.h"
#include "../../world/level/Level.h"
#include "../../NinecraftApp.h"

ParticleEngine::ParticleEngine(Level* level, Textures* textures)
:	level(level),
	textures(textures)
{
	textures->loadTexture("particles.png");
	for (int i = 0; i < TEXTURE_COUNT; ++i)
		particles[i].reserve(MCPE_MAX_PARTICLES_PER_TEXTURE);
}

ParticleEngine::~ParticleEngine() {
    clear();
}

void ParticleEngine::add(Particle* p) {
    int t = p->getParticleTexture();
    if (t < 0 || t >= TEXTURE_COUNT) {
        delete p;
        return;
    }

    ParticleList& list = particles[t];
    if (list.size() >= MCPE_MAX_PARTICLES_PER_TEXTURE) {
        // Drop the oldest particle instead of allowing unbounded growth on
        // low-memory devices. Particle order is not gameplay state.
        delete list.front();
        list.erase(list.begin());
    }
    list.push_back(p);
}

void ParticleEngine::tick() {
    for (int tt = 0; tt < TEXTURE_COUNT; tt++) {
        ParticleList& list = particles[tt];
        unsigned int i = 0;
        while (i < list.size()) {
            Particle* p = list[i];
            p->tick();
            if (p->removed) {
                // Particle order is irrelevant, so swap-with-last avoids the
                // O(n) tail shift caused by vector::erase in the hot loop.
                list[i] = list.back();
                list.pop_back();
                delete p;
            } else {
                ++i;
            }
        }
    }
}

void ParticleEngine::render(Entity* player, float a) {
    float xa = (float) Mth::cos(player->yRot * Mth::PI / 180);
    float za = (float) Mth::sin(player->yRot * Mth::PI / 180);

    float xa2 = -za * (float) Mth::sin(player->xRot * Mth::PI / 180);
    float za2 = xa * (float) Mth::sin(player->xRot * Mth::PI / 180);
    float ya = (float) Mth::cos(player->xRot * Mth::PI / 180);

    Particle::xOff = (player->xOld+(player->x-player->xOld)*a);
    Particle::yOff = (player->yOld+(player->y-player->yOld)*a);
    Particle::zOff = (player->zOld+(player->z-player->zOld)*a);
    for (int tt = 0; tt < 3; tt++) {
        if (particles[tt].empty()) continue;

        if (tt == MISC_TEXTURE)
            textures->loadAndBindTexture("particles.png");
        else if (tt == TERRAIN_TEXTURE)
            textures->loadAndBindTexture("terrain.png");
        else if (tt == ITEM_TEXTURE)
            textures->loadAndBindTexture("gui/items.png");

        Tesselator& t = Tesselator::instance;
        t.begin();
        const unsigned int size = particles[tt].size();
        for (unsigned int i = 0; i < size; ++i) {
            particles[tt][i]->render(t, a, xa, ya, za, xa2, za2);
        }
        t.draw();
    }

    renderLit(player, a);
}

void ParticleEngine::renderLit(Entity* player, float a) {
    int tt = ENTITY_PARTICLE_TEXTURE;
    ParticleList& pl = particles[tt];
    const int size = pl.size();
    if (size == 0) return;

    float xa = Mth::cos(player->yRot * Mth::DEGRAD);
    float za = Mth::sin(player->yRot * Mth::DEGRAD);

    float xs = Mth::sin(player->xRot * Mth::DEGRAD);
    float xa2 = -za * xs;
    float za2 = xa * xs;
    float ya = Mth::cos(player->xRot * Mth::DEGRAD);

    Tesselator& t = Tesselator::instance;
    for (int i = 0; i < size; i++) {
        Particle* p = pl[i];
        p->render(t, a, xa, ya, za, xa2, za2);
    }
}

void ParticleEngine::setLevel(Level* level) {
    this->level = level;
    clear();
}

void ParticleEngine::destroy(int x, int y, int z) {
    int tid = level->getTile(x, y, z);
    if (tid == 0) return;
    int data = level->getData(x, y, z);

    Tile* tile = Tile::tiles[tid];
    const int SD = 3;
    for (int xx = 0; xx < SD; xx++)
        for (int yy = 1; yy < SD; yy++)
            for (int zz = 0; zz < SD; zz++) {
                float xp = x + (xx + 0.5f) / SD;
                float yp = y + (yy + 0.5f) / SD;
                float zp = z + (zz + 0.5f) / SD;
                add((new TerrainParticle(level, xp, yp, zp, 2*(xp - x - 0.5f), 2*(yp - y - 0.5f), 2*(zp - z - 0.5f), tile, data))->init(x, y, z));
            }
}
