/*
* Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
*/
#ifndef _Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_inl_
#define _Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../LinearAlgebra/Matrix.h"
#include "../LinearAlgebra/Vector.h"

namespace Stroika {
    namespace Foundation {
        namespace Math {
            namespace Optimization {
                namespace DownhillSimplexMinimization {

                    /*
                     ********************************************************************************
                     ******************** DownhillSimplexMinimization::Run **************************
                     ********************************************************************************
                     */
                    template <typename FLOAT_TYPE>
                    Results<FLOAT_TYPE> Run (const TargetFunction<FLOAT_TYPE>& function2Minimize, const MinimizationParametersType<FLOAT_TYPE>& initialValues, const Options& options)
                    {
                        Results<FLOAT_TYPE> results{};

                        // Translated by hand from https://github.com/fchollet/nelder-mead/blob/master/nelder_mead.py - LGP 2017-02-20
                        auto nelder_mead_by_fchollet = [](
                            const TargetFunction<FLOAT_TYPE>&             f,
                            const MinimizationParametersType<FLOAT_TYPE>& x_start,
                            FLOAT_TYPE                                    step            = 0.1,
                            FLOAT_TYPE                                    no_improve_thr  = 10e-6,
                            unsigned int                                  no_improv_break = 10,
                            unsigned int                                  max_iter        = 0,
                            FLOAT_TYPE                                    alpha           = 1,
                            FLOAT_TYPE                                    gamma           = 2,
                            FLOAT_TYPE                                    rho             = -0.5,
                            FLOAT_TYPE                                    sigma           = 0.5) -> pair<MinimizationParametersType<FLOAT_TYPE>, FLOAT_TYPE> {

                            using namespace LinearAlgebra;

                            // init
                            size_t       dim       = x_start.size ();
                            FLOAT_TYPE   prev_best = f (x_start);
                            unsigned int no_improv = 0;
                            struct PartialResultType_ {
                                MinimizationParametersType<FLOAT_TYPE> fResults;
                                FLOAT_TYPE                             fScore;
                            };
                            vector<PartialResultType_> res = {PartialResultType_{x_start, prev_best}};
                            for (size_t i = 0; i != dim; ++i) {
                                MinimizationParametersType<FLOAT_TYPE> x = x_start;
                                x[i] += step;
                                FLOAT_TYPE score{f (x)};
                                res.push_back (PartialResultType_{x, score});
                            }

                            // Simplex iteration
                            unsigned int iters = 0;
                            while (true) {
                                std::sort (res.begin (), res.end (), [](auto l, auto r) { return l.fScore < r.fScore; });
                                FLOAT_TYPE best = res[0].fScore;

                                // break after max_iter
                                if (max_iter and iters >= max_iter) {
                                    return res[0];
                                }
                                iters += 1;

                                // break after no_improv_break iterations with no improvement
                                DbgTrace (L"...best so far: %s", Characters::ToString (best).c_str ());
                                if (best < prev_best - no_improve_thr) {
                                    no_improv = 0;
                                    prev_best = best;
                                }
                                else {
                                    no_improv += 1;
                                }
                                if (no_improv >= no_improv_break) {
                                    return res[0];
                                }

                                // Centroid
                                Vector<FLOAT_TYPE> x0{3, 0.0};
                                {
                                    auto removeLast = [](decltype (res) v) {
                                        decltype (res) tmp;
                                        for (auto i = v.begin (); i != v.end () and i + 1 != v.end (); ++i) {
                                            tmp.push_back (*i);
                                        }
                                        return tmp;
                                    };
                                    for (PartialResultType_ tup : removeLast (res)) {
                                        for (size_t i = 0; i < x0.GetDimension (); ++i) {
                                            FLOAT_TYPE c = tup[0][i];
                                            x0[i] += c / (res.size () - 1);
                                        }
                                    }
                                }

                                // Reflection
                                FLOAT_TYPE rscore{};
                                {
                                    Vector<FLOAT_TYPE> xr = x0 + alpha * (x0 - res[res.size () - 1].fResults);
                                    rscore                = f (xr);
                                    if (res[0].fScore <= rscore and rscore < res[res.size () - 2].fScore) {
                                        res[res.size () - 1] = PartialResultType_{xr, rscore};
                                        continue;
                                    }
                                }

                                // Expansion
                                {
                                    if (rscore < res[0].fScore) {
                                        Vector<FLOAT_TYPE> xe     = x0 + gamma * (x0 - res[res.size () - 1].fResults);
                                        FLOAT_TYPE         escore = f (xe);
                                        if (escore < rscore) {
                                            res[res.size () - 1] = PartialResultType_{xe, escore};
                                        }
                                        else {
                                            res[res.size () - 1] = PartialResultType_{xr, rscore};
                                        }
                                        continue;
                                    }
                                }

                                // Contraction
                                {
                                    Vector<FLOAT_TYPE> xc     = x0 + rho * (x0 - res[res.size () - 1].fResults);
                                    FLOAT_TYPE         cscore = f (xc);
                                    if (cscore < res[res.size () - 1].fResults) {
                                        res[res.size () - 1] = PartialResultType_{xc, cscore};
                                    }
                                    continue;
                                }

                                // Reduction
                                {
                                    Vector<FLOAT_TYPE>         x1 = res[0].fResults;
                                    vector<PartialResultType_> nres;
                                    for (PartialResultType_ tup : res) {
                                        Vector<FLOAT_TYPE> redx  = x1 + sigma * (tup[0] - x1);
                                        FLOAT_TYPE         score = f (redx);
                                        res.push_back (PartialResultType_{redx, score});
                                    }
                                    res = nres;
                                }
                            }
                            AssertNotReached ();
                            return pair<MinimizationParametersType<FLOAT_TYPE>, FLOAT_TYPE>{};
                        };

                        pair<MinimizationParametersType<FLOAT_TYPE>, FLOAT_TYPE> tmp = nelder_mead_by_fchollet (function2Minimize, initialValues);
                        results.fScore               = tmp.second;
                        results.fMinimizedParameters = tmp.first;
                        return results;
                    }
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_inl_*/
