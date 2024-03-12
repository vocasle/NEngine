#pragma once

#include "defines.h"
#include "nemath.h"

struct NE_Ari;
struct NE_CommandList;

NE_API struct NE_Ari *ne_ari_create(void);
NE_API void ne_ari_destroy(struct NE_Ari *ari);

NE_API void ne_ari_clear(struct NE_Ari *ari, const struct NE_Vec4 *color);


