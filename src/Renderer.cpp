/***************************************************************************
									Renderer.cpp
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
 * \file Renderer.cpp
 * \brief Renderer of the context
 * \author R. Bertozzi & S. Bougeois 
 */

#include "../include/Renderer.hpp"

Renderer::Renderer(int width, int height):
	m_width(width),
	m_height(height),
	m_display_gui(true),
	m_gui_models_toggle(false),
	m_gui_textures_toggle(false),
	m_gui_keyboard_layout(true),
	m_dc(2.0f),
	m_l(4.0f),
	m_selected_model(""),
	m_selected_texture(""),
	m_keyboard_layout(0),
	m_ssao_biais_value(1.50f),
	m_ssao_radius_value(0.06f),
	m_ssao_scale_value(2.0f),
	m_ssao_nb_samples_value(16),
	m_blur_coef_value(8),
	m_is_ssao_enabled(false)
{
	GLenum error;
	if((error = glewInit()) != GLEW_OK) {
		throw std::runtime_error((const char*)glewGetErrorString(error));
	}

	find_available_files((const char*)"models",m_list_of_models);
	find_available_files((const char*)"textures",m_list_of_textures);

	if (!imguiRenderGLInit("fonts/DroidSans.ttf"))
	{
		fprintf(stderr, "Could not init GUI renderer.\n");
		exit(EXIT_FAILURE);
	}
	
	//~ Loading quads
	try
	{
		m_quad_left = new Object("models/quad.obj",NULL);
		m_quad_right = new Object("models/quad.obj",NULL);
	}
	catch(int e)
	{
		switch(e)
		{
			case 0 : 	std::cout << "3D Model not found" << std::endl;
						break;
		}
	}

	//~ Compiling shaders
	m_basic_shader_program = loadProgram("shaders/basic.vertex.glsl","shaders/basic.fragment.glsl");
	m_lighting_shader_program = loadProgram("shaders/lighting.vertex.glsl","shaders/lighting.fragment.glsl");
	m_quad_shader = loadProgram("shaders/quad.vertex.glsl","shaders/quad.fragment.glsl");
	m_geometry_buffer_shader_program = loadProgram("shaders/geometry_buffer.vertex.glsl","shaders/geometry_buffer.fragment.glsl");
	m_light_accumulation_shader_program = loadProgram("shaders/light_accumulation.vertex.glsl","shaders/light_accumulation.fragment.glsl");
	m_ssao_shader_program = loadProgram("shaders/ssao.vertex.glsl","shaders/ssao.fragment.glsl");
	m_blur_shader_program = loadProgram("shaders/blur.vertex.glsl","shaders/blur.fragment.glsl");
	m_ssao_blend_shader_program = loadProgram("shaders/ssao_blend.vertex.glsl","shaders/ssao_blend.fragment.glsl");
	
	//~ Locating uniforms
	m_basic_shader_model_matrix_position = glGetUniformLocation(m_basic_shader_program,"model_matrix");
	m_basic_shader_view_matrix_position = glGetUniformLocation(m_basic_shader_program,"view_matrix");
	m_basic_shader_projection_matrix_position = glGetUniformLocation(m_basic_shader_program,"projection_matrix");

	m_lighting_shader_model_matrix_position = glGetUniformLocation(m_lighting_shader_program,"model_matrix");
	m_lighting_shader_view_matrix_position = glGetUniformLocation(m_lighting_shader_program,"view_matrix");
	m_lighting_shader_projection_matrix_position = glGetUniformLocation(m_lighting_shader_program,"projection_matrix");
	m_lighting_shader_camera_position = glGetUniformLocation(m_lighting_shader_program,"camera_position");
	m_lighting_shader_diffuse_texture = glGetUniformLocation(m_lighting_shader_program, "diffuse_texture");
	m_lighting_shader_light_intensity = glGetUniformLocation(m_lighting_shader_program, "lightIntensity");
	m_lighting_shader_light_radius = glGetUniformLocation(m_lighting_shader_program, "distance");

	m_quad_shader_texture_1 = glGetUniformLocation(m_quad_shader, "renderedTexture1");
	m_quad_shader_texture_2 = glGetUniformLocation(m_quad_shader, "renderedTexture2");
	glBindFragDataLocation(m_quad_shader, 0, "color");

	m_geometry_buffer_shader_model_matrix_location = glGetUniformLocation(m_geometry_buffer_shader_program,"model_matrix");
	m_geometry_buffer_shader_view_matrix_location = glGetUniformLocation(m_geometry_buffer_shader_program,"view_matrix");
	m_geometry_buffer_shader_projection_matrix_location = glGetUniformLocation(m_geometry_buffer_shader_program,"projection_matrix");
	m_geometry_buffer_shader_diffuse_location = glGetUniformLocation(m_geometry_buffer_shader_program,"diffuse_texture");
	//~ Adding some parameters
	glBindFragDataLocation(m_geometry_buffer_shader_program, 0, "out_color");
	glBindFragDataLocation(m_geometry_buffer_shader_program, 1, "out_normal");
	glBindFragDataLocation(m_geometry_buffer_shader_program, 2, "out_position");

	m_light_accumulation_camera_position_location = glGetUniformLocation(m_light_accumulation_shader_program,"camera_position");
	m_light_accumulation_light_position_location = glGetUniformLocation(m_light_accumulation_shader_program,"light_position");
	m_light_accumulation_light_color_location = glGetUniformLocation(m_light_accumulation_shader_program,"light_color");
	m_light_accumulation_light_intensity_location = glGetUniformLocation(m_light_accumulation_shader_program,"light_intensity");
	m_light_accumulation_material_location = glGetUniformLocation(m_light_accumulation_shader_program,"material_texture");
	m_light_accumulation_normal_location = glGetUniformLocation(m_light_accumulation_shader_program,"normal_texture");
	m_light_accumulation_depth_location = glGetUniformLocation(m_light_accumulation_shader_program,"depth_texture");
	m_light_accumulation_view_matrix_location = glGetUniformLocation(m_light_accumulation_shader_program,"view_matrix");
	m_light_accumulation_projection_matrix_location = glGetUniformLocation(m_light_accumulation_shader_program,"projection_matrix");
	//~ Adding some parameters
	glBindFragDataLocation(m_light_accumulation_shader_program, 0, "out_frag_color");

	m_ssao_radius_location = glGetUniformLocation(m_ssao_shader_program,"SamplingRadius");
	m_ssao_biais_location = glGetUniformLocation(m_ssao_shader_program,"OcclusionBias");
	m_ssao_scale_location = glGetUniformLocation(m_ssao_shader_program,"Scale");
	m_ssao_normals_texture_location = glGetUniformLocation(m_ssao_shader_program,"Normals");
	m_ssao_positions_texture_location = glGetUniformLocation(m_ssao_shader_program,"Positions");
	m_ssao_normal_map_location = glGetUniformLocation(m_ssao_shader_program,"NormalMap");
	m_ssao_nb_samples_location = glGetUniformLocation(m_ssao_shader_program,"NbSamples");
	m_ssao_view_matrix_location = glGetUniformLocation(m_ssao_shader_program,"viewMatrix");
	m_ssao_projection_matrix_location = glGetUniformLocation(m_ssao_shader_program,"projectionMatrix");
	m_ssao_model_matrix_location = glGetUniformLocation(m_ssao_shader_program,"modelMatrix");
	m_ssao_depth_location = glGetUniformLocation(m_ssao_shader_program,"Depth");
	//~ Adding some parameters
	glBindFragDataLocation(m_ssao_shader_program, 0, "Color");
	
	m_blur_texture_to_blur_location = glGetUniformLocation(m_blur_shader_program,"TextureToBlur");
	m_blur_coef_location = glGetUniformLocation(m_blur_shader_program,"BlurCoef");
	//~ Adding some parameters
	glBindFragDataLocation(m_blur_shader_program, 0, "OutColor");

	m_ssao_blend_color_map_location = glGetUniformLocation(m_ssao_blend_shader_program,"ColorMap");
	m_ssao_blend_occlusion_map_location = glGetUniformLocation(m_ssao_blend_shader_program,"OcclusionMap");

	load_normal_map();

	m_lightIntensity = 15.5f;
	m_radiusLight = 4.8f;
	
	//~ Creating the rig
	glm::vec3 rig_position = glm::vec3(0.0f,0.0f,2.0f);
	glm::vec3 rig_up = glm::vec3(0.0f,1.0f,0.0f);
	glm::vec3 rig_target = glm::vec3(0.0f,0.0f,1.0f);
	float rig_dioc = 0.065;
	m_rig = new Rig(rig_position, rig_dioc, m_dc, m_l, rig_up, rig_target, m_width, m_height);
	
	//~ Creating the framebuffers
	m_left_camera_framebuffer = new Framebuffer(1,m_width,m_height);
	m_right_camera_framebuffer = new Framebuffer(1,m_width,m_height);
	m_geometry_buffer_framebuffer = new Framebuffer(3,m_width,m_height);
	m_ssao_framebuffer = new Framebuffer(1,m_width,m_height);
	m_blur_ssao_framebuffer = new Framebuffer(1,m_width,m_height);
	m_left_ssao_blend_framebuffer = new Framebuffer(1,m_width,m_height);
	m_right_ssao_blend_framebuffer = new Framebuffer(1,m_width,m_height);

	//~ //Default view : Anaglyph
	m_view_mode = 0;
}

