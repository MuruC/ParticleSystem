#include "GLCoreUtil.h"
#include <stdlib.h>
#include <random>
float GLCoreUtil::randomFloat()
{
	static std::mt19937 randomEngine;
	std::uniform_int_distribution<std::mt19937::result_type> distribution;
	randomEngine.seed(std::random_device()());
	return (float)distribution(randomEngine) / (float)std::numeric_limits<uint32_t>::max();
}