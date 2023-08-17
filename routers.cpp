#include "routers.h"
#include <iostream>
#include <memory>

using namespace std;

CRouteBest CRoutes::buildRouter(string strPointDepart, string strDestin, VecTripFare vecTripFare)
{
    CRouteBest routeBest;
    int iCount = 0;

    m_vecTripFare = vecTripFare;
    m_strDestin = strDestin;
    this->m_strPointDepart = strPointDepart;
    this->m_strDestin = strDestin;

    int iRes = 0;
    int iBeg = 0;

    while(true)
    {
        iRes = findNextEx(iBeg, strPointDepart);
        if(iRes != -1){
            iCount++;
            iRes++;
            iBeg = iRes;
        }
        else
          break;
    }

    //cout << "count routers from departure point = " << iCount << endl;

    if(iCount == 0)
    {
        routeBest.vecPoint.clear();
        routeBest.flFare = 0.0;
        return routeBest;
    }
    // first iteration, create routers  -------------------
    if(m_bFirstIter)
    {
        m_pVecRoutes = new VecRoutes();
        iRes = 0;
        iBeg = 0;

        while(true)
        {
            iRes = findNextEx(iBeg, strPointDepart);
            if(iRes != -1){
                unique_ptr<CTripAndFare> pTripAndFare(new CTripAndFare(m_vecTripFare[iRes]));
                unique_ptr<CRoute> pRoute(new CRoute(move(*pTripAndFare)));
                pRoute->setState(EState::BUILT);
                m_pVecRoutes->push_back(move(*pRoute));
                iRes++;
                iBeg = iRes;
            }
            else
                break;
        }
    }

//---------------------------------------------------------
    float flFareComm = 1000000.0;
    int iIter = 0;
    int iNum = 0;

    VecRoutes vecRoutesFin;
    bool bRes = checkForFinish();
    printRoutes();

    if(bRes)
    {
        vecRoutesFin = calculateFare();
        if(vecRoutesFin.size() == 0)
        {
            routeBest.vecPoint.clear();
            routeBest.flFare = 0.0;
        }
        else
        {
            for(auto& vecRoute: vecRoutesFin)
            {
                float flValue = vecRoute.getFare();
                if(flValue < flFareComm){
                    flFareComm = flValue;
                    iNum = iIter;
                }
                iIter++;
            }
            routeBest.vecPoint.clear();
            CRoute routeFin = vecRoutesFin[iNum];
            VecRoute vecRoute = routeFin.getVecRoute();

            for(auto& trip: vecRoute)
            {
                routeBest.vecPoint.push_back(trip.getPointDepart());
            }
            size_t uSize = vecRoute.size();
            routeBest.vecPoint.push_back(vecRoute[--uSize].getDestinat());
            routeBest.flFare = routeFin.getFare();
            return routeBest;
        }
    }
    else
    {
        routeBest.vecPoint.clear();
        routeBest.flFare = 0.0;
    }

    return routeBest;
}
// Print routes ------------------------------------------
void CRoutes::printRoutes()
{
    for(size_t i = 0; i < m_pVecRoutes->size(); i++)
    {
        cout << "  *** route: " << endl;
        CRoute router((*m_pVecRoutes)[i]);  // using VecRoute = vector<CTripAndFare>;
        for(auto& val: router.getVecRoute())
        {
            cout << val.m_strPointDepatr << '\t' << val.m_strDestinat << '\t' << val.m_flFare << endl;
        }
    }
    // End print route ------------------------------------
}
// Print route best ---------------------------------------
void CRoutes::printBestRoutes(CRouteBest& routeBest){
    for(auto& val: routeBest.vecPoint)
    {
        cout << val << " => ";
    }
    cout << routeBest.flFare << endl;
}
// calculate routers fare ---------------------------------
VecRoutes&& CRoutes::calculateFare()
{
    VecRoutes *pVecRoutesRes;
    VecRoutes::iterator iterRoute;
    VecTripFare::iterator iterTrip;
    TripAndFare trip;
    string strPoint, strDest;

    pVecRoutesRes = new VecRoutes;

    for(iterRoute = m_pVecRoutes->begin(); iterRoute != m_pVecRoutes->end(); iterRoute++)
    {
        if(iterRoute->getState() == EState::BUILTWELL)
        {
            iterRoute->calcFareComm();
            pVecRoutesRes->push_back(*iterRoute);
        }
    }
    return move(*pVecRoutesRes);
}
// check for finish ---------------------------------------
bool CRoutes::checkForFinish()
{
    bool fFinish = false;
    size_t uCountTrip = m_vecTripFare.size();
    int iCount;

    while(!fFinish)
    {
        buildRouterNextIter();
        iCount = 0;
        // check for finish
        for(auto& route: *m_pVecRoutes)
        {
            VecRoute& vecRoute = route.getVecRoute();
            CTripAndFare& tripLast = vecRoute.back();
            if(vecRoute.size() > uCountTrip)
                route.setState(EState::NOEXIST);

            string strDest = tripLast.getDestinat();
            if(!this->m_strDestin.compare(strDest))
                route.setState(EState::BUILTWELL);

            if(route.getState() == EState::BUILT)
                iCount++;
        }
        if(!iCount){
            fFinish = true;
            break;
        }
    }
    return fFinish;
}
// second and larger iteration, add new trips for router --
void CRoutes::buildRouterNextIter()
{
    VecTripFare::iterator iter, iterOut;
    VecRoutes *pVecRoutesTemp = nullptr;
    VecRoutes::iterator iterRoute;
    CTripAndFare *pTripAndFare;

    pVecRoutesTemp = new VecRoutes();

    for(iterRoute = m_pVecRoutes->begin(); iterRoute != m_pVecRoutes->end(); iterRoute++)
    {
        int iCount = 0;
        CRoute *pRouteTemp;
        CRoute routeTmp = *iterRoute;
        VecRoute& vecRouteLoc = routeTmp.getVecRoute();
        CTripAndFare& tripTmp = vecRouteLoc.back();
        string strDest = tripTmp.getDestinat();
        int iRes = 0;
        int iBeg = 0;

        while(true)
        {
            iRes = findNextEx(iBeg, strDest);
            if(iRes != -1){
                pTripAndFare = new CTripAndFare(m_vecTripFare[iRes]);
                pRouteTemp = new CRoute(routeTmp);
                pRouteTemp->addTrip(move(*pTripAndFare));
                pRouteTemp->setState(EState::BUILT);
                pVecRoutesTemp->push_back(move(*pRouteTemp));
                iCount++;
                iRes++;
                iBeg = iRes;
            }
            else
                break;
        }
        if(!iCount)
        {
            pRouteTemp = new CRoute(routeTmp);
            pRouteTemp->setState(EState::NOEXIST);
            pVecRoutesTemp->push_back(move(*pRouteTemp));
        }
    }

    delete this->m_pVecRoutes;
    this->m_pVecRoutes = pVecRoutesTemp;
}
//---------------------------------------------------------
int CRoutes::findNextEx(int iIndBeg, string str)
{
    int iSize = m_vecTripFare.size();
    string strTmp;
    int i;

    if(iIndBeg >= iSize ) return -1;

    for(i = iIndBeg; i < iSize; i++)
    {
        strTmp = m_vecTripFare[i].m_strPointDepatr;
        if(!str.compare( strTmp )){
            return i;
        }
    }
    return -1;
}