Renderer::~Renderer()
{
	//~ Deleting Framebuffers
	delete m_left_camera_framebuffer;
	delete m_right_camera_framebuffer;
	delete m_geometry_buffer_framebuffer;
	delete m_ssao_framebuffer;
	delete m_blur_ssao_framebuffer;
	delete m_left_ssao_blend_framebuffer;
	delete m_right_ssao_blend_framebuffer;
	//~ Deleting objects
	delete m_object;
	delete m_quad_left;
	delete m_quad_right;
	//~ Deleting cameras and rig
	delete m_rig;
	imguiRenderGLDestroy();
}

void Renderer::render()
{
	glClearColor(0.0,0.0,0.0,1.0);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//~ ------------------------------------------------------------------------------------------------------------
	//~ Rendering the GUI
	//~ ------------------------------------------------------------------------------------------------------------
	if(m_display_gui)
	{
		render_GUI();
	}
	else
	{
		if(m_object != NULL)
		{
			std::vector<glm::vec3> light_position;
			light_position.push_back(glm::vec3(-m_radiusLight,-m_radiusLight,-m_radiusLight));
			light_position.push_back(glm::vec3(m_radiusLight,-m_radiusLight,-m_radiusLight));
			light_position.push_back(glm::vec3(-m_radiusLight,m_radiusLight,-m_radiusLight));
			light_position.push_back(glm::vec3(m_radiusLight,m_radiusLight,-m_radiusLight));
			
			glm::vec3 light_color = glm::vec3(1.0f,1.0f,1.0f);
			//~ ------------------------------------------------------------------------------------------------------------
			//~ Rendering the first geometry buffer
			//~ ------------------------------------------------------------------------------------------------------------
			//~ Initializing the parameters
			glBindFramebuffer(GL_FRAMEBUFFER, m_geometry_buffer_framebuffer->get_framebuffer_id());
			glDrawBuffers(m_geometry_buffer_framebuffer->get_number_of_color_textures(), m_geometry_buffer_framebuffer->get_draw_buffers());
			glViewport(0, 0, m_width, m_height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//~ Choosing the geometry buffer
			glUseProgram(m_geometry_buffer_shader_program);
			//~ Sending uniforms
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_object->get_diffuse_texture());
			glUniform1i(m_geometry_buffer_shader_diffuse_location, 0);
			glUniformMatrix4fv(m_geometry_buffer_shader_model_matrix_location, 1, GL_FALSE, glm::value_ptr(m_object->get_model_matrix()));
			glUniformMatrix4fv(m_geometry_buffer_shader_view_matrix_location, 1, GL_FALSE, glm::value_ptr(m_rig->get_camera_one()->get_view_matrix()));
			glUniformMatrix4fv(m_geometry_buffer_shader_projection_matrix_location, 1, GL_FALSE, glm::value_ptr(m_rig->get_camera_one()->get_projection_matrix()));
			//~ Binding VAO
			glBindVertexArray(m_object->get_vao());
			//~ Drawing
			glDrawArrays(GL_TRIANGLES, 0, m_object->get_size());
			//~ Unbind
			glBindVertexArray(0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//~ ------------------------------------------------------------------------------------------------------------
			//~ Rendering the SSAO for the first camera
			//~ ------------------------------------------------------------------------------------------------------------
			render_SSAO(	m_geometry_buffer_framebuffer->get_texture_color_id()[1],
							m_geometry_buffer_framebuffer->get_texture_color_id()[2],
							m_normal_map_texture,
							m_rig->get_camera_one()->get_view_matrix(),
							m_rig->get_camera_one()->get_projection_matrix(),
							m_object->get_model_matrix(),
							m_geometry_buffer_framebuffer->get_depth_texture_id()
							);
			//~ ------------------------------------------------------------------------------------------------------------
			//~ Bluring the first SSAO
			//~ ------------------------------------------------------------------------------------------------------------
			blur(m_ssao_framebuffer->get_texture_color_id()[0]);
			//~ ------------------------------------------------------------------------------------------------------------
			//~ Rendering the first camera
			//~ ------------------------------------------------------------------------------------------------------------
			glBindFramebuffer(GL_FRAMEBUFFER, m_left_camera_framebuffer->get_framebuffer_id());
			glDrawBuffers(m_left_camera_framebuffer->get_number_of_color_textures(), m_left_camera_framebuffer->get_draw_buffers());
			glViewport(0, 0, m_width, m_height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//~ Choosing shader
			glUseProgram(m_light_accumulation_shader_program);
			
			glUniform3fv(m_light_accumulation_camera_position_location, GL_FALSE, glm::value_ptr(m_rig->get_camera_one()->get_position()));
			glUniformMatrix4fv(m_light_accumulation_view_matrix_location, 1, GL_FALSE, glm::value_ptr(m_rig->get_camera_one()->get_view_matrix()));
			glUniformMatrix4fv(m_light_accumulation_projection_matrix_location, 1, GL_FALSE, glm::value_ptr(m_rig->get_camera_one()->get_projection_matrix()));
			
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,m_geometry_buffer_framebuffer->get_texture_color_id()[0]);
			glUniform1i(m_light_accumulation_material_location,0);
			
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,m_geometry_buffer_framebuffer->get_texture_color_id()[1]);
			glUniform1i(m_light_accumulation_normal_location,1);
			
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D,m_geometry_buffer_framebuffer->get_depth_texture_id());
			glUniform1i(m_light_accumulation_depth_location,2);
			
			for(unsigned int i = 0; i < light_position.size(); ++i)
			{
				//~ Sending uniforms
				glUniform3fv(m_light_accumulation_light_position_location, 1, glm::value_ptr(light_position.at(i)));
				glUniform3fv(m_light_accumulation_light_color_location, 1, glm::value_ptr(light_color));
				glUniform1f(m_light_accumulation_light_intensity_location, m_lightIntensity);
				//~ Binding vao
				glBindVertexArray(m_quad_left->get_vao());
				//~ Drawing
				glDrawArrays(GL_TRIANGLES, 0, m_quad_left->get_size());
			}
			
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			//~ Unbind
			glBindVertexArray(0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//~ ------------------------------------------------------------------------------------------------------------
			//~ Blending Occlusion Map and Light Accumulation map
			//~ ------------------------------------------------------------------------------------------------------------
			blend_SSAO(m_left_ssao_blend_framebuffer, m_left_camera_framebuffer->get_texture_color_id()[0],m_blur_ssao_framebuffer->get_texture_color_id()[0]);
			//~ ------------------------------------------------------------------------------------------------------------
			//~ Rendering the second geometry buffer
			//~ ------------------------------------------------------------------------------------------------------------
			//~ //Initializing the parameters
			glBindFramebuffer(GL_FRAMEBUFFER, m_geometry_buffer_framebuffer->get_framebuffer_id());
			glDrawBuffers(m_geometry_buffer_framebuffer->get_number_of_color_textures(), m_geometry_buffer_framebuffer->get_draw_buffers());
			glViewport(0, 0, m_width, m_height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//~ //Choosing the geometry buffer
			glUseProgram(m_geometry_buffer_shader_program);
			//~ //Sending uniforms
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_object->get_diffuse_texture());
			glUniform1i(m_geometry_buffer_shader_diffuse_location, 0);
			glUniformMatrix4fv(m_geometry_buffer_shader_model_matrix_location, 1, GL_FALSE, glm::value_ptr(m_object->get_model_matrix()));
			glUniformMatrix4fv(m_geometry_buffer_shader_view_matrix_location, 1, GL_FALSE, glm::value_ptr(m_rig->get_camera_two()->get_view_matrix()));
			glUniformMatrix4fv(m_geometry_buffer_shader_projection_matrix_location, 1, GL_FALSE, glm::value_ptr(m_rig->get_camera_two()->get_projection_matrix()));
			//~ //Binding VAO
			glBindVertexArray(m_object->get_vao());
			//~ //Drawing
			glDrawArrays(GL_TRIANGLES, 0, m_object->get_size());
			//~ //Unbind
			glBindVertexArray(0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//~ ------------------------------------------------------------------------------------------------------------
			//~ Rendering the SSAO for the second camera
			//~ ------------------------------------------------------------------------------------------------------------
			render_SSAO(	m_geometry_buffer_framebuffer->get_texture_color_id()[1],
							m_geometry_buffer_framebuffer->get_texture_color_id()[2],
							m_normal_map_texture,
							m_rig->get_camera_two()->get_view_matrix(),
							m_rig->get_camera_two()->get_projection_matrix(),
							m_object->get_model_matrix(),
							m_geometry_buffer_framebuffer->get_depth_texture_id()
							);
			//~ ------------------------------------------------------------------------------------------------------------
			//~ Bluring the second SSAO
			//~ ------------------------------------------------------------------------------------------------------------
			blur(m_ssao_framebuffer->get_texture_color_id()[0]);
			//~ ------------------------------------------------------------------------------------------------------------
			//~ Rendering the second camera
			//~ ------------------------------------------------------------------------------------------------------------
			glBindFramebuffer(GL_FRAMEBUFFER, m_right_camera_framebuffer->get_framebuffer_id());
			glDrawBuffers(m_right_camera_framebuffer->get_number_of_color_textures(), m_right_camera_framebuffer->get_draw_buffers());
			glViewport(0, 0, m_width, m_height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//~ //Choosing shader
			glUseProgram(m_light_accumulation_shader_program);
			
			glUniform3fv(m_light_accumulation_camera_position_location, GL_FALSE, glm::value_ptr(m_rig->get_camera_two()->get_position()));
			glUniformMatrix4fv(m_light_accumulation_view_matrix_location, 1, GL_FALSE, glm::value_ptr(m_rig->get_camera_two()->get_view_matrix()));
			glUniformMatrix4fv(m_light_accumulation_projection_matrix_location, 1, GL_FALSE, glm::value_ptr(m_rig->get_camera_two()->get_projection_matrix()));
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,m_geometry_buffer_framebuffer->get_texture_color_id()[0]);
			glUniform1i(m_light_accumulation_material_location,0);
			
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,m_geometry_buffer_framebuffer->get_texture_color_id()[1]);
			glUniform1i(m_light_accumulation_normal_location,1);
			
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D,m_geometry_buffer_framebuffer->get_depth_texture_id());
			glUniform1i(m_light_accumulation_depth_location,2);
			
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);
			
			for(unsigned int i = 0; i < light_position.size(); ++i)
			{
				//~ //Sending uniforms
				glUniform3fv(m_light_accumulation_light_position_location, 1, glm::value_ptr(light_position.at(i)));
				glUniform3fv(m_light_accumulation_light_color_location, 1, glm::value_ptr(light_color));
				glUniform1f(m_light_accumulation_light_intensity_location, m_lightIntensity);
				//~ //Binding vao
				glBindVertexArray(m_quad_left->get_vao());
				//~ //Drawing
				glDrawArrays(GL_TRIANGLES, 0, m_quad_left->get_size());
			}
			
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			//~ //Unbind
			glBindVertexArray(0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//~ ------------------------------------------------------------------------------------------------------------
			//~ Blending Occlusion Map and Light Accumulation map
			//~ ------------------------------------------------------------------------------------------------------------
			blend_SSAO(m_right_ssao_blend_framebuffer, m_right_camera_framebuffer->get_texture_color_id()[0],m_blur_ssao_framebuffer->get_texture_color_id()[0]);
			//~ ------------------------------------------------------------------------------------------------------------
			//~ Rendering the final view
			//~ ------------------------------------------------------------------------------------------------------------
			//~ //Anaglyph
			if (m_view_mode == 0)
			{
				glClearColor(0.0,0.0,0.0,1.0);
				glViewport(0, 0, m_width, m_height);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				//~ //Choosing shader
				glUseProgram(m_quad_shader);
				//~ //Sending uniforms
				if(m_is_ssao_enabled)
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, m_left_ssao_blend_framebuffer->get_texture_color_id()[0]);
					glUniform1i(m_quad_shader_texture_1, 0);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, m_right_ssao_blend_framebuffer->get_texture_color_id()[0]);
					glUniform1i(m_quad_shader_texture_2, 1);
				}
				else
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, m_left_camera_framebuffer->get_texture_color_id()[0]);
					glUniform1i(m_quad_shader_texture_1, 0);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, m_right_camera_framebuffer->get_texture_color_id()[0]);
					glUniform1i(m_quad_shader_texture_2, 1);
				}
				//~ //Binding vao
				glBindVertexArray(m_quad_left->get_vao());
				//~ //Drawing
				glDrawArrays(GL_TRIANGLES, 0, m_quad_left->get_size());
				//~ //Unbind
				glBindVertexArray(0);
			}
			
			//~ //Side by side
			else if(m_view_mode == 1)
			{
				//~ //Left view
				glClearColor(0.0,0.0,0.0,1.0);
				glViewport(0, 0, m_width/2, m_height);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				//~ //Choosing shader
				glUseProgram(m_quad_shader);
				//~ //Sending uniforms
				if(!m_is_ssao_enabled)
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, m_left_camera_framebuffer->get_texture_color_id()[0]);
					glUniform1i(m_quad_shader_texture_1, 0);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, m_left_camera_framebuffer->get_texture_color_id()[0]);
					glUniform1i(m_quad_shader_texture_2, 1);
				}
				else
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, m_left_ssao_blend_framebuffer->get_texture_color_id()[0]);
					glUniform1i(m_quad_shader_texture_1, 0);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, m_left_ssao_blend_framebuffer->get_texture_color_id()[0]);
					glUniform1i(m_quad_shader_texture_2, 1);
				}
				//~ //Binding vao
				glBindVertexArray(m_quad_left->get_vao());
				//~ //Drawing
				glDrawArrays(GL_TRIANGLES, 0, m_quad_left->get_size());
				//~ //Unbind
				glBindVertexArray(0);

				//~ //Right view
				glViewport(m_width/2, 0, m_width/2, m_height);
				//~ //Choosing shader
				glUseProgram(m_quad_shader);
				if(!m_is_ssao_enabled)
				{
					//~ //Sending uniforms
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, m_right_camera_framebuffer->get_texture_color_id()[0]);
					glUniform1i(m_quad_shader_texture_1, 0);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, m_right_camera_framebuffer->get_texture_color_id()[0]);
					glUniform1i(m_quad_shader_texture_2, 1);
				}
				else
				{
					//~ //Sending uniforms
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, m_right_ssao_blend_framebuffer->get_texture_color_id()[0]);
					glUniform1i(m_quad_shader_texture_1, 0);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, m_right_ssao_blend_framebuffer->get_texture_color_id()[0]);
					glUniform1i(m_quad_shader_texture_2, 1);
				}
				//~ //Binding vao
				glBindVertexArray(m_quad_right->get_vao());
				//~ //Drawing
				glDrawArrays(GL_TRIANGLES, 0, m_quad_right->get_size());
				//~ //Unbind
				glBindVertexArray(0);
			}
		}
	}
}

