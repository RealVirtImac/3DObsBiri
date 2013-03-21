/***************************************************************************
									Framebuffer.hpp
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

//!  Framebuffers
/*!
  * \author R. Bertozzi & S. Bougeois
  * \brief Contains one or several textures of rendered images
  * \file Framebuffer.hpp
*/

#pragma once

#ifdef _WIN32
	#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <iostream>
#include <vector>

/*!
 * \brief Contains one or several textures of rendered images
 */ 
class Framebuffer
{
	public :
		//! Constructor
		/*!
		 * Initializes several elements such as the number of color textures and the width / height of the framebuffer
		 * \param number_of_color_textures Number of color textures
		 * \param width Width of the window
		 * \param height Height of the window
		 */ 
		Framebuffer(const int number_of_color_textures, const unsigned int width, const unsigned int weight);
		//! Destuctor
		~Framebuffer();
		
		//! Gets the framebuffer id
		/*!
		 * \return The id of the framebuffer
		 */ 
		GLuint get_framebuffer_id() const;
		
		//! Gets the array containing all the textures ids
		/*!
		 * \return The array containing all the textures ids
		 */ 
		GLuint* get_texture_color_id() const;
		
		unsigned int get_number_of_color_textures() const;
		
		GLenum* get_draw_buffers() const;
		
	private :
		GLuint m_framebuffer_id;
		GLuint* m_texture_color_id;
		GLuint m_depth_texture_id;
		GLenum* m_draw_buffers;
		
		unsigned int m_number_of_color_textures;
		unsigned int m_width;
		unsigned int m_height;
};
