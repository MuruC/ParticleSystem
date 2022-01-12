#pragma once

namespace GLCore
{
	class TimeStep
	{
	public:
		TimeStep(float t = 0.0f) : time(t)
		{
		}
		float getSecond() const{ return time; }
		float getMilliseconds() const { return time * 1000.0f; }
	private:
		float time = 0.0f;
	};
}

/*
namespace GLCore {

	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: m_Time(time)
		{
		}

		operator float() const { return m_Time; }

		float GetSeconds() const { return m_Time; }
		float GetMilliseconds() const { return m_Time * 1000.0f; }
	private:
		float m_Time;
	};

}
*/