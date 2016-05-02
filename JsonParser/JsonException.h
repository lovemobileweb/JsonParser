
#ifndef JSON_EX_H
#define JSON_EX_H

namespace JSON
{

class json_exception : public std::exception
{
public:

	json_exception(const char* arg) :
		std::exception(arg)
	{
	}
	
	// 
    virtual ~json_exception() throw()	{}

    // Returns a C-style character string describing 
	// the general cause of the current error.
	 virtual const char* what() const throw()
	 {
		return std::exception::what();
	 }
};	// class

}	//namespace

#endif
