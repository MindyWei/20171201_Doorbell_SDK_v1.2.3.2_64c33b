/** @file
 * ITE DoorBell Indoor Modules.
 * @copyright ITE Tech. Inc. All Rights Reserved.
 */
/** @defgroup doorbell_indoor ITE DoorBell Indoor Modules
 *  @{
 */
#ifndef DOORBELL_H
#define DOORBELL_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes device information module.
 */
//Init leaf
void LeafInit(void); 

//play keysound
void LeafPlayKeySound(void);

//play soundfile
void LeafPlayfile(char* filename);

//stop soundfile play
void LeafStopSound(void);

//Treminate the call(video & audio)
void LeafTerminate(void);

//pause key play
void LinphonePauseKeySound(void);

//resume key play
void LinphoneResumeKeySound(void);

//start preview & ring before connect
void LeafStratRingPreview(void);

//start the call
void LeafStartConnect(void);

#ifdef __cplusplus
}
#endif

#endif /* DOORBELL_H */
/** @} */ // end of doorbell_indoor