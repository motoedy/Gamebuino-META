#ifndef FOLDER_NAME
#ifndef __SKETCH_NAME__
#define FOLDER_NAME "testsketch"
#endif
#endif

///////////////
// save defines
///////////////

#ifndef SAVEBLOCK_NUM
#define SAVEBLOCK_NUM 64
#endif

#ifndef SAVEFILE_NAME
#define SAVEFILE_NAME "SAVE.SAV"
#endif

#ifndef SAVECONF_DEFAULT_BLOBSIZE
#define SAVECONF_DEFAULT_BLOBSIZE 32
#endif

#ifndef AUTOSHOW_TITLESCREEN
#define AUTOSHOW_TITLESCREEN 1
#endif

///////////////
// display defines
///////////////

#ifndef DISPLAY_MODE
#define DISPLAY_MODE DISPLAY_MODE_RGB565
#endif

#ifndef DISPLAY_DEFAULT_BACKGROUND_COLOR
#define DISPLAY_DEFAULT_BACKGROUND_COLOR Color::black
#endif

#ifndef DISPLAY_DEFAULT_COLOR
#define DISPLAY_DEFAULT_COLOR Color::white
#endif

#ifndef DEFAULT_FONT_SIZE
#if DISPLAY_MODE == DISPLAY_MODE_INDEX
#define DEFAULT_FONT_SIZE 2
#else
#define DEFAULT_FONT_SIZE 1
#endif
#endif

///////////////
// sound defines
///////////////

#ifndef SOUND_CHANNELS
#define SOUND_CHANNELS 4
#endif

#ifndef SOUND_FREQ
#define SOUND_FREQ 44100
#endif

#ifndef SOUND_BUFFERSIZE
#define SOUND_BUFFERSIZE (2048 * SOUND_FREQ / 44100)
#endif

#ifndef SOUND_ENABLE_FX
#define SOUND_ENABLE_FX 1
#endif

///////////////
// image defines
///////////////

#ifndef STRICT_IMAGES
#define STRICT_IMAGES 0
#endif

#ifndef MAX_IMAGE_RECORDING
#define MAX_IMAGE_RECORDING 2
#endif

///////////////
// language defines
///////////////

#ifndef LANGUAGE_DEFAULT_SIZE
#define LANGUAGE_DEFAULT_SIZE 0
#endif

///////////////
// Fault Handler defines
///////////////

#ifndef HARDFAULT_DEBUG_HANDLER
#define HARDFAULT_DEBUG_HANDLER 0
#endif

///////////////
// GUI defines
///////////////

#ifndef GUI_ENABLE_POPUP
#define GUI_ENABLE_POPUP 1
#endif

#ifndef HOME_MENU_NO_EXIT
#define HOME_MENU_NO_EXIT 0
#endif
