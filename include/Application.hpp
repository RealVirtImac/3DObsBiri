/***************************************************************************
									Application.hpp
                             --------------------
    begin                : Feb 1 2013
    copyright            : (C) 2013 by R. Bertozzi & S. Bougeois
    email                : romain.bertozzi@gmail.com s.bougeois@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/

//!  Creates the context of the program
/*!
  * This class contains the main skeleton of the program, from the initialization of the SDL context to its deletion.
  * \author R. Bertozzi & S. Bougeois
  * \brief Creates the context of the program
  * \file Application.hpp
*/

#pragma once

#ifdef _WIN32
	#define GLEW_STATIC
#endif

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_getenv.h>
#include <vector>

#include "Renderer.hpp"

/*!
 * \brief Contains the main skeleton of the program, from the initialization of the SDL context to its deletion.
 */ 
class Application
{
	public:
		//! Constructor
		Application();
		//! Destructor
		~Application();
		//! Main skeleton of the program.
		/*!
		 * \return 0
		*/
		int on_execute();
		//! Initializes the SDL context
		/*!
		 * 	\return True if everything was all right, False otherwise
		 */
		bool on_init();
		//! Manages all the user's events
		/*!
		 * 	\param Event the input event
		 */
		void on_event(SDL_Event* Event);
		//! Makes all the required calculations just before the render method
		void on_loop();
		//! Renders the context
		void on_render();
		//! Cleans the context
		void on_cleanup();
	
	private :
		//! Is false, the program closes
		bool m_running;
		
		SDL_Surface* m_display;
		
		Renderer* m_renderer;
		
		int m_mouse_x;
		int m_mouse_y;
		
		int m_width;
		int m_height;
		
		SDL_Joystick* m_joystick;
		
		std::vector<bool> m_input_keys;
		
		bool m_display_gui;
		
		bool m_has_focus_changed;
};
