/***************************************************************************
									Camera.hpp
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

//!  Camera
/*!
  * Camera
  * \author R. Bertozzi & S. Bougeois
  * \brief Camera
  * \file Camera.hpp
*/

#pragma once

#include <cmath>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

/*!
 * \brief Camera that is viewing the scene
 */ 
class Camera
{
	public:
		//! Constructor
		/*!
		 * \param width Width of the window
		 * \param height Height of the window
		 * \param type 1 if we are considering the left camera, 0 otherwise 
		 */
		Camera(int width, int height,float near, int type);
		//! Destructor
		~Camera();
		
		//! Computes the View matrix from the position, the target and the up vectors of the camera
		void compute_view_matrix();
		//! Computes the Projection matrix
                void compute_projection_matrix(float dc, float l);
		
		//! Gets the View matrix
		/*!
		 * \return View matrix
		 */
		glm::mat4 get_view_matrix() const;
		//! Gets the Projection matrix
		/*!
		 * \return Projection matrix
		 */
		glm::mat4 get_projection_matrix() const;
		//! Gets the position of the camera
		/*!
		 * \return Position of the camera
		 */
		glm::vec3 get_position() const;
		//! Gets the field of view of the camera
		/*!
		 * \return Field of view of the camera
		 */
		float get_fov() const;
		//! Gets the ratio of the camera
		/*!
		 * \return Ratio of the camera
		 */
		float get_ratio() const;
		//! Gets the near of the camera
		/*!
		 * \return Near of the camera
		 */
		float get_near() const;
		//! Gets the far of the camera
		/*!
		 * \return Far of the camera
		 */
		float get_far() const;
		//! Gets the DIOC of the camera
		/*!
		 * \return DIOC of the camera
		 */
		float get_dioc() const;
		//! Gets the target of the camera
		/*!
		 * \return Target of the camera
		 */
		glm::vec3 get_target() const;
		//! Gets the horizontal angle of the camera
		/*!
		 * \return Horizontal angle of the camera
		 */
		float get_horizontal_angle() const;
		//! Gets the vertical angle of the camera
		/*!
		 * \return Vertical angle of the camera
		 */
		float get_vertical_angle() const;
		
		//! Sets the position of the camera
		/*!
		 * \param position Vector position
		 */
		void set_position(const glm::vec3 position);
		//! Sets the target of the camera
		/*!
		 * \param target Vector target
		 */
		void set_target(const glm::vec3 target);
		//! Sets the up vector of the camera
		/*!
		 * \param up Up vector
		 */
		void set_up(const glm::vec3 up);
		//! Sets the horizontal angle of the camera
		/*!
		 * \param angle
		 */
		void set_horizontal_angle(const float angle);
		//! Sets the vertical angle of the camera
		/*!
		 * \param angle
		 */
		void set_vertical_angle(const float angle);
		//! Sets the dioc of the camera
		/*!
		 * \param dioc The new DIOC
		 */
		void set_dioc(const float dioc);
		//! Sets the right vector of the camera
		/*!
		 * \param right The new right vector
		 */
		void set_right(const glm::vec3 right);
	private:
		//! Specifies if the camera is the left (1) or the right(0) one
		int m_type;
		float m_fov;
		float m_ratio;
		float m_near;
		float m_far;
		
		glm::vec3 m_position;
		glm::vec3 m_up;
		glm::vec3 m_target;
		glm::vec3 m_right;
		
		glm::mat4 m_view_matrix;
		glm::mat4 m_projection_matrix;
		
		float m_horizontal_angle;
		float m_vertical_angle;
		
		int m_width;
		int m_height;
		
		float m_dioc;
};
