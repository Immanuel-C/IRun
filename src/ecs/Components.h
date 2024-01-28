#pragma once

#include <CNtity/Helper.hpp>
#include <ILog.h>
#include <array>
#include <vector>

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
			std::vector<float> data;

			inline std::string ToString() const {
				std::string finalString;

				for (size_t i = 0; i < data.size(); i++) {
					finalString += std::to_string(data[i]) + " ";
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

				std::copy(std::istream_iterator<float>(dataSS), std::istream_iterator<float>(), std::back_inserter(data));
			}

		};

		struct Shader {
			std::string vertexFilename, fragmentFilename;

			inline std::string ToString() const {
				return vertexFilename + " " + fragmentFilename;
			}

			inline void FromString(std::string& values) {
				std::stringstream ss{ values };
				ss >> values;
				vertexFilename = values;
				ss >> values;
				fragmentFilename = values;

			}
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