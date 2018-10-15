#include "KeyFrameAnimation.hpp"
#include "Scene.hpp"
#include "read_chunk.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

void Magpie::KeyFrameAnimation::update(Scene *scene) {
	elapsed_frames += 1;
	
	// For each entry in the map, update the mesh
	for(auto iter = frames.begin(); iter != frames.end(); ++iter)
	{
		// Get the transform of the object
		Scene::Transform *trans = scene->look_up(iter->first);
		// Set the transform to be the difference between concurrent frames
		if (trans != nullptr) {
			//std::cout << "Updating " << trans->name << std::endl;
			//Magpie::KeyFrame difference = iter->second[elapsed_frames % iter->second.size()];
			Magpie::KeyFrame difference = Magpie::KeyFrame::get_difference(iter->second[(elapsed_frames + 1) % iter->second.size()], iter->second[elapsed_frames % iter->second.size()]);
			trans->position += difference.position;
			trans->rotation *= difference.rotation;
			trans->scale += difference.scale;
		}

	}
	
}

void Magpie::KeyFrameAnimation::load_model(std::string const &filename) {

	std::cout << filename << std::endl;

    std::ifstream file(filename, std::ios::binary);

	std::vector< char > names;
	read_chunk(file, "str0", &names);

    
	struct HierarchyEntry {
		uint32_t parent;
		uint32_t name_begin;
		uint32_t name_end;
        uint32_t frame;
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};
	static_assert(sizeof(HierarchyEntry) == 4 + 4 + 4 + 4 + 4*3 + 4*4 + 4*3, "HierarchyEntry is packed.");
	std::vector< HierarchyEntry > hierarchy;
	read_chunk(file, "xfh0", &hierarchy);

	
	struct MeshEntry {
		uint32_t transform;
		uint32_t name_begin;
		uint32_t name_end;
	};
	static_assert(sizeof(MeshEntry) == 4 + 4 + 4, "MeshEntry is packed.");
	std::vector< MeshEntry > meshes;
	read_chunk(file, "msh0", &meshes);

	if (file.peek() != EOF) {
		std::cerr << "WARNING: trailing data in scene file '" << filename << "'" << std::endl;
	}

	//--------------------------------
	//Now that file is loaded, create transforms for hierarchy entries:

	for (auto const &h : hierarchy) {
		// Sort out all of the key frames
		std::string name = "";
		if (h.name_begin <= h.name_end && h.name_end <= names.size()) {
			name = std::string(names.begin() + h.name_begin, names.begin() + h.name_end);
		} else {
			throw std::runtime_error("scene file '" + filename + "' contains hierarchy entry with invalid name indices");
		}
		// Create a new frame
		KeyFrame frame;
		frame.frame = h.frame;
		frame.position = h.position;
		frame.rotation = h.rotation;
		frame.scale = h.scale;
		
		// Add this frame to the frames map
		if (frames.count(name) > 0){
			// Append the keyframe
			frames[name].push_back(frame);
		} else {
			frames.insert({name, { frame }});
			current_key_frames.push_back(0);
		}
	}
}
