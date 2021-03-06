// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "VvTransformer.h"

namespace Sgt
{
    arma::Mat<Complex> VvTransformer::inServiceY() const
    {
        if (!isValid_)
        {
            validate();
        }
        return Y_;
    }

    void VvTransformer::validate() const
    {
        // 1-3 windings:
        Complex ai = 1.0 / a13();
        Complex aci = conj(ai);
        Complex a2i = ai * aci;
        
        // 2-3 windings:
        Complex bi = 1.0 / a23();
        Complex bci = conj(bi);
        Complex b2i = ai * bci;

        Complex data[] =  
            {
                 a2i,          0,       -a2i,   -aci,      0,        aci,
                   0,        b2i,       -b2i,      0,   -bci,        bci,
                -a2i,       -b2i,    a2i+b2i,   -aci,   -bci,    aci+bci,
                 -ai,          0,         ai,      1,      0,         -1,
                   0,        -bi,         bi,      0,      1,         -1,
                 -ai,        -bi,      ai+bi,      1,      1,         -2
            };

        Y_ = arma::Mat<Complex>(6, 6, arma::fill::none);
        for (arma::uword i = 0; i < 6; ++i)
        {
            for (arma::uword j = 0; j < 6; ++j)
            {
                Y_(i, j) = YL_ * data[6 * i + j];
            }
        }
    }
};
