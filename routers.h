#ifndef ROUTERS_H_INCLUDED
#define ROUTERS_H_INCLUDED

#include <string>
#include <vector>
#include <utility>

using namespace std;

struct TripAndFare
{
    string m_strPointDepatr;
    string m_strDestinat;
    float m_flFare;
};

using VecTripFare = vector<TripAndFare>;

class CTripAndFare: public TripAndFare
{
public:
    CTripAndFare(TripAndFare trip): TripAndFare(trip){};
    CTripAndFare(string strDest, string strPointDepatr, float flFare):
        TripAndFare({strDest,strPointDepatr,flFare})
        {}
    string getPointDepart() const { return TripAndFare::m_strPointDepatr; }
    string getDestinat() const { return TripAndFare::m_strDestinat; }
    float getFare() const { return TripAndFare::m_flFare; }
    bool isFromThisPointDepart(string str)
    {
        return !str.compare(m_strPointDepatr);
    }
};

using VecRoute = vector<CTripAndFare>;

enum class EState
{
    BUILT,      // the route is being built
    BUILTWELL,  // the route was built well
    NOEXIST     // the router does not exist
};

struct CRouteBest
{
    vector<string> vecPoint;
    float flFare;
};

class CRoute
{
    EState m_eState;
    float m_flFareCommon;
    VecRoute m_vecRoute;
    string m_strDestin;
public:
    float getFare(){ return m_flFareCommon; }
    VecRoute& getVecRoute(){ return m_vecRoute;}
    EState getState() { return m_eState; }
    void setState(EState eState) { m_eState = eState; }

    CRoute(CTripAndFare tripData)
    {
        m_vecRoute.push_back(tripData);
    };

    void addTrip(CTripAndFare tripData)
    {
        m_vecRoute.push_back(tripData);
    }

    float calcFareComm()
    {
        float flRes = 0;
        for(auto& val: m_vecRoute){
            flRes += val.getFare();
        }
        m_flFareCommon = flRes;
        return flRes;
    }
};

using VecRoutes = vector<CRoute>;

class CRoutes
{
    VecRoutes *m_pVecRoutes = nullptr;
    VecTripFare m_vecTripFare;
    string m_strPointDepart;
    string m_strDestin;
    //int m_iCounter;
    bool m_bFirstIter = true;

    void buildRouterNextIter();
    VecRoutes&& calculateFare();
    bool checkForFinish();
    int findNextEx(int iIndBeg, string str);

    VecRoutes *getVecRoutes() { return m_pVecRoutes; }
    void printRoutes();
public:
    ~CRoutes(){
        if(m_pVecRoutes)
          delete m_pVecRoutes;
    }
    CRouteBest buildRouter(string strPointDepart, string strDestin, VecTripFare vecTripFare);
    void printBestRoutes(CRouteBest& routeBest);
};

#endif // ROUTERS_H_INCLUDED
