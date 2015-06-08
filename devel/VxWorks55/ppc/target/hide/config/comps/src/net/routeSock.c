void routeSockInit()
    {
    if (sockLibAdd ((FUNCPTR) bsdSockLibInit, AF_ROUTE, AF_ROUTE) == ERROR)
        return (ERROR);

    routeSockLibInit () ;
    }
