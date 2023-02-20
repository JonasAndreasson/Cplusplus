#include <ctime>  // time and localtime
#include <iostream>
#include <string>
#include "date.h"

int Date::daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

Date::Date() {
	time_t timer = time(0); // time in seconds since 1970-01-01
	tm* locTime = localtime(&timer); // broken-down time
	year = 1900 + locTime->tm_year;
	month = 1 + locTime->tm_mon;
	day = locTime->tm_mday;
}

Date::Date(int y, int m, int d) {
	year = y;
	month = m;
	day = d;
}

int Date::getYear() const {
	return 0;
}

int Date::getMonth() const {
	return 0;
}

int Date::getDay() const {
	return 0;
}

void Date::next() {
}

std::ostream& operator<<(std::ostream& os, Date& dt){
	os << dt.year <<"-";
	if (dt.month<10){
		os << "0"<< dt.month;
	} else {
		os << dt.month;
	}
	os << "-";
	if (dt.day<10){
	os << "0"<< dt.day;
	} else {
		os << dt.day;
	}
	os << std::endl;
	return os;
}
Date& operator>>(std::istream& is, Date& dt){
	std::string year;
	std::getline(is, year, '-');
	if (year.size()!=4){
		is.setstate(std::ios_base::failbit);
		return dt;
	}
	std::string month;
	std::getline(is, month, '-');
	if (month.size()!=2 || stoi(month)>12 || stoi(month)<1){
		is.setstate(std::ios_base::failbit);
		return dt;
	}
	std::string day;
	std::getline(is, day);	
	if (day.size()!=2 || std::stoi(day)>Date::daysPerMonth[stoi(month)-1] || stoi(day)<1){
		is.setstate(std::ios_base::failbit);
		return dt;
	}
	dt.day = stoi(day);
	dt.month = stoi(month);
	dt.year = stoi(year);
	return dt;
}

