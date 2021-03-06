#include "timer.h"


////////////////////////////////////////////////////////////////////////////////
Timer::Timer() {
    tic();
}


////////////////////////////////////////////////////////////////////////////////
void Timer::tic() {
    then_ = std::chrono::high_resolution_clock::now();
}


////////////////////////////////////////////////////////////////////////////////
double Timer::tac()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now - then_;
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    const double seconds = micros / (double)1e6;

    return seconds;
}


////////////////////////////////////////////////////////////////////////////////
double Timer::toc() {
    const double dt = tac();
    std::cout << "tictoc: " << dt << "s" << std::endl;
    return dt;
}
