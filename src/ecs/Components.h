#pragma once

#include <CNtity/Helper.hpp>
#include <ILog.h>
#include <array>
#include <vector>

#include "renderer/Vertex.h"
#include "renderer/ShaderLang.h"

namespace IRun {
	namespace ECS {
		typedef CNtity::Helper Helper;
		template <class Component>
		using Index = CNtity::Index<Component>;
		typedef CNtity::Entity Entity;
		template <class... Components>
		using View = CNtity::View<Components...>;

		using SerializedEntity = std::vector<std::pair<std::string, std::string>>;
		
		struct VertexData {
			std::vector<Vertex> data;

			inline std::string ToString() const {
				std::string finalString;

				for (size_t i = 0; i < data.size(); i++) {
					finalString += std::to_string(data[i].position.x) + " " + std::to_string(data[i].position.y) + " " + std::to_string(data[i].position.z) + " ";
				}

				return finalString;
			}

			inline void FromString(const std::string& values) {
				size_t offset = values.find_last_of(" ");

				if (offset == std::string::npos) {
					I_LOG_ERROR("Failed to de-serialize values of VertexData!");
					return;
				}

				std::istringstream dataSS{ values.substr(0, offset) };

				std::vector<float> tempVec{};

				std::copy(std::istream_iterator<float>(dataSS), std::istream_iterator<float>(), std::back_inserter(tempVec));

				for (int i = 0; i < tempVec.size(); i += 3) {
					std::vector<float> vec3;
					std::copy_n(tempVec.begin() + i, (i + 3), std::back_inserter(vec3));

					data.push_back({ { vec3[0], vec3[1], vec3[2] } });
				}
			}
		};

		struct IndexData {
			std::vector<uint32_t> data;

			inline std::string ToString() const {
				std::string finalString;

				for (uint32_t val: data) {
					finalString += std::to_string(val) + " ";
				}

				return finalString;
			}

			inline void FromString(const std::string& values) {
				size_t offset = values.find_last_of(" ");

				if (offset == std::string::npos) {
					I_LOG_ERROR("Failed to de-serialize values of VertexData!");
					return;
				}

				std::istringstream dataSS{ values.substr(0, offset) };

				std::copy(std::istream_iterator<uint32_t>(dataSS), std::istream_iterator<uint32_t>(), std::back_inserter(data));
			}
		};


		struct Shader {
			std::string vertexFilename, fragmentFilename;
			ShaderLanguage language;

			inline std::string ToString() const {
				return vertexFilename + " " + fragmentFilename + " " + ShaderLanguageToString(language);
			}

			inline void FromString(std::string& values) {
				std::stringstream ss{ values };
				ss >> values;
				vertexFilename = values;
				ss >> values;
				fragmentFilename = values;
				ss >> values;
				language = StringToShaderLanguage(values);
			}

			inline bool operator==(const Shader& shader) const {
				return (vertexFilename.compare(shader.vertexFilename) == 0) && (fragmentFilename.compare(shader.fragmentFilename) == 0) && language == shader.language;
			}

			struct HashFn {
				inline size_t operator() (const Shader& shader) const {
					size_t h1 = std::hash<std::string>()(shader.vertexFilename);
					size_t h2 = std::hash<std::string>()(shader.fragmentFilename);
					size_t h3 = std::hash<int64_t>()((int64_t)shader.language);

					return h1 ^ h2 ^ h3;
				}
			};
		};

		struct Transform {
			glm::vec3 position;
			glm::vec3 scale;
			/// degrees
			glm::vec3 rotation; 
		};

		template<typename ...Components >
		SerializedEntity Serialize(Helper& helper, const Entity& entity) {
			SerializedEntity serializedEntity{};

			helper.visit<Components...>(entity, [&helper, &serializedEntity](auto component, auto index)
				{
					if (index)
					{
						serializedEntity.push_back(std::make_pair(index.value(), component.ToString()));
					}
				});

			return serializedEntity;
		}

		template<typename ...Components >
		Entity Deserialize(CNtity::Helper& helper, const SerializedEntity& serializedEntity)
		{
			auto entity = helper.create();

			for (std::pair<std::string, std::string> serializedComponent: serializedEntity)
			{
				std::string& data = serializedComponent.second;
			    std::string& index = serializedComponent.first;
				helper.visit<Components...>(entity, index, [&data](auto& component)
					{
						component.FromString(data);
					});
			}

			return entity;
		}

	}
}