CXX = g++
CFLAGS = -W -Wall -ansi
LDFLAGS = -lSDL -lGL -lGLEW -lassimp
EXEC = 3DObs
#-L/usr/local/lib -lassimp
all:	$(EXEC)
	

$(EXEC): bin/Application.o bin/Object.o bin/Renderer.o bin/Camera.o bin/Rig.o bin/Framebuffer.o bin/stb_image.o bin/imgui.o bin/imguiRenderGL.o bin/main.o
	@echo "\033[33;33m \t Linking \033[m\017" 
	@$(CXX) -o $(EXEC) bin/Application.o bin/Object.o bin/Renderer.o bin/Camera.o bin/Rig.o bin/Framebuffer.o bin/stb_image.o bin/imgui.o bin/imguiRenderGL.o bin/main.o $(CFLAGS) $(LDFLAGS)
	@echo "\033[33;34m \t Done : type : ./3DObs to run \033[m\017"

bin/Application.o: src/Application.cpp include/Application.hpp
	@mkdir -p bin
	@echo "\033[33;32m \t Compiling" $< "\033[m\017" 
	@$(CXX) -c src/Application.cpp $(CFLAGS)
	@mv Application.o bin/

bin/Camera.o: src/Camera.cpp include/Camera.hpp
	@echo "\033[33;32m \t Compiling" $< "\033[m\017" 
	@$(CXX) -c src/Camera.cpp $(CFLAGS)
	@mv Camera.o bin/

bin/Object.o: src/Object.cpp include/Object.hpp
	@echo "\033[33;32m \t Compiling" $< "\033[m\017" 
	@$(CXX) -c src/Object.cpp $(CFLAGS)
	@mv Object.o bin/

bin/Renderer.o: src/Renderer.cpp include/Renderer.hpp
	@echo "\033[33;32m \t Compiling" $< "\033[m\017" 
	@$(CXX) -c src/Renderer.cpp $(CFLAGS)
	@mv Renderer.o bin/

bin/Rig.o: src/Rig.cpp include/Rig.hpp
	@echo "\033[33;32m \t Compiling" $< "\033[m\017" 
	@$(CXX) -c src/Rig.cpp $(CFLAGS)
	@mv Rig.o bin/

bin/stb_image.o: include/stb_image/stb_image.c include/stb_image/stb_image.h
	@echo "\033[33;32m \t Compiling" $< "\033[m\017" 
	@$(CXX) -c include/stb_image/stb_image.c $(CFLAGS) -Wno-missing-field-initializers -Wno-unused-but-set-variable
	@mv stb_image.o bin/
	
bin/imgui.o: include/imgui/imgui.cpp include/imgui/imgui.h
	@echo "\033[33;32m \t Compiling" $< "\033[m\017" 
	@$(CXX) -c include/imgui/imgui.cpp $(CFLAGS)
	@mv imgui.o bin/

bin/imguiRenderGL.o: include/imgui/imguiRenderGL.cpp include/imgui/imguiRenderGL.h
	@echo "\033[33;32m \t Compiling" $< "\033[m\017" 
	@$(CXX) -c include/imgui/imguiRenderGL.cpp $(CFLAGS) -Wno-unused-but-set-variable
	@mv imguiRenderGL.o bin/
	
bin/Framebuffer.o: src/Framebuffer.cpp include/Framebuffer.hpp
	@echo "\033[33;32m \t Compiling" $< "\033[m\017" 
	@$(CXX) -c src/Framebuffer.cpp $(CFLAGS)
	@mv Framebuffer.o bin/

bin/main.o: src/main.cpp
	@echo "\033[33;32m \t Compiling" $< "\033[m\017" 
	@$(CXX) -c src/main.cpp $(CFLAGS) -Wno-unused-parameter
	@mv main.o bin/

clean:
	@rm -rf bin/*.o
	@echo "\033[33;31m \t Cleaning '.o' \033[m\017" 

mrproper: clean
	@rm -rf $(EXEC)
	@echo "\033[33;31m \t Cleaning the executable \033[m\017" 
