/***************************************************************************
									Rig.hpp
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

//!  The rig that maintains the cameras in a single entity
/*!
  * \author R. Bertozzi & S. Bougeois
  * \brief The rig that maintains the cameras in a single entity
  * \file Rig.hpp
*/

#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Camera.hpp"

#include <iostream>
#include <cmath>

/*!
 * \brief The rig that maintains the cameras in a single entity
 */ 
class Rig
{
	public:
		//! Constructor
		/*!
		 * \param position The position of the rig
		 * \param up The up vector of the rig
		 * \param target The target vector of the rig
		 * \param width The width of the window
		 * \param height The height of the window
		 */ 
		Rig(glm::vec3 position, float dioc, float dc, float l, glm::vec3 up, glm::vec3 target, int width, int height);
		//! Destructor
		~Rig();
		
		//! Sets the position of the rig
		/*!
		 * \param position The position to be written
		 */ 
		void set_position(const glm::vec3 position);
		//! Sets the up vector of the rig
		/*!
		 * \param up The up vector to be written
		 */
		void set_up(const glm::vec3 up);
		//! Sets the target vector of the rig
		/*!
		 * \param up The target vector to be written
		 */
		void set_target(const glm::vec3 target);
		
		//! Updates the position of the rig thanks to the user's controls
		/*!
		 * \param direction The direction entered by the user (0,1,2,3)
		 */
		void update_position(const int direction, const float delta);
		//! Updates the horizontal angle of the rig thanks to the user's mouse
		/*!
		 * \param mouse_x The x-coordinate of the cursor in the window
		 */
		void update_horizontal_angle(const int mouse_x);
		//! Updates the vertical angle of the rig thanks to the user's mouse
		/*!
		 * \param mouse_y The y-coordinate of the cursor in the window
		 */
		void update_vertical_angle(const int mouse_y);
		//! Updates the target of the rig
		void update_target();
		//! Updates up vector of the cameras
		void update_up();
		//! Changes the dioc and applies the proper effects to the cameras
		/*!
		 * \param delta The value that is going to be added to the dioc (can be negative)
		 * \param dc The distance between the camera and the virtual plane
		 */
		void change_dioc(const float delta, const float dc, const float l);
		//! Resets the dioc to 6.5cm and applies the proper effects to the cameras
		void reset_dioc(const float dc, const float l);
		//! Gets the position of the rig
		/*!
		 * \return The position of the rig
		 */ 
		glm::vec3 get_position() const;
		//! Gets the target of the rig
		/*!
		 * \return The target of the rig
		 */ 
		glm::vec3 get_target() const;
		//! Gets the first camera
		/*!
		 * \return The first camera
		 */ 
		Camera* get_camera_one() const;
		//! Gets the second camera
		/*!
		 * \return The second camera
		 */ 
		Camera* get_camera_two() const;
	private:
		Camera* m_camera_one;
		Camera* m_camera_two;
		
		glm::vec3 m_position;
		glm::vec3 m_up;
		glm::vec3 m_target;
		glm::vec3 m_right;
		
		float m_speed;
		float m_mouse_speed;
		float m_horizontal_angle;
		float m_vertical_angle;
		
		int m_width;
		int m_height;
};
