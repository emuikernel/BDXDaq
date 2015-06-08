#include "smiprocessing.h"


void Domain::smiDomainHandler()
{
    int nobjs;
    cout << "Domain " << getName();
    if((nobjs = getNObjects()))
    {
        cout << " is Up (" << nobjs << " objects)" << endl;
//        while(getNextObject()) { ...};
    }
    else
    {
        cout << " is Down" << endl;
    }
}

void Domain::smiStateChangeHandler()
{
    SmiObject *objp = getObject();
    char *name, *state;

    name = objp->getName();
    state = objp->getState();
    if(name)
    {
        if(state)
            cout << name << " in state " << objp->getState() << endl;
        else
            cout << name << " is dead" << endl;
    }
}

void Domain::smiExecutingHandler()
{
    SmiObject *objp = getObject();
    char *name;

    name = objp->getName();
    if(name)
    {
        if(objp->getBusy())
        {
            cout << name << " executing " << objp->getActionInProgress() << endl;
        }
    }
}

void Domain::smiObjSetChangeHandler()
{
    SmiObjectSet *objsetp = getObjectSet();
    char *name;
    int nsetobjs;

    cout << "ObjectSet " << objsetp->getName();
    if ((nsetobjs = objsetp->getNObjects()))
    {
        cout << " Contains " << nsetobjs << " objects" << endl;
    }
    else
    {
        cout << " is Empty" << endl;
    }
}

int main_smiprocessing(int argc, char *argv[])
{

    Domain *domainp;

    domainp = new Domain(argv[1]);

    while(1)
        pause();
}
