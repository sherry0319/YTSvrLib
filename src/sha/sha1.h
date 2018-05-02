/*
    sha1.hpp - header of

    ============
    SHA-1 in C++
    ============

    100% Public Domain.

    Original C Code
        -- Steve Reid <steve@edmweb.com>
    Small changes to fit into bglibs
        -- Bruce Guenter <bruce@untroubled.org>
    Translation to simpler C++ Code
        -- Volker Grabsch <vog@notjusthosting.com>
    Safety fixes
        -- Eugene Hopkinson <slowriot at voxelstorm dot com>
*/

#ifndef SHA1_HPP
#define SHA1_HPP


#include <cstdint>
#include <iostream>
#include <string>

namespace YTSvrLib
{
	class SHA1
	{
	public:
		SHA1();
		void update(const std::string &s);
		void update(std::istream &is);
		std::string finalize();
		void finalize(uint8_t out[20]);
		static std::string from_file(const std::string &filename);

	private:
		uint32_t digest[5];
		std::string buffer;
		uint64_t transforms;
	};

	YTSVRLIB_EXPORT std::string SHA1HASH(const std::string &s);

	YTSVRLIB_EXPORT void SHA1HASH(const std::string &s, uint8_t out[20]);
}

#endif /* SHA1_HPP */