const char* Renderer::readFile(const char* filePath) {
    std::ifstream file(filePath);

    if (!file) {
        return 0;
    }

    file.seekg(0, std::ios_base::end);
    size_t length = file.tellg();
    file.seekg(0);

    char* src = new char[length];
    file.read(src, length);
    src[length - 1] = '\0';

    return src;
}

GLuint Renderer::loadProgram(const char* vertexShaderFile, const char* fragmentShaderFile) {
    const char* vertexShaderSource = this->readFile(vertexShaderFile);
    if(!vertexShaderSource) {
        std::cerr << "Unable to load " << vertexShaderFile << std::endl;
        return 0;
    }

    const char* fragmentShaderSource = this->readFile(fragmentShaderFile);
    if(!fragmentShaderSource) {
        std::cerr << "Unable to load " << fragmentShaderFile << std::endl;
        return 0;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, 0);
    glCompileShader(vertexShader);

    GLint compileStatus;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus == GL_FALSE) {
        GLint logLength;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);

        char* log = new char[logLength];

        glGetShaderInfoLog(vertexShader, logLength, 0, log);
        std::cerr << "Vertex Shader error:" << log << std::endl;
        std::cerr << vertexShaderSource << std::endl;

        delete [] log;
        return 0;
    }
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, 0);

    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus == GL_FALSE) {
        GLint logLength;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);

        char* log = new char[logLength];

        glGetShaderInfoLog(fragmentShader, logLength, 0, log);
        std::cerr << "Fragment Shader error:" << log << std::endl;
        std::cerr << fragmentShaderSource << std::endl;

        delete [] log;
        return 0;
    }

    GLuint program;

    program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glLinkProgram(program);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if(linkStatus == GL_FALSE) {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        char* log = new char[logLength];

        glGetProgramInfoLog(program, logLength, 0, log);
        std::cerr << "Program link error:" << log << std::endl;

        delete [] log;
        return 0;
    }

    delete [] vertexShaderSource;
    delete [] fragmentShaderSource;

    return program;
}

