/***************************************************************************
									Renderer.hpp
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

//!  Renderer of the context
/*!
  * \author R. Bertozzi & S. Bougeois
  * \brief Renderer of the context
  * \file Renderer.hpp
*/

#pragma once

#ifdef _WIN32
	#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <fstream>
#include <SDL/SDL.h>
#include <dirent.h>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Object.hpp"
#include "Rig.hpp"
#include "Framebuffer.hpp"
#include "imgui/imgui.h"
#include "imgui/imguiRenderGL.h"

/*!
 * \brief Renderer of the context
 */ 
class Renderer
{
	public:
		//! Constructor
		/*!
		 * Initializes several elements such as GLEW, the object, the quad, the shaders, the framebuffers...
		 * \param width Width of the window
		 * \param height Height of the window
		 */ 
		Renderer(int width, int height);
		//! Destructor
		~Renderer();
		
		//! Renders the scene thanks to the two cameras and the framebuffers
		void render();
		//! Finds all the available files in a directory
		/*!
		 * Finds all the available files in a directory using a specific pattern
		 * \param directory The directory where the search will be made
		 * \param container The container where the names of the files will be stored
		 */ 
		void find_available_files(const char* directory,std::vector<std::string> &container);
		//! Loads an object in the renderer
		/*!
		 * Loads an object in the renderer
		 * \param model The model to load
		 * \param texture The texture to load
		 */ 
		void load_object(const std::string model,const std::string texture);
		//! Loads the normal map
		void load_normal_map();
		//! Renders the GUI
		void render_GUI();
		//! Renders the Space Screen Ambient Occlusion map
		/*!
		 * \param positions_map The texture containing the positions. Can be found in the geometry buffer
		 * \param normals_map The texture containing the normals. Can be found in the geometry buffer
		 * \param random_map The texture containing the random data needed by the SSAO technique
		 */ 
		void render_SSAO(const GLuint positions_map, const GLuint normals_map, const GLuint random_map, glm::mat4 view, glm::mat4 projection, glm::mat4 model,const GLuint depth_map);
		//! Blurs a texture
		/*!
		 * \param texture_to_blur The texture to be blur. The blur coefficient is available in its own slider in the GUI
		 */
		void blur(const GLuint texture_to_blur);
		//! Blends the ssao map and the color map
		/*!
		 * \param color_map The texture to be blend with the occlusion map.
		 * \param occlusion_map The occlusion map
		 */ 
		void blend_SSAO(const Framebuffer* output_frambuffer, const GLuint color_map, const GLuint occlusion_map);
		void toggle_ssao(const int enable_disable);
		//! Gets the rig maintaining the two cameras
		/*!
		 * \return The rig
		 */ 
		Rig* get_rig() const;
		//! Gets the view mode
		/*!
		 * \return the view mode
		 */
		int get_view_mode() const;
		//! Gets the display gui mode
		/*!
		 * \return the display gui mode
		 */
		bool get_display_gui() const;
		//! Gets the distance between the camera and the virtual plane
		/*!
		 * \return the distance between the camera and the virtual plane
		 */
		float get_dc() const;

		//! Gets the virtual screen width
		/*!
		 * \return the virtual screen width
		 */
		float get_l() const;

		//! Gets the keyboard's layout
		/*!
		 * \return the layout of the keyboard
		 */
		int get_keyboard_layout() const;
		//! Sets the view mode
		/*!
		 * \param mode the view mode
		 */
		void set_view_mode(const int mode);
		//! Sets the display gui mode
		/*!
		 * \param mode the display mode
		 */
		void set_display_gui(const bool mode);
		//! Sets the keyboard's layout
		/*!
		 * \param layout the identifier of the keyboard
		 */
		void set_keyboard_layout(const int layout);
	private:
		int m_width;
		int m_height;
	
		const char* readFile(const char* filePath);
		GLuint loadProgram(const char* vertexShaderFile, const char* fragmentShaderFile);
		
		Rig* m_rig;
		Object* m_object;
		Object* m_quad_left;
		Object* m_quad_right;
		int m_view_mode;
		
		GLuint m_basic_shader_program;
		GLuint m_basic_shader_model_matrix_position;
		GLuint m_basic_shader_view_matrix_position;
		GLuint m_basic_shader_projection_matrix_position;
		
