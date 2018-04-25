/** @file
 * Template scene definition.
 *
 */
/** @defgroup template
 *  @{
 */
#ifndef SCENE_H
#define SCENE_H

#include "ite/itu.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup template_scene
 *  @{
 */

#define MS_PER_FRAME    17  ///< Drawing delay per frame

#define SCENE_MSG_CMD_QUEUE_ENABLE //2018.3.28 my.wei add for UI command queue
/**
 * Global instance variable of scene.
 */
extern ITUScene theScene;

// scene
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

//2018.3.28 my.wei add for UI command queue -
#if defined(SCENE_MSG_CMD_QUEUE_ENABLE)
/**
 * Scene Goto a Layer.
 */
void SceneGotoLayer(char *layerName);

/**
 * Scene Widget Set Visible.
 */
void SceneWidgetSetVisible(char *widgetName, bool visible);

/**
 * Scene Widget Set Enable.
 */
void SceneWidgetSetEnable(char *widgetName, bool enable);

/**
 * Scene Event Handler.
 */
void SceneEventHandler(void);

/**
 * Meida Evnet Handler UI.
 */
void media_event_handler_ui(void);
#endif
//2018.3.28 my.wei add for UI command queue +


/**
 * Runs the main loop to receive events, update and draw scene.
 *
 * @return The QuitValue.
 */
int SceneRun(void);

/** @} */ // end of template_scene

#ifdef __cplusplus
}
#endif

#endif /* SCENE_H */

/** @} */ // end of template
