#ifndef __PROGTEST__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#include <array>
#include <iterator>
#include <set>
#include <list>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <compare>
#include <queue>
#include <stack>
#include <deque>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <stdexcept>
#include <condition_variable>
#include <pthread.h>
#include <semaphore.h>
#include "progtest_solver.h"
#include "sample_tester.h"

using namespace std;
#endif /* __PROGTEST__ */

//-------------------------------------------------------------------------------------------------------------------------------------------------------------



class COptimizer {
public:
    static bool usingProgtestSolver() {
        return true;
    }

    static void checkAlgorithmMin(APolygon p) {
        // dummy implementation if usingProgtestSolver() returns true
    }

    static void checkAlgorithmCnt(APolygon p) {
        // dummy implementation if usingProgtestSolver() returns true
    }

    COptimizer()
            : solverMin(createProgtestMinSolver(), true),
              solverCnt(createProgtestCntSolver(), false) {
    }

    void start(int threadCount);

    void stop();

    void addCompany(ACompany company);

    void producer(size_t id);

    void consumer(size_t id);

    void worker(size_t id);

private:
    struct problem {
        problem(APolygon ap, size_t id, size_t order)
                : polygon(ap), companyID(id), order(order) {}

        APolygon polygon;
        size_t companyID;
        size_t order;
        size_t polygonID = 0;
    };

    struct problemPack {
        problemPack(const AProblemPack &ppack, size_t tid, size_t order){
            this->pack = ppack;
            companyID = tid;
            shouldSolve = ppack->m_ProblemsCnt.size() + ppack->m_ProblemsMin.size();
            problemsSolved = 0;
            packOrder = order;
            solved = false;
        }

        AProblemPack pack;
        size_t companyID;
        size_t shouldSolve;
        size_t problemsSolved;
        size_t packOrder;
        bool solved;
    };

    struct Company {
        Company(ACompany ac)
                : com(ac) {}

        ACompany com;
        std::vector<problemPack> problems;
        size_t packsRecieved = 0;
        size_t packsSolved = 0;
        bool waiterEnded = false;
    };

    struct Solver {
        Solver(AProgtestSolver aps, bool isMin)
                : solvVar(std::move(aps)), isMin(isMin) {}

        AProgtestSolver solvVar;
        std::vector<problem> problems;
        bool isMin;
    };

    std::mutex mtx;
    std::condition_variable cv, cv2;
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    std::vector<std::thread> workers;

    std::vector<Company> companies;
    size_t mainCompanyID = 0;
    size_t waitersTotal = 0;
    size_t waitersEnded = 0;

    Solver solverMin;
    Solver solverCnt;
    std::deque<Solver> toSolve;
};

void COptimizer::producer(size_t id) {
    //printf("Wait id%zu: waiting..................\n", id);
    AProblemPack pack = companies[id].com->waitForPack();
    while (pack) {
        size_t polygon_id = 0;
        //std::unique_lock<std::mutex> ul(mtx);
        mtx.lock();
        problemPack pr(pack, id, companies[id].packsRecieved);
        companies[id].problems.push_back(pr);

        for (const auto &polygonMin: pack->m_ProblemsMin) {
            solverMin.solvVar->addPolygon(polygonMin);
            problem p(polygonMin, id, companies[id].packsRecieved);
            //printf("Wait id%zu polygonMin_points:{%zu}, packOrder:%zu, full:%u, polyID:%zu \n", id, polygonMin->m_Points.size(),companies[id].packsRecieved, !solverMin.solvVar->hasFreeCapacity(), polygon_id);
            p.polygonID = polygon_id++;
            solverMin.problems.emplace_back(p);
            if (!solverMin.solvVar->hasFreeCapacity()) {
                toSolve.emplace_back(solverMin);
                //printf("Wait id%zu : notify min\n", id);
                solverMin.solvVar = createProgtestMinSolver();
                solverMin.problems.clear();
                cv.notify_one();
            }
        }
        //printf("Wait id%zu ========================================\n", id);

        for (const auto &polygonCnt: pack->m_ProblemsCnt) {
            solverCnt.solvVar->addPolygon(polygonCnt);
            problem p(polygonCnt, id, companies[id].packsRecieved);
            //printf("Wait id%zu polygonCnt_points:{%zu}, packOrder:%zu,full:%u, polyID:%zu \n", id, polygonCnt->m_Points.size(),companies[id].packsRecieved, !solverCnt.solvVar->hasFreeCapacity(), polygon_id);
            p.polygonID = polygon_id++;
            solverCnt.problems.push_back(p);
            if (!solverCnt.solvVar->hasFreeCapacity()) {
                toSolve.push_back(solverCnt);
                //printf("wait id%zu : notify cnt\n", id);
                solverCnt.solvVar = createProgtestCntSolver();
                solverCnt.problems.clear();
                cv.notify_one();
            }
        }
        companies[id].packsRecieved++;
        //printf("Wait id%zu: waiting.........................\n", id);
        mtx.unlock();
        pack = companies[id].com->waitForPack();
    }
    std::unique_lock<std::mutex> ul2(mtx);
    //printf("Wait %zu: pack was null\n", id);
    waitersEnded++;
    companies[id].waiterEnded = true;
    ul2.unlock();
    cv.notify_all();
    cv2.notify_all();
    //printf("Wait %zu: END\n", id);
}

