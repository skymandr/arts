#pragma once

#include "atm.h"
#include "jacobian.h"
#include "matpack_arrays.h"
#include "ppath_struct.h"
#include "propagationmatrix.h"
#include "transmissionmatrix.h"
#include <tuple>

class Workspace;
class Agenda;

ArrayOfAtmPoint& atm_path_resize(ArrayOfAtmPoint&, const Ppath&);

void forward_atm_path(ArrayOfAtmPoint&, const Ppath&, const AtmField&);

ArrayOfAtmPoint forward_atm_path(const Ppath&, const AtmField&);

ArrayOfVector& path_freq_resize(ArrayOfVector&, const Vector&, const ArrayOfAtmPoint&);

void forward_ppath_freq(ArrayOfVector&, const Vector&, const Ppath&, const ArrayOfAtmPoint&, const Numeric);

ArrayOfVector forward_ppath_freq(const Vector&, const Ppath&, const ArrayOfAtmPoint&, const Numeric);

std::tuple<ArrayOfPropagationMatrix, ArrayOfRadiationVector, ArrayOfArrayOfPropagationMatrix, ArrayOfArrayOfRadiationVector>
forward_propmat(Workspace&, const Agenda&, const ArrayOfVector&, const ArrayOfAtmPoint&, const ArrayOfRetrievalQuantity&);