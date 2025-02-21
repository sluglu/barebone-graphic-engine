
#include <iostream>
#include <queue>


struct fixedTime {
	private :
		double simTime;
		double simTimeStep;

	public :
		void restartSimTimer() {
			simTime = 0.0;
		}
	
		double getSimTime()
		{
			return simTime;
		}
	
		void incrementSimTimer()
		{
			simTime += simTimeStep;
		}
	
		double getSimTimeStep()
		{
			return simTimeStep;
		}
	
		void setSimTimeStep(double step)
		{
			simTimeStep = step;
		}
};

struct Time {
	int nanoseconds;
	int microseconds;
	int milliseconds;
	int seconds;
	int minutes;
	int hours;
	int days;
	int months;
	int years;

	Time() {

		auto now = std::chrono::system_clock::now().time_since_epoch();
		auto now_high_resolution = std::chrono::high_resolution_clock::now().time_since_epoch();

		nanoseconds = now.count();
		microseconds = nanoseconds / 1000;
		milliseconds = microseconds / 1000;
		seconds = milliseconds / 1000;
		minutes = seconds / 60;
		hours = minutes / 60;
		days = hours / 24;
		months = days / 31;
		years = days / 365;
	}
};

Time getAbsoluteTime() {
	return Time();
}

class MovingAverage {
private:
    std::queue<double> window;
    int windowSize;
    double sum;

public:
    MovingAverage(int size) : windowSize(size), sum(0) {}

	double getValue() {
		return sum / window.size();
	}

    double addValue(double value) {
        sum += value;
        window.push(value);

        // Remove oldest value if window exceeds the limit
        if (window.size() > windowSize) {
            sum -= window.front();
            window.pop();
        }

        return sum / window.size(); // Return current moving average
    }
};