/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2012-2013 Alan Buckley   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef TBX_FIXED16_H_
#define TBX_FIXED16_H_

#include <ostream>
#include <istream>

namespace tbx
{
	/**
	 * Class to represent a fixed point number with 16bits
	 * before and after the point.
	 *
	 * There is no checking for range errors.
	 */
	class Fixed16
	{
	private:
		int _bits;

	public:
		Fixed16() {};
		Fixed16(const Fixed16 &other) {_bits = other._bits;}
		Fixed16(int value) {_bits = value<<16;}
		Fixed16(double value) {_bits = int(value * 65536.0);}

		operator int() const {return _bits>>16;}
		operator double() const {return double(_bits)/65536.0;}

		/**
		 * Get bits used to represent this number
		 */
		int bits() const {return _bits;}

		Fixed16 &operator=(const Fixed16 &other) {_bits = other._bits; return *this;}

		bool operator<(const Fixed16 &other) {return _bits < other._bits;}
		bool operator>(const Fixed16 &other) {return _bits > other._bits;}
		bool operator<=(const Fixed16 &other) {return _bits <= other._bits;}
		bool operator>=(const Fixed16 &other) {return _bits >= other._bits;}

		Fixed16 &operator++() {_bits += 65536;return *this;}
		Fixed16 &operator--() {_bits -= 65536;return *this;}
		Fixed16 operator++(int) {Fixed16 temp(*this); _bits+= 65536; return temp;}
		Fixed16 operator--(int) {Fixed16 temp(*this); _bits-= 65536; return temp;}

		Fixed16 &operator+=(const Fixed16 &other) {_bits += other._bits; return *this;}
		Fixed16 &operator-=(const Fixed16 &other) {_bits -= other._bits; return *this;}
		Fixed16 &operator*=(const Fixed16 &other) {_bits = (int)(((long long)_bits * (long long)other._bits) >> 16); return *this;}
		Fixed16 &operator/=(const Fixed16 &other) {_bits = (int)((((long long)_bits) << 16) / other._bits); return *this;}

		Fixed16 operator-() {Fixed16 temp;temp._bits = -_bits; return temp;}

		// Integer on RHS operators 
		Fixed16 &operator=(int value) {_bits = value << 16; return *this;}

		bool operator==(int value) const {return ((_bits & 0xFFFF) == 0) && ((_bits>>16) == value);}
		bool operator!=(int value) const {return !operator==(value);}
		bool operator<(int value) const {return (_bits>>16) < value;}
		bool operator>(int value) const {return ((_bits>>16) > value) || (((_bits>>16) == value) && ((_bits & 0xFFFF) != 0));}
		bool operator<=(int value) const {return !operator>(value);}
		bool operator>=(int value) const {return !operator<(value);}

		Fixed16 &operator+=(int value) {_bits += value<<16; return *this;}
		Fixed16 &operator-=(int value) {_bits -= value<<16; return *this;}
		Fixed16 &operator*=(int value) {_bits = (int)(((long long)_bits * (long long)value)); return *this;}
		Fixed16 &operator/=(int value) {_bits /= value; return *this;}

		// Double on RHS operators 
		Fixed16 &operator=(double value) {_bits = int(value * 65536.0); return *this;}

		bool operator==(double value) const {return double(_bits) / 65536.0 == value;}
		bool operator!=(double value) const {return !operator==(value);}
		bool operator<(double value) const {return double(_bits) / 65536.0 < value;}
		bool operator>(double value) const {return double(_bits) / 65536.0 > value;}
		bool operator<=(double value) const {return double(_bits) / 65536.0 <= value;}
		bool operator>=(double value) const {return double(_bits) / 65536.0 >= value;}

		Fixed16 &operator+=(double value) {_bits += int(value * 65536.0); return *this;}
		Fixed16 &operator-=(double value) {_bits -= int(value * 65536.0); return *this;}
		Fixed16 &operator*=(double value) {_bits = int(double(_bits) * value); return *this;}
		Fixed16 &operator/=(double value) {_bits = int(double(_bits) / value); return *this;}
	};

	inline bool operator==(const Fixed16 &lhs, const Fixed16 &rhs) {return lhs.bits() == rhs.bits();}
	inline bool operator!=(const Fixed16 &lhs, const Fixed16 &rhs) {return lhs.bits() != rhs.bits();}

