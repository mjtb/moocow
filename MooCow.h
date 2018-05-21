#ifndef MOOCOW_H_INCLUDED

#define MOOCOW_H_INCLUDED

#include "Box.h"
#include "FileTypeBox.h"
#include "MovieBox.h"
#include "UserDataBox.h"

#ifdef MOOCOW_H_STATIC_DEFINES

Box::registry_map_t Box::registry;
static BoxTypeRegistration<FileTypeBox> FileTypeBoxRegistration("ftyp");
static BoxTypeRegistration<MovieBox> MovieBoxRegistration("moov");
static BoxTypeRegistration<MovieHeaderBox> MovieHeaderBoxRegistration("mvhd");
static BoxTypeRegistration<UserDataBox> UserDataBoxRegistration("udta");
static UserDataStringBoxRegistry UserDataStringBoxRegistration("\xC2\xA9xyz", nullptr);

#endif

#endif
