/** @file
 * ITE DoorBell Outdoor Scene Definition.
 *
 * @author Jim Tan
 * @version 1.0
 * @date 2013
 * @copyright ITE Tech. Inc. All Rights Reserved.
 */
/** @defgroup doorbell_outdoor ITE DoorBell Outdoor Modules
 *  @{
 */
#ifndef SCENE_H
#define SCENE_H

#include "doorbell.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup doorbell_outdoor_scene Scene
 *  @{
 */
/**
 * Initializes scene module.
 */
void SceneInit(void);

/**
 * Exits scene module.
 */
void SceneExit(void);

/**
 * Runs the main loop to receive events, update and draw scene.
 *
 * @return The QuitValue.
 */
int SceneRun(void);

/**
 * Quits the scene.
 *
 * @param value The reason to quit the scene.
 */
void SceneQuit(QuitValue value);

/**
 * Gets the current quit value.
 *
 * @return The current quit value.
 */
QuitValue SceneGetQuitValue(void);

/**
 * Is the call connected or not.
 *
 * @return true for connected, false for not connected yet.
 */
bool SceneIsCallConnected(void);

/**
 * Sets the IP address of remote side.
 *
 * @param the IP address of remote side.
 */
void SceneSetRemoteIP(char* ip);

// calling
/**
 * Makes a phone call to the specified address.
 *
 * @param num The floor and room code for .
 * @return 0 for success, otherwise for fail.
 */
int CallingCall(char* num);

/**
 * Check if dial time is reached.
 *
 * @return true for success, otherwise for fail.
 */
bool CallingOnTimer(void);

/**
 * Reset the calling ,callend flag.
 *
 */
void SceneFlagReset(void);

/**
 * Reset the calling ,callend flag.
 * for polling call
 */
void sceneFlagdail(void);
/** @} */ // end of doorbell_outdoor_scene

#ifdef __cplusplus
}
#endif

#endif /* SCENE_H */
/** @} */ // end of doorbell_outdoor