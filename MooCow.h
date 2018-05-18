#ifndef MOOCOW_H_INCLUDED

#define MOOCOW_H_INCLUDED

#include "Box.h"
#include "FileTypeBox.h"
#include "MovieBox.h"

#ifdef MOOCOW_H_STATIC_DEFINES

Box::registry_map_t Box::registry;
static BoxTypeRegistration<FileTypeBox> FileTypeBoxRegistration("ftyp");
static BoxTypeRegistration<MovieBox> MovieBoxRegistration("moov");
static BoxTypeRegistration<MovieHeaderBox> MovieHeaderBoxRegistration("mvhd");

#endif

#endif
