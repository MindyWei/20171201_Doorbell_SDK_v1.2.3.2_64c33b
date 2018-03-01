#ifndef SCENE_H
#define SCENE_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "ite/itu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MS_PER_FRAME                17              ///< Drawing delay per frame

/**
 * Initializes scene module.
 */
void SceneInit(void);

/**
 * Exits scene module.
 */
void SceneExit(void);

/**
 * Loads ITU file.
 */
void SceneLoad(void);

/**
 * Runs the main loop to receive events, update and draw scene.
 *
 * @return The quit value.
 */
int SceneRun(void);

/**
 * Global instance variable of scene.
 */
extern ITUScene theScene;

#ifdef __cplusplus
}
#endif

#endif /* SCENE_H */