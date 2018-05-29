#ifndef MOOCOW_H_INCLUDED

#define MOOCOW_H_INCLUDED

#include "Box.h"
#include "FileTypeBox.h"
#include "MovieBox.h"
#include "UserDataBox.h"
#include "TrackBox.h"
#include "MediaBox.h"
#include "MediaInformationBox.h"
#include "SampleTableBox.h"
#include "MetaBox.h"
#include "HandlerBox.h"

#ifdef MOOCOW_H_STATIC_DEFINES

Box::registry_map_t Box::registry;
static BoxTypeRegistration<FileTypeBox> FileTypeBoxRegistration("ftyp");
static BoxTypeRegistration<MovieBox> MovieBoxRegistration("moov");
static BoxTypeRegistration<MovieHeaderBox> MovieHeaderBoxRegistration("mvhd");
static BoxTypeRegistration<UserDataBox> UserDataBoxRegistration("udta");
static UserDataStringBoxRegistry UserDataStringBoxRegistration("\xC2\xA9xyz", nullptr);
static BoxTypeRegistration<TrackBox> TrackBoxRegistration("trak");
static BoxTypeRegistration<MediaBox> MediaBoxRegistration("mdia");
static BoxTypeRegistration<MediaInformationBox> MediaInformationBoxRegistration("minf");
static BoxTypeRegistration<SampleTableBox> SampleTableBoxRegistration("stbl");
static BoxTypeRegistration<BitRateBox> BitRateBoxRegistration("btrt");
static BoxTypeRegistration<SampleDescriptionBox> SampleDescriptionBoxRegistration("stsd");
static BoxTypeRegistration<TimeToSampleBox> TimeToSampleBoxRegistration("stts");
static BoxTypeRegistration<MetaBox> MetaBoxRegistration("meta");
static BoxTypeRegistration<HandlerBox> HandlerBoxRegistration("hdlr");


#endif

#endif
