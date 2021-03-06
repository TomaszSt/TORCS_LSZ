/***************************************************************************

    file                 : myrobot.cpp
    created              : Wed Jan 8 18:31:16 CET 2003
    copyright            : (C) 2002-2004 Bernhard Wymann
    email                : berniw@bluewin.ch
    version              : $Id: myrobot.cpp,v 1.5.2.2 2008/11/09 17:50:19 berniw Exp $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <tgf.h>
#include <track.h>
#include <car.h>
#include <raceman.h>
#include <robottools.h>
#include <robot.h>

#include "driver.h"

#define NBBOTS 10

static const char* botname[NBBOTS] = {
	"myrobot 1", "myrobot 2", "myrobot 3", "myrobot 4", "myrobot 5",
	"myrobot 6", "myrobot 7", "myrobot 8", "myrobot 9", "myrobot 10"
};

static const char* botdesc[NBBOTS] = {
	"myrobot 1", "myrobot 2", "myrobot 3", "myrobot 4", "myrobot 5",
	"myrobot 6", "myrobot 7", "myrobot 8", "myrobot 9", "myrobot 10"
};

static Driver *driver[NBBOTS];

static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s);
static void newRace(int index, tCarElt* car, tSituation *s);
static void drive(int index, tCarElt* car, tSituation *s);
static int pitcmd(int index, tCarElt* car, tSituation *s);
static void shutdown(int index);
static int InitFuncPt(int index, void *pt);
static void endRace(int index, tCarElt *car, tSituation *s);


// Module entry point.
extern "C" int myrobot(tModInfo *modInfo)
{
	int i;
	
	// Clear all structures.
	memset(modInfo, 0, 10*sizeof(tModInfo));

	for (i = 0; i < NBBOTS; i++) {
		modInfo[i].name    = strdup(botname[i]);	// name of the module (short).
		modInfo[i].desc    = strdup(botdesc[i]);	// Description of the module (can be long).
		modInfo[i].fctInit = InitFuncPt;			// Init function.
		modInfo[i].gfId    = ROB_IDENT;				// Supported framework version.
		modInfo[i].index   = i;						// Indices from 0 to 9.
	}
	return 0;
}


// Module interface initialization.
static int InitFuncPt(int index, void *pt)
{
	tRobotItf *itf = (tRobotItf *)pt;

	// Create robot instance for index.
	driver[index] = new Driver(index);
	itf->rbNewTrack = initTrack;	// Give the robot the track view called.
	itf->rbNewRace  = newRace;		// Start a new race.
	itf->rbDrive    = drive;		// Drive during race.
	itf->rbPitCmd   = pitcmd;		// Pit commands.
	itf->rbEndRace  = endRace;		// End of the current race.
	itf->rbShutdown = shutdown;		// Called before the module is unloaded.
	itf->index      = index;		// Index used if multiple interfaces.
	return 0;
}


// Called for every track change or new race.
static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s)
{
	driver[index]->initTrack(track, carHandle, carParmHandle, s);
}


// Start a new race.
static void newRace(int index, tCarElt* car, tSituation *s)
{
	driver[index]->newRace(car, s);
}


// Drive during race.
static void drive(int index, tCarElt* car, tSituation *s)
{
	driver[index]->drive(s);
	//memset(&car->ctrl, 0, sizeof(tCarCtrl));

 //   float angle;
 //   const float SC = 1.0;

 //   angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
 //   NORM_PI_PI(angle); // put the angle back in the range from -PI to PI
 //   angle -= SC*car->_trkPos.toMiddle/car->_trkPos.seg->width;

 //   // set up the values to return
 //   car->ctrl.steer = angle / car->_steerLock;
 //   
 //   car->ctrl.accelCmd = 0.3; // 30% accelerator pedal
 //   car->ctrl.brakeCmd = 0.0; // no brakes
	//printf("gear: %d\n", car->priv.gear);
	//if (car->priv.gear == 2) {
	//	car->ctrl.gear = 2;
	//} else if (car->priv.gear == 1 && car->priv.enginerpm / car->priv.enginerpmMax > 0.7) {
	//	car->ctrl.gear = 2; // second gear
	//} else if (car->priv.gear == 0 || car->priv.gear == 1) {
	//	car->ctrl.gear = 1; // first gear
	//}
}


// Pitstop callback.
static int pitcmd(int index, tCarElt* car, tSituation *s)
{
	return driver[index]->pitCommand(s);
}


// End of the current race.
static void endRace(int index, tCarElt *car, tSituation *s)
{
	driver[index]->endRace(s);
}


// Called before the module is unloaded.
static void shutdown(int index)
{
	delete driver[index];
}

