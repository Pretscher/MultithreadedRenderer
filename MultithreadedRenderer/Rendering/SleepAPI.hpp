class SleepAPI {
private:
    void* timer;
public:
    SleepAPI();
    ~SleepAPI();
    /* Windows sleep in accurate milliseconds */
    void millisleep(long long ms);
};