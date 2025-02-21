
#include <iostream>
#include <queue>


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