#include "Configuration.h"
Configuration configuration = Configuration();

glm::mat4 Configuration::GetScreenMatrix()
{
    return glm::mat4(
        float(resolution.width) / 2.0, 0, 0, 0,
        0, float(-resolution.height) / 2.0, 0, 0,
        0, 0, 1, 0,
		float(resolution.width) / 2.0, float(resolution.height) / 2.0, 1, 1
    );
}

Configuration::Configuration()
{
	this->colorBuffer = new Buffer<Color>(800, 450);
}
