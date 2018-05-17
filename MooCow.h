#ifndef MOOCOW_H_INCLUDED

#define MOOCOW_H_INCLUDED

#include "Box.h"
#include "FileTypeBox.h"

#ifdef MOOCOW_H_STATIC_DEFINES

Box::registry_map_t Box::registry;
static BoxTypeRegistration<FileTypeBox> FileTypeBoxRegistration("ftyp");
#endif

#endif
