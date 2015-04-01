#include <string>
#include <map>

#include "datatypes.h"

using namespace std;

using ParserData = map < string, string > ;

class IParser
{
	public:
		virtual string parsedata(ParamMap& data) = 0;
};