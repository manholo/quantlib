
/*
 Copyright (C) 2002, 2003 Ferdinando Ametrano

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file localvolcurve.hpp
    \brief Local volatility curve derived from a Black curve
*/

#ifndef quantlib_localvolcurve_hpp
#define quantlib_localvolcurve_hpp

#include <ql/Volatilities/blackvariancecurve.hpp>

namespace QuantLib {

    namespace VolTermStructures {

        //! Local volatility curve derived from a Black curve
        class LocalVolCurve : public LocalVolTermStructure,
                              public Patterns::Observer {
          public:
            // constructor
            LocalVolCurve(const RelinkableHandle<BlackVarianceCurve>& curve)
            : blackVarianceCurve_(curve) {
                registerWith(blackVarianceCurve_);
            }
            // inspectors
            Date referenceDate() const {
                return blackVarianceCurve_->referenceDate();
            }
            DayCounter dayCounter() const {
                return blackVarianceCurve_->dayCounter();
            }
            Date maxDate() const { 
                return blackVarianceCurve_->maxDate(); 
            }
            // Observer interface
            void update() {
                notifyObservers();
            }
          protected:
            double localVolImpl(Time, double, bool extrapolate) const;
          private:
            RelinkableHandle<BlackVarianceCurve> blackVarianceCurve_;
        };


        /*! The relation
            \f[
            \int_0^T \sigma_L^2(t)dt = \sigma_B^2 T
            \f]
            holds, where \f$ \sigma_L(t) \f$ is the local volatility at
            time \f$ t \f$ and \f$ \sigma_B(T) \f$ is the Black
            volatility for maturity \f$ T \f$. From the above, the formula
            \f[
            \sigma_L(t) = \sqrt{\frac{\mathrm{d}}{\mathrm{d}t}\sigma_B^2(t)t}
            \f]
            can be deduced which is here implemented.
        */
        inline double LocalVolCurve::localVolImpl(Time t, double dummy, 
                                                  bool extrapolate) const {

            double dt = (1.0/365.0);
            double var1 = blackVarianceCurve_->blackVariance(t, dummy,
                                                             extrapolate);
            double var2 = blackVarianceCurve_->blackVariance(t+dt, dummy,
                                                             true);
            double derivative = (var2-var1)/dt;
            return QL_SQRT(derivative);
        }

    }

}


#endif