		GLuint m_lighting_shader_program;
		GLuint m_lighting_shader_model_matrix_position;
		GLuint m_lighting_shader_view_matrix_position;
		GLuint m_lighting_shader_projection_matrix_position;
		GLuint m_lighting_shader_camera_position;
		GLuint m_lighting_shader_diffuse_texture;
		GLuint m_lighting_shader_light_intensity;
		GLuint m_lighting_shader_light_radius;

		GLuint m_quad_shader;
		GLuint m_quad_shader_texture_1;
		GLuint m_quad_shader_texture_2;
		
		GLuint m_geometry_buffer_shader_program;
		GLuint m_geometry_buffer_shader_model_matrix_location;
		GLuint m_geometry_buffer_shader_view_matrix_location;
		GLuint m_geometry_buffer_shader_projection_matrix_location;
		GLuint m_geometry_buffer_shader_diffuse_location;

		GLuint m_light_accumulation_shader_program;
		GLuint m_light_accumulation_camera_position_location;
		GLuint m_light_accumulation_light_position_location;
		GLuint m_light_accumulation_light_color_location;
		GLuint m_light_accumulation_light_intensity_location;
		GLuint m_light_accumulation_material_location;
		GLuint m_light_accumulation_normal_location;
		GLuint m_light_accumulation_depth_location;
		GLuint m_light_accumulation_view_matrix_location;
		GLuint m_light_accumulation_projection_matrix_location;
		GLuint m_light_accumulation_shadow_location;
		GLuint m_light_accumulation_light_projection_location;
		GLuint m_light_accumulation_light_direction_location;
		
		GLuint m_ssao_shader_program;
		GLuint m_ssao_radius_location;
		GLuint m_ssao_biais_location;
		GLuint m_ssao_scale_location;
		GLuint m_ssao_normals_texture_location;
		GLuint m_ssao_positions_texture_location;
		GLuint m_ssao_normal_map_location;
		GLuint m_ssao_nb_samples_location;
		GLuint m_ssao_view_matrix_location;
		GLuint m_ssao_projection_matrix_location;
		GLuint m_ssao_model_matrix_location;
		GLuint m_ssao_depth_location;
		
		GLuint m_normal_map_texture;
		
		GLuint m_blur_shader_program;
		GLuint m_blur_texture_to_blur_location;
		GLuint m_blur_coef_location;
		
		GLuint m_ssao_blend_shader_program;
		GLuint m_ssao_blend_occlusion_map_location;
		GLuint m_ssao_blend_color_map_location;
		
		GLuint m_shadow_shader_program;
		GLuint m_shadow_projection_matrix_location;
		GLuint m_shadow_model_matrix_location;
		GLuint m_shadow_view_matrix_location;

		float m_lightIntensity;
		float m_radiusLight;
		
		Framebuffer* m_left_camera_framebuffer;
		Framebuffer* m_right_camera_framebuffer;
		Framebuffer* m_geometry_buffer_framebuffer;
		Framebuffer* m_ssao_framebuffer;
		Framebuffer* m_blur_ssao_framebuffer;
		Framebuffer* m_left_ssao_blend_framebuffer;
		Framebuffer* m_right_ssao_blend_framebuffer;
		Framebuffer* m_shadow_framebuffer;
		
		bool m_display_gui;
		
		std::vector<std::string> m_list_of_models;
		std::vector<std::string> m_list_of_textures;
		
		bool m_gui_models_toggle;
		bool m_gui_textures_toggle;
		bool m_gui_keyboard_layout;
		int m_toggle;
		
		//~ Distance between camera and virtual screen
		float m_dc;
		//~ width of the virtual screen
		float m_l;
		
		std::string m_selected_model;
		std::string m_selected_texture;
		
		//0 : AZERTY
		//1 : QWERTY
		int m_keyboard_layout;
		
		//~ SSAO
		float m_ssao_biais_value;
		float m_ssao_radius_value;
		float m_ssao_scale_value;
		float m_ssao_nb_samples_value;
		
		//~ Blur
		float m_blur_coef_value;
		
		bool m_is_ssao_enabled;
};
