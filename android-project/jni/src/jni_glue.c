#include <jni.h>
#include <android/log.h>
#include <unistd.h>

#include <SDL.h>
#include <libtcod.h>

/* Change these. */
#define LOG_TAG "SDL"
#define PKGNAME_FUNCTION org_libsdl_app
#define PKGNAME_CLASS "org.libsdl.app"
#undef USING_CUSTOM_PKGNAME
/* Use of # or ## bypasses macro expansion, need a layer of indirection to make it work. */
#define CONCAT_(A,B,C,D) A##B##C##D
#define PACKAGE_FNAME(PKGPART,FUNCPART) CONCAT_(Java_,PKGPART,_SDLActivity_,FUNCPART)

static JNIEnv *appenv;
static jclass appClass;


static void show_alert_dialog(JNIEnv *env, jclass cls, jobject obj, const char *msg) {
	jmethodID showAlertDialogMID;
	jstring msgTxt = (*env)->NewStringUTF(env, msg);
	jstring neutralTxt = (*env)->NewStringUTF(env, "Quit");
	showAlertDialogMID = (*env)->GetStaticMethodID(env, cls, "showAlertDialog", "(Ljava/lang/String;Ljava/lang/String;)V");
	(*env)->CallStaticVoidMethod(env, cls, showAlertDialogMID, msgTxt, neutralTxt);
	(*env)->DeleteLocalRef(env, msgTxt);
	(*env)->DeleteLocalRef(env, neutralTxt);
}

static int file_access_init(JNIEnv *env, jclass cls, jobject obj) {
	char filePath[] = "/Android/data/" PKGNAME_CLASS "/files";
	jclass clazz = (*env)->FindClass(env,"android/os/Environment");
	jmethodID getExternalStorageDirectoryMID = (*env)->GetStaticMethodID(env,clazz, "getExternalStorageDirectory",  "()Ljava/io/File;");
	jobject dirFileObject = (*env)->CallStaticObjectMethod(env,clazz, getExternalStorageDirectoryMID);
	jclass fileClazz = (*env)->FindClass(env,"java/io/File");
	jmethodID fileConstructorMID = (*env)->GetMethodID(env,fileClazz, "<init>", "(Ljava/io/File;Ljava/lang/String;)V");
	jmethodID mkdirsMID = (*env)->GetMethodID(env,fileClazz, "mkdirs", "()Z");
	jmethodID existsMID = (*env)->GetMethodID(env,fileClazz, "exists", "()Z");
	jmethodID getPathMID = (*env)->GetMethodID(env,fileClazz, "getPath", "()Ljava/lang/String;");
	jstring filesPathObject = (*env)->NewStringUTF(env,filePath); /* DeleteLocalRef */
	jobject appDirFileObject = (*env)->NewObject(env,fileClazz, fileConstructorMID, dirFileObject, filesPathObject);
	jstring pathObject = (jstring)(*env)->CallObjectMethod(env,appDirFileObject, getPathMID);
	const char* path = (*env)->GetStringUTFChars(env,pathObject, NULL);
	jboolean mkdirsRet, existsRet;
	/* We should be able to create the external data storage directory. */
	existsRet = (*env)->CallBooleanMethod(env,appDirFileObject, existsMID);
	if (!existsRet) {
		mkdirsRet = (*env)->CallBooleanMethod(env,appDirFileObject, mkdirsMID);
		if (!mkdirsRet) {
			show_alert_dialog(env, cls, obj, "Unable to create sdcard directory");
			return 0;
		}
		__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "mkdir(\"%s\") successful", path);
	}
	if (chdir(path) == -1) {
		show_alert_dialog(env, cls, obj, "Unable to access sdcard directory");
		return 0;
	}
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "chdir(\"%s\") successful", path);
	(*env)->ReleaseStringUTFChars(env,pathObject, path);
	(*env)->DeleteLocalRef(env,filesPathObject);
	return 1;
}

#ifdef USING_CUSTOM_PKGNAME
void Java_org_libsdl_app_SDLActivity_nativeRunAudioThread(JNIEnv* env, jclass cls);
void Java_org_libsdl_app_SDLActivity_onNativeResize(
								JNIEnv* env, jclass jcls,
								jint width, jint height, jint format);
void Java_org_libsdl_app_SDLActivity_onNativeKeyDown(
								JNIEnv* env, jclass jcls, jint keycode);
void Java_org_libsdl_app_SDLActivity_onNativeKeyUp(
								JNIEnv* env, jclass jcls, jint keycode);
void Java_org_libsdl_app_SDLActivity_onNativeTouch(
								JNIEnv* env, jclass jcls,
								jint touch_device_id_in, jint pointer_finger_id_in,
								jint action, jfloat x, jfloat y, jfloat p);
void Java_org_libsdl_app_SDLActivity_onNativeAccel(
								JNIEnv* env, jclass jcls,
								jfloat x, jfloat y, jfloat z);
void Java_org_libsdl_app_SDLActivity_nativePause(
								JNIEnv* env, jclass cls);