void COptimizer::worker(size_t id) {
    //printf("Work %zu: waiting\n", id);
    //printf("Work %zu: stopped waiting\n", id);
    while (true) {
        std::unique_lock<std::mutex> ul(mtx);
        cv.wait(ul, [this, id]() {
            return !toSolve.empty() || (waitersEnded == waitersTotal && toSolve.empty());
        });
        //printf("Work %zu: stopped waiting (%d)\n", id,(waitersEnded == waitersTotal && toSolve.empty()));

        if(waitersEnded == waitersTotal && toSolve.empty() && solverCnt.problems.empty() && solverMin.problems.empty()){
            //printf("Work %zu: waiters stopped, nothing to solve\n", id);
            break;
        }
        else if(waitersEnded == waitersTotal && toSolve.empty()){
            if(!solverCnt.problems.empty()){
                toSolve.push_back(solverCnt);
                solverCnt.problems.clear();
                auto elem = toSolve.front();
                toSolve.pop_front();
                cv.notify_all();

                //printf("Work %zu: solving..................\n", id);
                ul.unlock();
                elem.solvVar->solve();
                ul.lock();
                for (auto &problem: elem.problems) {
                    companies[problem.companyID].problems[problem.order].problemsSolved++;
                    //printf("work %zu: packOrder: %zu, polyID:%zu, compID:%zu\n", id, problem.order, problem.polygonID, problem.companyID);
                    if ( companies[problem.companyID].problems[problem.order].problemsSolved == companies[problem.companyID].problems[problem.order].shouldSolve) {
                        companies[problem.companyID].problems[problem.order].solved = true;
                        //printf("Work %zu: pack for comp:%zu order:%zu solved\n", id, problem.companyID, problem.order);
                        cv2.notify_all();
                    }
                }
                //printf("Work %zu: solved solver.........................\n", id);
            }
            if(!solverMin.problems.empty()){
                toSolve.push_back(solverMin);
                //printf("Work %zu: notify min, rest to solve\n", id);
                solverMin.problems.clear();
                auto elem = toSolve.front();
                toSolve.pop_front();
                cv.notify_all();

                //printf("Work %zu: solving..................\n", id);
                ul.unlock();
                elem.solvVar->solve();
                ul.lock();
                for (auto &problem: elem.problems) {
                    companies[problem.companyID].problems[problem.order].problemsSolved++;
                    //printf("work %zu: packOrder: %zu, polyID:%zu, compID:%zu\n", id, problem.order, problem.polygonID, problem.companyID);
                    if ( companies[problem.companyID].problems[problem.order].problemsSolved == companies[problem.companyID].problems[problem.order].shouldSolve) {
                        companies[problem.companyID].problems[problem.order].solved = true;
                        //printf("Work %zu: pack for comp:%zu order:%zu solved\n", id, problem.companyID, problem.order);
                        cv2.notify_all();
                    }
                }
                //printf("Work %zu: solved solver.........................\n", id);
            }
            break;
        }
        if(toSolve.empty())
            break;
        auto elem = toSolve.front();
        toSolve.pop_front();
        cv.notify_all();

        //printf("Work %zu: solving..................\n", id);
        ul.unlock();
        elem.solvVar->solve();
        ul.lock();
        for (auto &problem: elem.problems) {
            companies[problem.companyID].problems[problem.order].problemsSolved++;
            //printf("work %zu: packOrder: %zu, polyID:%zu, compID:%zu\n", id, problem.order, problem.polygonID, problem.companyID);
            //printf("%zu == %zu\n", companies[problem.companyID].problems[problem.order].problemsSolved, companies[problem.companyID].problems[problem.order].shouldSolve);
            if ( companies[problem.companyID].problems[problem.order].problemsSolved == companies[problem.companyID].problems[problem.order].shouldSolve) {
                companies[problem.companyID].problems[problem.order].solved = true;
                //printf("Work %zu: pack for comp:%zu order:%zu solved\n", id, problem.companyID, problem.order);
                cv2.notify_all();
            }
        }
        //printf("Work %zu: solved solver.........................\n", id);

    }
    //printf("Work %zu: end\n", id);
}

