/*
 * @(#)jp_co_ipride_License.c
 */

#include "jp_co_ipride_License.h"
#include "license.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32													
#include <windows.h>
#endif

const char *GetFilePath(JNIEnv* jenv, jstring file)
{
	const char* f;

#ifdef WIN32													
	const jchar* jcFile = (*jenv)->GetStringChars(jenv, file, NULL);	
	int jcFileLen = (*jenv)->GetStringLength(jenv, file);
	char *cFile = (char *)malloc(jcFileLen * 2 + 1);				
	memset(cFile, 0, jcFileLen * 2 + 1);
	WideCharToMultiByte(CP_ACP, 0, jcFile, jcFileLen, cFile, jcFileLen * 2 + 1, NULL, NULL);

	f = cFile;
	(*jenv)->ReleaseStringChars(jenv, file, jcFile);
#else															
	f = (*jenv)->GetStringUTFChars(jenv, file, NULL);
#endif

	return f;
}

JNIEXPORT jboolean JNICALL Java_jp_co_ipride_License_isValidFile(JNIEnv* jenv, jclass self, jstring file, jstring vers)
{
	int r;
    const char* v = (*jenv)->GetStringUTFChars(jenv, vers, NULL);
	const char* f = GetFilePath(jenv, file);

	r = license_is_valid_file(f, v);

#if WIN32
  free((void*)f);
#else
  (*jenv)->ReleaseStringUTFChars(jenv, file, f);
#endif

  (*jenv)->ReleaseStringUTFChars(jenv, vers, v);

  return r ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL Java_jp_co_ipride_License_getSubject
	(JNIEnv* jenv, jclass self, jstring file)
{
  const char* f = GetFilePath(jenv, file);

  char* subject = license_get_subject(f);
	jstring r = (*jenv)->NewStringUTF(jenv, subject);

#if WIN32
	free((void*)f);
#else
  (*jenv)->ReleaseStringUTFChars(jenv, file, f);
#endif
	free(subject);

	return r;
}

JNIEXPORT jstring JNICALL Java_jp_co_ipride_License_getAppVersion
	(JNIEnv* jenv, jclass self, jstring file)
{
  const char* f = GetFilePath(jenv, file);

  char* info = license_get_info(f);
	jstring r = (*jenv)->NewStringUTF(jenv, info);

#if WIN32
  free((void*)f);
#else
  (*jenv)->ReleaseStringUTFChars(jenv, file, f);
#endif
	free(info);

	return r;
}

JNIEXPORT jstring JNICALL Java_jp_co_ipride_License_getStartDate
	(JNIEnv* jenv, jclass self, jstring file)
{
  const char* f = GetFilePath(jenv, file);

  char* startDate = license_get_begin_date(f);
	jstring r = (*jenv)->NewStringUTF(jenv, startDate);

#if WIN32
  free((void*)f);
#else
  (*jenv)->ReleaseStringUTFChars(jenv, file, f);
#endif

  free(startDate);

	return r;
}

JNIEXPORT jstring JNICALL Java_jp_co_ipride_License_getEndDate
	(JNIEnv* jenv, jclass self, jstring file)
{
  const char* f = GetFilePath(jenv, file);

  char* endDate = license_get_end_date(f);
	jstring r = (*jenv)->NewStringUTF(jenv, endDate);

#if WIN32
  free((void*)f);
#else
  (*jenv)->ReleaseStringUTFChars(jenv, file, f);
#endif

  free(endDate);

	return r;
}

JNIEXPORT jbyteArray JNICALL Java_jp_co_ipride_License_getPublicKey
	(JNIEnv* jenv, jclass self, jstring file)
{
    const char* f = GetFilePath(jenv, file);

    char* r = license_get_public_key(f);
	int len;
	jstring jr;
	len = strlen(r);
	jr = (*jenv)->NewByteArray(jenv, len);
	(*jenv)->SetByteArrayRegion(jenv, jr, 0, len, (jbyte*) r);

#if WIN32
  free((void*)f);
#else
  (*jenv)->ReleaseStringUTFChars(jenv, file, f);
#endif

  free((void*)r);

	return jr;
}
/* the end of file */