void Java_org_libsdl_app_SDLActivity_nativeResume(
								JNIEnv* env, jclass cls);
void Java_org_libsdl_app_SDLActivity_nativeQuit(
								JNIEnv* env, jclass cls);
void Java_org_libsdl_app_SDLActivity_nativeRunAudioThread(
								JNIEnv* env, jclass cls);
void Java_org_libsdl_app_SDLActivity_nativeInit(JNIEnv* env, jclass cls, jobject obj);

// Resize
void PACKAGE_FNAME(PKGNAME_FUNCTION, onNativeResize)(
                                    JNIEnv* env, jclass jcls,
                                    jint width, jint height, jint format)
{
    Java_org_libsdl_app_SDLActivity_onNativeResize(env, jcls, width, height, format);
}

// Keydown
void PACKAGE_FNAME(PKGNAME_FUNCTION, onNativeKeyDown)(
                                    JNIEnv* env, jclass jcls, jint keycode)
{
    Java_org_libsdl_app_SDLActivity_onNativeKeyDown(env, jcls, keycode);
}

// Keyup
void PACKAGE_FNAME(PKGNAME_FUNCTION, onNativeKeyUp)(
                                    JNIEnv* env, jclass jcls, jint keycode)
{
    Java_org_libsdl_app_SDLActivity_onNativeKeyUp(env, jcls, keycode);
}

// Touch
void PACKAGE_FNAME(PKGNAME_FUNCTION, onNativeTouch)(
                                    JNIEnv* env, jclass jcls,
                                    jint touch_device_id_in, jint pointer_finger_id_in,
                                    jint action, jfloat x, jfloat y, jfloat p)
{
    Java_org_libsdl_app_SDLActivity_onNativeTouch(env, jcls, touch_device_id_in, pointer_finger_id_in, action, x, y, p);
}

// Accelerometer
void PACKAGE_FNAME(PKGNAME_FUNCTION, onNativeAccel)(
                                    JNIEnv* env, jclass jcls,
                                    jfloat x, jfloat y, jfloat z)
{
     Java_org_libsdl_app_SDLActivity_onNativeAccel(env, jcls, x, y, z);
}

// Pause
void PACKAGE_FNAME(PKGNAME_FUNCTION, nativePause)(
                                    JNIEnv* env, jclass cls)
{
    Java_org_libsdl_app_SDLActivity_nativePause(env, cls);
}

// Resume
void PACKAGE_FNAME(PKGNAME_FUNCTION, nativeResume)(
                                    JNIEnv* env, jclass cls)
{
    Java_org_libsdl_app_SDLActivity_nativeResume(env, cls);
}

// Quit
void PACKAGE_FNAME(PKGNAME_FUNCTION, nativeQuit)(
                                    JNIEnv* env, jclass cls)
{
    Java_org_libsdl_app_SDLActivity_nativeQuit(env, cls);
}

void PACKAGE_FNAME(PKGNAME_FUNCTION, nativeRunAudioThread)(
                                    JNIEnv* env, jclass cls)
{
    Java_org_libsdl_app_SDLActivity_nativeRunAudioThread(env, cls);
}

void PACKAGE_FNAME(PKGNAME_FUNCTION,nativeInit)(JNIEnv* env, jclass cls, jobject obj) {
	appenv = env;
	appClass = cls;
    if (!file_access_init(env, cls, obj))
    	return;

    Java_org_libsdl_app_SDLActivity_nativeInit(env, cls, obj);
}
#endif

void PACKAGE_FNAME(PKGNAME_FUNCTION,nativeTCODScaling)(JNIEnv *env, jclass cls, jint scaling) {
	TCOD_sys_set_scaling(scaling);
}

void PACKAGE_FNAME(PKGNAME_FUNCTION,nativeTCODSetFPS)(JNIEnv *env, jclass cls, jint value) {
	TCOD_sys_set_fps(value);
}

void PACKAGE_FNAME(PKGNAME_FUNCTION,nativeSDLRecordGesture)(JNIEnv *env, jclass cls) {
	if (SDL_RecordGesture(-1))
		__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "SDL_RecordGesture succeeded");
	else
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "SDL_RecordGesture failed");
}

void PACKAGE_FNAME(PKGNAME_FUNCTION,nativeSDLSaveGestures)(JNIEnv *env, jclass cls) {
	int cnt;
	FILE *sFile = fopen("gestures.bin", "wb");
	cnt = SDL_SaveAllDollarTemplates(sFile);
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "SDL_SaveAllDollarTemplates saved %d templates", cnt);
	fclose(sFile);
}

void PACKAGE_FNAME(PKGNAME_FUNCTION,nativeSDLLoadGestures)(JNIEnv *env, jclass cls) {
	int cnt;
	FILE *sFile = fopen("gestures.bin", "rb");
	cnt = SDL_LoadDollarTemplates(-1, sFile);
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "SDL_LoadDollarTemplates loaded %d templates", cnt);
	fclose(sFile);
}