void COptimizer::consumer(size_t id) {

//    if(companies[id].waiterEnded && companies[id].packsRecieved == 0){
//        //printf("Solve %zu: END no pack\n", id);
//        return;
//    }
    //printf("Solve %zu: notified(smth solved), go to be solved problem:%zu\n", id, companies[id].packsSolved);
    while (true) {
        std::unique_lock<std::mutex> ul(mtx);
        //printf("Solve %zu: waiting for pack:%zu. to be solved\n", id, companies[id].packsSolved);
        cv2.wait(ul, [this, id]() {
            return (companies[id].packsSolved < companies[id].packsRecieved &&
                    companies[id].problems[companies[id].packsSolved].solved) ||
                   (companies[id].packsSolved >= companies[id].packsRecieved &&
                    companies[id].waiterEnded);
        });
        if(companies[id].packsSolved >= companies[id].packsRecieved && companies[id].waiterEnded)
            break;
        AProblemPack pack = companies[id].problems[companies[id].packsSolved].pack;
        ul.unlock();
        companies[id].com->solvedPack(pack);
        ul.lock();
        companies[id].packsSolved++;
        if(companies[id].packsSolved >= companies[id].packsRecieved && companies[id].waiterEnded)
            break;
        cv2.notify_all();
        //printf("Solve %zu: waiting for pack:%zu. to be solved\n", id, companies[id].packsSolved);

        //printf("Solve %zu: notified pack:%zu. solved, go to be solved\n", id, companies[id].packsSolved);
    }
    //printf("Solve %zu: END\n", id);
}

void COptimizer::start(int threadCount) {
    for (int i = 0; i < threadCount; i++)
        workers.emplace_back(&COptimizer::worker, this, i);
}

void COptimizer::stop() {
    for (auto& p: producers) {
        p.join();
    }
    for (auto &w: workers) {
        w.join();
    }
    for (auto &c: consumers)
        c.join();
}

void COptimizer::addCompany(ACompany company) {
    waitersTotal++;
    companies.push_back(company);
    producers.emplace_back(&COptimizer::producer, this, mainCompanyID);
    consumers.emplace_back(&COptimizer::consumer, this, mainCompanyID);
    //printf("company: ID%zu\n", mainCompanyID);
    mainCompanyID++;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __PROGTEST__

int main() {
    g_IToldYouNmIsUseful = 1000000000;
    COptimizer optimizer;
    std::vector<ACompanyTest> v;
    //ACompanyTest company = std::make_shared<CCompanyTest>();
    //ACompanyTest company1 = std::make_shared<CCompanyTest>();
    ACompanyTest company;
    for (int i = 0; i < 20; ++i) {
        v.push_back(std::make_shared<CCompanyTest>());
        optimizer.addCompany(v[i]);
    }
    //optimizer.addCompany(company);
    //optimizer.addCompany((company1));
    optimizer.start(20);
    optimizer.stop();
    for (const auto& x: v) {
        if (!x->allProcessed())
            throw std::logic_error("(some) problems were not correctly processsed");
    }
    return 0;
}

#endif /* __PROGTEST__ */
