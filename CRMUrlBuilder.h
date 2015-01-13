#include "iexecuter.h"
#include "Parser.h"

#include <string>

class CRMUrlBuilder: public ExecuterInterface
{
		Parser parser;
		int SendRequest(std::string url);
	public:
		CRMUrlBuilder(Parser parsertostr):parser(parsertostr){};
		virtual int Execute(ParamMap& data);
};