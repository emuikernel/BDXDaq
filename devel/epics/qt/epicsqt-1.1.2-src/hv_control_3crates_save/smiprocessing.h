#include <smiuirtl.hxx>

class Domain: public SmiDomain
{
    void smiDomainHandler();
    void smiStateChangeHandler();
    void smiExecutingHandler();
    void smiObjSetChangeHandler();
public:
    Domain(char *name): SmiDomain(name) {};
};

