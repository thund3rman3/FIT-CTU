#ifndef PROGTEST_SOLVER_H_9523464567576456
#define PROGTEST_SOLVER_H_9523464567576456

#include <vector>
#include <memory>
#include "common.h"

extern uint64_t g_IToldYouNmIsUseful;

//=============================================================================================================================================================
/**
 * Problem solver provided by the progtest environment. Solves a batch of problems. There are basically two kinds of
 * solvers: a solver solves the minimal triangulation (TriangMin problems), or a solvers that solves the number of
 * triangulations (TriangCnt problems). The solvers are technically subclasses of the base CProgtestSolver class, the
 * factory function creates either a subclass to solve the TriangMin problems, or it creates a subclass to slve the
 * TriangCnt problems.
 *
 * Each instance of the solver has a certain capacity - the solver solves a batch of problems at most of that size.
 * You are expected to fill the solver with CPolygon instances, you may add any number of instances smaller than or
 * equal to the capacity. Subsequently, you are expected to call the solve() method to start the computation. The
 * solver processes all stored problems and fills the corresponding m_TriangMin / m_TriangCnt fields in the solved
 * problems.
 *
 * If you need to solve further problems, you are free to create further CProgtestSolver instances. Moreover, you may
 * call solve () at any moment before the capacity of a CProgtestSolver is exceeded. That is, you may even add just
 * one CPolygon instance and call solve ().
 *
 * There are, however, some pitfalls. The progtest solver is a bit moody. It is aware of the test data and it knows
 * there will be N problems in the test. Moreover, it decides to create only k useful instances (k is a certain integer).
 * The created instances set their capacities to m1, m2, m3, ..., mk. The total sum is M = m1+m2+m3+ ... + mk and the
 * solver guarantees that M is equal to N or a bit greater than N.
 *
 * In other words, the solver is able to solve all problems in the test, however, you may not waste the capacity of
 * the solver instances. If you do so, you may run out of the total solver capacity.
 *
 * As stated above, the solver only creates k useful instances. If you want to create more than k instances, the
 * subsequent calls to createProgtestSolver() will return an empty smart pointer, a solved that fills-in invalid
 * result, or a solver with zero capacity (as stated above, the solver is moody/playful). Either way, the extra solver
 * instances are not useful.
 */
class CProgtestSolver {
public:
    virtual ~CProgtestSolver() noexcept = default;

    /**
     * Check the used capacity of the solver. Returns true if the used capacity is smaller than the limit.
     * @return true = there is a free space (capacity not fully used yet), false = no free capacity
     */
    virtual bool hasFreeCapacity() const = 0;

    /**
     * Add a new polygon instance to be solved.
     * @param[in] p                    the polygon to add
     * @return true = problem added, false = problem not added (already at the capacity)
     */
    virtual bool addPolygon(APolygon p) = 0;

    /**
     * Solve the problems previously stored into the solver.
     * @return the number of problems solved. Typically, 0 indicates an error (solve already called for the instance).
     */
    virtual size_t solve() = 0;
};

using AProgtestSolver = std::shared_ptr<CProgtestSolver>;
//=============================================================================================================================================================
/**
 * Factory functions to create a new instance of progtest solver for the TriangMin / TriangCnt problems.
 *
 * The delivered library creates solver instances such that the total number of solved problems is 100 (M=100).
 * This limit applies to the solver in the attached library only. The solvers in the Progtest testing environment
 * use different limits, the limits are set with respect to the size of input data used for the tests.
 */
AProgtestSolver createProgtestMinSolver();

AProgtestSolver createProgtestCntSolver();

#endif /* PROGTEST_SOLVER_H_9523464567576456 */
