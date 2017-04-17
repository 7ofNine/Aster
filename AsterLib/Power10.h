//
// a cahce to keep the powers of ten needed in
//
#pragma once
#include <unordered_map>

class Power10
{
public:
	Power10();
	~Power10();
	long double operator()(int const power);

private:
	inline long double value(int const power);
	typedef std::unordered_map<int, long double> CacheType;
	typedef CacheType::iterator RefElement;
	static  CacheType cache;
};