void Renderer::find_available_files(const char* directory,std::vector<std::string> &container)
{
	struct dirent* file = NULL;
	DIR* rep = opendir(directory);
	
	if(rep == NULL)
	{
		exit(1);
	}
	
	while((file = readdir(rep))!=NULL)
	{
		if (strcmp(file->d_name, "." ) != 0 && strcmp(file->d_name, ".." ) != 0)
		{
			container.push_back(file->d_name);
		}
	}
	
	for(std::vector<std::string>::iterator it = container.begin(); it!=container.end(); ++it)
	{
		iter_swap(it, min_element(it, container.end()));
	}
	
	if(closedir(rep) == -1)
	{
		exit(-1);
	}
}

void Renderer::load_normal_map()
{
	//~ Declarating values : width, height, components per pixel
	int w, h, comp;
	//~ Calling stbi
	unsigned char* normal_map = stbi_load((const char*)"textures/normalmap.jpg", &w, &h, &comp, 3);
	//~ Processing texture
	glGenTextures(1, &m_normal_map_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_normal_map_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, normal_map);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	stbi_image_free(normal_map);
}

void Renderer::load_object(const std::string model,const std::string texture)
{
	if(m_object != NULL) 
	{
		delete m_object;
	}
	//~ Loading object
	std::string m = "models/";
	m += model;
	
	std::string t = "textures/";
	t += texture;
	
	try
	{
		m_object = new Object(m.c_str(),t.c_str());
	}
	catch(int e)
	{
		switch(e)
		{
			case 0 : 	std::cout << "3D Model not found" << std::endl;
						break;
		}
	}
	m_object->set_model_matrix(glm::translate(m_object->get_model_matrix(),glm::vec3(0.00f,0.00f,-m_dc)));

	float avgDistToBarycentre = m_object->computeAvgDistToBarycentre();
	float scale = (m_dc*(2.0f/3.0f))/avgDistToBarycentre;
	m_object->set_model_matrix(glm::scale(m_object->get_model_matrix(),glm::vec3(scale,scale,scale)));

	glm::vec3 barycentre = m_object->computeBarycentre();
	barycentre *= scale;
	m_object->set_model_matrix(glm::translate(m_object->get_model_matrix(),-barycentre));

	m_object->set_model_matrix(glm::rotate(m_object->get_model_matrix(), 90.0f, glm::vec3(0, 1, 0)));
}

