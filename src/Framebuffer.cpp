/***************************************************************************
									Framebuffer.cpp
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

/*!
 * \file Framebuffer.cpp
 * \brief A framebuffer class
 * \author R. Bertozzi & S. Bougeois 
 */

#include "../include/Framebuffer.hpp"

Framebuffer::Framebuffer(const int number_of_color_textures, const unsigned int width, const unsigned int height):
	m_number_of_color_textures(number_of_color_textures),
	m_width(width),
	m_height(height)
{
	//~ Generating arrays
	m_texture_color_id = new GLuint[m_number_of_color_textures];
	m_draw_buffers = new GLenum[m_number_of_color_textures];
	
	//~ Generating textures ids
	glGenTextures(m_number_of_color_textures, m_texture_color_id);
	glGenTextures(1, &m_depth_texture_id);
	
	//~ Binding textures
	for(unsigned int i = 0; i < m_number_of_color_textures; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_texture_color_id[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	
	glBindTexture(GL_TEXTURE_2D, m_depth_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	//~ Generating framebuffer
	glGenFramebuffers(1, &m_framebuffer_id);
	
	//~ Binding framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_id);
	
	for(unsigned int i = 0; i < m_number_of_color_textures; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_texture_color_id[i], 0);
		m_draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture_id, 0);
	
	//~ Checking
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		delete this;
		std::cerr << "Framebuffer error" << std::endl;
	}
	
	//~ Unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Framebuffer::~Framebuffer()
{
	//~ Deleting textures
	glDeleteTextures(m_number_of_color_textures, m_texture_color_id);
	glDeleteTextures(1, &m_depth_texture_id);
	
	//~ Delete arrays
	delete[] m_texture_color_id;
	delete[] m_draw_buffers;
	
	//~ Deleting Framebuffer
	glDeleteFramebuffers(1,&m_framebuffer_id);
}

//~ Getters
GLuint Framebuffer::get_framebuffer_id() const
{
	return m_framebuffer_id;
}

GLuint* Framebuffer::get_texture_color_id() const
{
	return m_texture_color_id;
}

unsigned int Framebuffer::get_number_of_color_textures() const
{
	return m_number_of_color_textures;
}

GLenum* Framebuffer::get_draw_buffers() const
{
	return m_draw_buffers;
}

GLuint Framebuffer::get_depth_texture_id() const
{
	return m_depth_texture_id;
}
