/***************************************************************************
									Rig.cpp
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
 * \file Rig.cpp
 * \brief The rig that maintains the cameras in a single entity
 * \author R. Bertozzi & S. Bougeois 
 */

#include "../include/Rig.hpp"

Rig::Rig(glm::vec3 position, float dioc, float dc, float l, glm::vec3 up, glm::vec3 target, int width, int height):
	m_position(position),
	m_up(up),
	m_target(target),
	m_speed(0.15f),
	m_mouse_speed(0.005f),
	m_horizontal_angle(M_PI),
	m_vertical_angle(0.0f),
	m_width(width),
	m_height(height)
{
	m_camera_one = new Camera(m_width,m_height, dc*(2.0f/3.0f), 0);
	m_camera_two = new Camera(m_width,m_height, dc*(2.0f/3.0f), 1);
	
	m_camera_one->set_dioc(dioc);
	m_camera_two->set_dioc(dioc);
	
	m_camera_one->set_up(m_up);
	m_camera_two->set_up(m_up);
	
	update_horizontal_angle(m_width/2);
	update_vertical_angle(m_height/2);
	update_target();
	
	glm::vec3 orthogonal_vector = glm::normalize(glm::cross(m_up,m_target));
	glm::vec3 displacement = (m_camera_one->get_dioc()/2)*orthogonal_vector;
	m_camera_one->set_position(m_position - displacement);
	m_camera_two->set_position(m_position + displacement);

	m_camera_one->compute_projection_matrix(dc, l);
	m_camera_two->compute_projection_matrix(dc, l);
}

Rig::~Rig()
{
	delete m_camera_one;
	delete m_camera_two;
}

void Rig::update_position(const int direction, const float delta)
{
	glm::vec3 orthogonal_vector = glm::normalize(glm::cross(m_up,m_target));
	glm::vec3 displacement = glm::vec3(0.0,0.0,0.0);
	
	switch(direction)
	{
		//~ Backward
		case 0 : 	m_position -= m_target * (m_speed * delta);
					displacement = (m_camera_one->get_dioc()/2)*orthogonal_vector;
					
					m_camera_one->set_position(m_position - displacement);
					m_camera_two->set_position(m_position + displacement);
		break;
		//~ Forward
		case 1 : 	m_position += m_target * (m_speed * delta);
					displacement = (m_camera_one->get_dioc()/2)*orthogonal_vector;
					
					m_camera_one->set_position(m_position - displacement);
					m_camera_two->set_position(m_position + displacement);
		break;
		//~ Left
		case 2 :	{
						glm::vec3 right = glm::vec3(sin(m_horizontal_angle - M_PI/2.0f), 0, cos(m_horizontal_angle - M_PI/2.0f));
						m_right = glm::normalize(right);

						m_position -= right * (m_speed * delta);

						glm::vec3 displacement_camera = (m_camera_one->get_dioc()/2)*orthogonal_vector;
						
						m_camera_one->set_position(m_position - displacement_camera);
						m_camera_two->set_position(m_position + displacement_camera);
					}
		break;
		//~ Right
		case 3 :	{
						glm::vec3 right = glm::vec3(sin(m_horizontal_angle - M_PI/2.0f), 0, cos(m_horizontal_angle - M_PI/2.0f));
						m_right = glm::normalize(right);

						m_position += right * (m_speed * delta);

						glm::vec3 displacement_camera = (m_camera_one->get_dioc()/2)*orthogonal_vector;
						
						m_camera_one->set_position(m_position - displacement_camera);
						m_camera_two->set_position(m_position + displacement_camera);
					}
		break;
		//~ Up
		case 4 :	{
						m_position.y += (m_speed * delta);
						displacement = (m_camera_one->get_dioc()/2)*orthogonal_vector;
						m_camera_one->set_position(m_position - displacement);
						m_camera_two->set_position(m_position + displacement);
					}
		break;
		//~ Down
		case 5 :	{
						m_position.y -= (m_speed * delta);
						displacement = (m_camera_one->get_dioc()/2)*orthogonal_vector;
						m_camera_one->set_position(m_position - displacement);
						m_camera_two->set_position(m_position + displacement);
					}
		break;
	}
}

void Rig::update_horizontal_angle(const int mouse_x)
{
	m_horizontal_angle += (m_mouse_speed * float(m_width/2.0 - mouse_x));
	m_camera_one->set_horizontal_angle(m_horizontal_angle);
	m_camera_two->set_horizontal_angle(m_horizontal_angle);
}

void Rig::update_vertical_angle(const int mouse_y)
{
	m_vertical_angle += (m_mouse_speed * float(m_height/2.0 - mouse_y));
	m_camera_one->set_vertical_angle(m_vertical_angle);
	m_camera_two->set_vertical_angle(m_vertical_angle);
}

void Rig::update_target()
{
	m_target = glm::vec3(cos(m_vertical_angle) * sin(m_horizontal_angle), sin(m_vertical_angle), cos(m_vertical_angle) * cos(m_horizontal_angle));
	m_camera_one->set_target(m_target);
	m_camera_two->set_target(m_target);
}

void Rig::update_up()
{
	glm::vec3 right = glm::vec3(sin(m_horizontal_angle - M_PI/2.0f), 0, cos(m_horizontal_angle - M_PI/2.0f));
	m_right = glm::normalize(right);
	glm::vec3 up = glm::cross(m_right, m_target);
	
	m_camera_one->set_up(up);
	m_camera_two->set_up(up);
}

void Rig::change_dioc(const float delta, const float dc, const float l)
{
	m_camera_one->set_dioc(m_camera_one->get_dioc()+delta);
	m_camera_two->set_dioc(m_camera_two->get_dioc()+delta);
	
	m_camera_one->compute_projection_matrix(dc, l);
	m_camera_two->compute_projection_matrix(dc, l);
	
	update_position(0,0.0f);
}

void Rig::reset_dioc(const float dc, const float l)
{
	m_camera_one->set_dioc(0.065);
	m_camera_two->set_dioc(0.065);
	
	m_camera_one->compute_projection_matrix(dc, l);
	m_camera_two->compute_projection_matrix(dc, l);
	
	update_position(0,0.0f);
}

//~ Getters
glm::vec3 Rig::get_position() const
{
	return m_position;
}

//~ Setters
void Rig::set_position(const glm::vec3 position)
{
	m_position = position;
}

void Rig::set_up(const glm::vec3 up)
{
	m_up = up;
}

void Rig::set_target(const glm::vec3 target)
{
	m_target = target;
}

glm::vec3 Rig::get_target() const
{
	return m_target;
}

Camera* Rig::get_camera_one() const
{
	return m_camera_one;
}

Camera* Rig::get_camera_two() const
{
	return m_camera_two;
}