void Renderer::render_GUI()
{
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, m_width, m_height);
	glDisable(GL_DEPTH_TEST);

	unsigned char mbut = 0;
	int mscroll = 0;
	int mousex; int mousey;
	SDL_GetMouseState(&mousex, &mousey);
	mousey = m_height - mousey;

	if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)) mbut |= IMGUI_MBUT_LEFT;

	imguiBeginFrame(mousex, mousey, mbut, mscroll);
	int logScroll = 0;
	imguiBeginScrollArea("Settings",10,0, 200, m_height, &logScroll);
	imguiSlider("Light intensity", &m_lightIntensity, 0.0, 50.0, 0.1);
	imguiSlider("Radius light position", &m_radiusLight, 0.0, 20.0, 0.1);
	m_toggle = imguiCollapse("Models", "", m_gui_models_toggle);
	if (m_gui_models_toggle)
	{
		imguiIndent();
		for(std::vector<std::string>::iterator it = m_list_of_models.begin(); it!=m_list_of_models.end(); ++it)
		{
			int res = imguiButton((*it).c_str());
			if(res) 
			{
				m_selected_model = *it;
				if(m_selected_model != "" && m_selected_texture != "")
				{
					load_object(m_selected_model,m_selected_texture);
				}
			}
		}
		imguiUnindent();
	}
	if(m_toggle) m_gui_models_toggle = !m_gui_models_toggle;
	m_toggle = imguiCollapse("Textures", "", m_gui_textures_toggle);
	if (m_gui_textures_toggle)
	{
		imguiIndent();
		for(std::vector<std::string>::iterator it = m_list_of_textures.begin(); it!=m_list_of_textures.end(); ++it)
		{
			int res = imguiButton((*it).c_str());
			if(res) 
			{
				m_selected_texture = *it;
				if(m_selected_model != "" && m_selected_texture != "")
				{
					load_object(m_selected_model,m_selected_texture);
				}
			}
		}
		imguiUnindent();
	}
	if(m_toggle) m_gui_textures_toggle = !m_gui_textures_toggle;
	
	m_toggle = imguiCollapse("Keyboard", "", m_gui_keyboard_layout);
	if (m_gui_keyboard_layout)
	{
		imguiIndent();
			int res1 = imguiButton("Azerty layout");
			int res2 = imguiButton("Qwerty layout");
			if(res1)
			{
				m_keyboard_layout = 0;
			}
			if(res2)
			{
				m_keyboard_layout = 1;
			}
		imguiUnindent();
	}
	if(m_toggle) m_gui_keyboard_layout = !m_gui_keyboard_layout;
	
	imguiBeginScrollArea("Settings",m_width-200,0, 200, m_height, &logScroll);
	imguiSlider("Biais", &m_ssao_biais_value, 0.0, 10.0, 0.01);
	imguiSlider("Sampling radius", &m_ssao_radius_value, 0.0, 1.0, 0.01);
	imguiSlider("Scale", &m_ssao_scale_value, 0.0, 10.0, 0.01);
	imguiSlider("NbSamples", &m_ssao_nb_samples_value, 0.0, 64.0, 1.0);
	imguiSlider("Blur Coefficient", &m_blur_coef_value, 0.0, 36.0, 1.0);
	
	imguiEndScrollArea();
	imguiEndFrame();
	imguiRenderGLDraw(m_width, m_height); 

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::render_SSAO(const GLuint positions_map, const GLuint normals_map, const GLuint random_map, glm::mat4 view, glm::mat4 projection, glm::mat4 model, const GLuint depth_map)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ssao_framebuffer->get_framebuffer_id());
	glDrawBuffers(m_ssao_framebuffer->get_number_of_color_textures(), m_ssao_framebuffer->get_draw_buffers());
	glViewport( 0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_ssao_shader_program);

	glUniform1i(m_ssao_normals_texture_location, 0);
	glUniform1i(m_ssao_positions_texture_location, 1);
	glUniform1i(m_ssao_normal_map_location, 2);
	glUniform1i(m_ssao_depth_location, 3);
	glUniform1f(m_ssao_biais_location, m_ssao_biais_value);
	glUniform1f(m_ssao_radius_location, m_ssao_radius_value);
	glUniform1f(m_ssao_scale_location, m_ssao_scale_value);
	glUniform1f(m_ssao_nb_samples_location, m_ssao_nb_samples_value);
	glUniformMatrix4fv(m_ssao_view_matrix_location, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(m_ssao_projection_matrix_location, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(m_ssao_model_matrix_location, 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, positions_map);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normals_map);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, random_map);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	
	//~ //Binding vao
	glBindVertexArray(m_quad_left->get_vao());
	//~ //Drawing
	glDrawArrays(GL_TRIANGLES, 0, m_quad_left->get_size());
	//~ //Unbind
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::blur(const GLuint texture_to_blur)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_blur_ssao_framebuffer->get_framebuffer_id());
	glDrawBuffers(m_blur_ssao_framebuffer->get_number_of_color_textures(), m_blur_ssao_framebuffer->get_draw_buffers());
	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_blur_shader_program);

	glUniform1i(m_blur_texture_to_blur_location,0);
	glUniform1f(m_blur_coef_location,m_blur_coef_value);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_to_blur);
	
	//~ //Binding vao
	glBindVertexArray(m_quad_left->get_vao());
	//~ //Drawing
	glDrawArrays(GL_TRIANGLES, 0, m_quad_left->get_size());
	//~ //Unbind
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::blend_SSAO(const Framebuffer* output_frambuffer, const GLuint color_map, const GLuint occlusion_map)
{
	glBindFramebuffer(GL_FRAMEBUFFER, output_frambuffer->get_framebuffer_id());
	glDrawBuffers(output_frambuffer->get_number_of_color_textures(), output_frambuffer->get_draw_buffers());
	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//~ Choosing the geometry buffer
	glUseProgram(m_ssao_blend_shader_program);
	
	//~ Sending uniforms
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_map);
	glUniform1i(m_ssao_blend_color_map_location, 0);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, occlusion_map);
	glUniform1i(m_ssao_blend_occlusion_map_location, 1);
	
	//~ Binding VAO
	glBindVertexArray(m_quad_left->get_vao());
	//~ Drawing
	glDrawArrays(GL_TRIANGLES, 0, m_quad_left->get_size());
	//~ Unbind
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::toggle_ssao(const int enable_disable)
{
	m_is_ssao_enabled = enable_disable;
}

//~ Getters
Rig* Renderer::get_rig() const
{
	return m_rig;
}

int Renderer::get_view_mode() const
{
    return m_view_mode;
}

bool Renderer::get_display_gui() const
{
	return m_display_gui;
}

float Renderer::get_dc() const
{
	return m_dc;
}

float Renderer::get_l() const
{
    return m_l;
}

int Renderer::get_keyboard_layout() const
{
    return m_keyboard_layout;
}

//~ Setters

void Renderer::set_view_mode(const int mode)
{
    m_view_mode = mode;
}

void Renderer::set_display_gui(const bool mode)
{
	m_display_gui = mode;
}

void Renderer::set_keyboard_layout(const int layout)
{
    m_keyboard_layout = layout;
}