	// Integer on LHS operator
	inline bool operator==(int lhs, const Fixed16 &rhs) {return rhs == lhs;}
	inline bool operator!=(int lhs, const Fixed16 &rhs) {return rhs != lhs;}
	inline bool operator<(int lhs, const Fixed16 &rhs) {return rhs >= lhs;}
	inline bool operator>(int lhs, const Fixed16 &rhs) {return rhs <= lhs;}
	inline bool operator<=(int lhs, const Fixed16 &rhs) {return rhs > lhs;}
	inline bool operator>=(int lhs, const Fixed16 &rhs) {return rhs < lhs;}

	inline Fixed16 &operator+=(int &lhs, const Fixed16 &rhs) {Fixed16 temp(rhs); return temp+=lhs;}
	inline Fixed16 &operator-=(int &lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp+=rhs;}
	inline Fixed16 &operator*=(int &lhs, const Fixed16 &rhs) {Fixed16 temp(rhs); return temp*=lhs;}
	inline Fixed16 &operator/=(int &lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp/=rhs;}

	inline Fixed16 operator+(const Fixed16 &lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp+=rhs;}
	inline Fixed16 operator-(const Fixed16 &lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp-=rhs;}
	inline Fixed16 operator*(const Fixed16 &lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp*=rhs;}
	inline Fixed16 operator/(const Fixed16 &lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp/=rhs;}

	inline Fixed16 operator+(int lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp+=rhs;}
    inline Fixed16 operator+(const Fixed16 &lhs, int rhs) {Fixed16 temp(lhs); return temp+=rhs;}
	inline Fixed16 operator-(int lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp-=rhs;}
    inline Fixed16 operator-(const Fixed16 &lhs, int rhs) {Fixed16 temp(lhs); return temp-=rhs;}
	inline Fixed16 operator*(int lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp*=rhs;}
    inline Fixed16 operator*(const Fixed16 &lhs, int rhs) {Fixed16 temp(lhs); return temp*=rhs;}
	inline Fixed16 operator/(int lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp/=rhs;}
    inline Fixed16 operator/(const Fixed16 &lhs, int rhs) {Fixed16 temp(lhs); return temp/=rhs;}

	// double on LHS operator
	inline bool operator==(double lhs, const Fixed16 &rhs) {return rhs == lhs;}
	inline bool operator!=(double lhs, const Fixed16 &rhs) {return rhs != lhs;}
	inline bool operator<(double lhs, const Fixed16 &rhs) {return rhs >= lhs;}
	inline bool operator>(double lhs, const Fixed16 &rhs) {return rhs <= lhs;}
	inline bool operator<=(double lhs, const Fixed16 &rhs) {return rhs > lhs;}
	inline bool operator>=(double lhs, const Fixed16 &rhs) {return rhs < lhs;}

	inline double &operator+=(double &lhs, const Fixed16 &rhs) {lhs += (double)rhs; return lhs;}
	inline double &operator-=(double &lhs, const Fixed16 &rhs) {lhs -= (double)rhs; return lhs;}
	inline double &operator*=(double &lhs, const Fixed16 &rhs) {lhs *= (double)rhs; return lhs;}
	inline double &operator/=(double &lhs, const Fixed16 &rhs) {lhs /= (double)rhs; return lhs;}

	inline Fixed16 operator+(double lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp+=rhs;}
    inline Fixed16 operator+(const Fixed16 &lhs, double rhs) {Fixed16 temp(lhs); return temp+=rhs;}
	inline Fixed16 operator-(double lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp-=rhs;}
    inline Fixed16 operator-(const Fixed16 &lhs, double rhs) {Fixed16 temp(lhs); return temp-=rhs;}
	inline Fixed16 operator*(double lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp*=rhs;}
    inline Fixed16 operator*(const Fixed16 &lhs, double rhs) {Fixed16 temp(lhs); return temp*=rhs;}
	inline Fixed16 operator/(double lhs, const Fixed16 &rhs) {Fixed16 temp(lhs); return temp/=rhs;}
    inline Fixed16 operator/(const Fixed16 &lhs, double rhs) {Fixed16 temp(lhs); return temp/=rhs;}

	inline std::ostream& operator<<(std::ostream &os, const Fixed16 &num) {os << double(num); return os;}
	inline std::istream& operator>>(std::istream &is, Fixed16 &num) {double val; is >> val; num = val; return is;}
};

#endif

