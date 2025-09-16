#pragma once

#include <glm/glm.hpp>

typedef glm::vec4 Color;

#define BLUE Color(0.1f, 0.3f, 0.9f, 1.f)
#define CYAN Color(0.1f, 0.9f, 0.9f, 1.f)
#define RED Color(0.9f, 0.3f, 0.1f, 1.f)
#define GREEN Color(0.1f, 0.9f, 0.3f, 1.f)
#define LIME Color(0.5f, 1.f, 0.f, 1.f)
#define YELLOW Color(0.8f, 0.8f, 0.2f, 1.f)
#define ORANGE Color(1.f, 0.5f, 0.f, 1.f)
#define PURPLE Color(0.5f, 0.1f, 0.8f, 1.f)
#define BLACK Color(0.f, 0.f, 0.f, 1.f)
#define WHITE Color(1.f)
#define GREY Color(0.5f, 0.5f, 0.5f, 1.f)

#define TRANSPARENCY(transparency) Color(0.f, 0.f, 0.f, -transparency)