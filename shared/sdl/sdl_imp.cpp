/*
===========================================================================
Copyright (C) 2005 - 2015, ioquake3 contributors
Copyright (C) 2013 - 2015, OpenJK contributors
Copyright (C) 2023         Jorge Martinez

This file is part of the OpenJK source code.

OpenJK is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/


#include <SDL.h>
#include "qcommon/qcommon.h"

SDL_sem* 						renderCommandsEvent;
SDL_sem* 						renderActiveEvent;
SDL_mutex* 						renderMutex;
SDL_cond* 						renderCompletedCond;
DECLSPEC SDL_Thread *SDLCALL 	renderThreadHandle;
void 							(*glimpRenderThread)( void );
static	void					*smpData;
qboolean 						is_rendering;

int GLimp_RenderThreadWrapper( void* data ) {
	glimpRenderThread();
	return 0;
}

qboolean GLimp_SpawnRenderThread( void (*function)( void ) ) {
	renderActiveEvent = SDL_CreateSemaphore(0);
	renderCommandsEvent = SDL_CreateSemaphore(0);
	renderMutex = SDL_CreateMutex();
	renderCompletedCond = SDL_CreateCond();

	glimpRenderThread = function;

	renderThreadHandle = SDL_CreateThread(GLimp_RenderThreadWrapper, "renderer", NULL);

	if ( !renderThreadHandle ) {
		return qfalse;
	}

	return qtrue;
}

void *GLimp_RendererSleep( void ) {
	void	*data;

	// the front end can exit GLimp_FrontEndSleep
	SDL_LockMutex(renderMutex);
	is_rendering = qfalse;
	SDL_UnlockMutex(renderMutex);
	SDL_CondSignal(renderCompletedCond);

	SDL_SemWait(renderCommandsEvent);

	data = smpData;

	// after this, the main thread can exit GLimp_WakeRenderer
	SDL_SemPost(renderActiveEvent);

	SDL_LockMutex(renderMutex);
	is_rendering = qtrue;
	SDL_UnlockMutex(renderMutex);

	return data;
}

void GLimp_FrontEndSleep( void ) {
	SDL_LockMutex(renderMutex);
	while (is_rendering) {
		SDL_CondWait(renderCompletedCond, renderMutex);
	}
	SDL_UnlockMutex(renderMutex);
}

void GLimp_WakeRenderer( void *data ) {
	smpData = data;
	SDL_UnlockMutex(renderMutex);

	SDL_SemPost(renderCommandsEvent);
	SDL_SemWait(renderActiveEvent);
}