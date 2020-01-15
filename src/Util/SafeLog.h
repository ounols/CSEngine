class SafeLog {
private:
	SafeLog() = default;
	~SafeLog() = default;

public:
	static void Log(const char* log);
};